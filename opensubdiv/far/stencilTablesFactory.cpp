//
//   Copyright 2014 DreamWorks Animation LLC.
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

#include "../far/stencilTables.h"
#include "../far/stencilTablesFactory.h"
#include "../far/refineTables.h"

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <list>
#include <vector>

#define MAX_STENCIL_SIZE 40

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
// A pool allocator for stencils of known sizes
//
class StencilAllocator {

public:

    StencilAllocator(FarRefineTables const & refTables);

    ~StencilAllocator();

    // Allocates pool memory for a stencil of size 'size'
    FarStencil Allocate(int size);

    // Returns the number of stencils allocated
    int GetNumStencils() const {
        return _numStencils;
    }

    // Returns the number stencil elements allocated (indices, weights)
    int GetNumStencilElements() const {
        return _numStencilElements;
    }

    // Gather all the stencil data from the allocator and copy it into vectors
    void PopulateTables(std::vector<unsigned char> & sizes,
        std::vector<int> & indices, std::vector<float> & weights) const;

    // Gather the data for a selection of stencils and copy it from the
    // allocator into vectors
    void PopulateTables(std::vector<FarStencil> const & farStencils,
        std::vector<unsigned char> & sizesVec, std::vector<int> & indicesVec,
            std::vector<float> & weightsVec) const;

    // Prints all stencil data on console
    void PrintStencils() const;

    // Print pool allocation stats
    void PrintStats() const;

private:

    //
    // A block of stencils that all share the same size
    //
    struct Block {

        // Constructor
        Block(int size, int capacity) :
            size(size), capacity(capacity), used(0) { }

        // Returns a FarStencil pointing to the next unused allocated space
        FarStencil Allocate();

        // Copy all stencil data from the block and advance the pointers
        void CopyData(int ** sizes, int ** indices, float ** weights);

        // Returns a pointer to the aggregated stencil indices
        int * GetIndices() {
            return data;
        }

        // Returns a pointer to the aggregated stencil weights
        float * GetWeights() {
            return reinterpret_cast<float *>(data + capacity*size);
        }

        // True if the block is fully used
        bool IsFull() const {
            return (used==capacity);
        }

        unsigned char size;       // stencil size

        int           capacity,   // block capacity (number of stencils)
                      used;       // number of stencils in use
        int           data[1];    // block of stencil data
    };

private:

    // Allocates a block of stencils of size 'size' and insert it at the front
    // of the list.
    // Note: assuming capacity hints are robust, we do not expect to perform
    // this allocation often (if at all).
    Block * allocateBlock(int size, int capacity) {
        assert(size>0);
        int n = size * capacity;
        Block * block = (Block *)malloc(sizeof(Block) + n*(sizeof(int)+sizeof(float)));
        new (block) Block(size,capacity);
        _blocks[size-1].push_front(block);
        return block;
    }

    // Returns the vector of block-capacity hints
    int getCapacityHint(int size) const {
        assert(size<(int)_capacityHints.size());
        return _capacityHints[size];
    }

private:

    int _numStencils,         // Total number of stencils allocated
        _numStencilElements;  // Total number of stencil elements

    std::vector<int> _capacityHints; // Array of hints to initialize block
                                     // capacities for each stencil size

    typedef std::list<Block *> BlockList;

    std::vector<BlockList> _blocks; // Array of BlockLists for each stencil size
};

// Destructor
StencilAllocator::~StencilAllocator() {

    for (int i=0; i<FarStencilTablesFactory::GetMaxStencilSize(); ++i) {
        BlockList & block = _blocks[i];
        for (BlockList::iterator it=block.begin(); it!=block.end(); ++it) {
            assert(*it);
            free(*it);
        }
    }
}

// Constructor
StencilAllocator::StencilAllocator(FarRefineTables const & refTables) :
    _numStencils(0), _numStencilElements(0) {

    int maxSize = FarStencilTablesFactory::GetMaxStencilSize();

    _blocks.resize(maxSize);

    { // populate capacity hints
        int nvertstotal = refTables.GetNumVerticesTotal();

        int defaultHint = std::max(10, nvertstotal/10);
        _capacityHints.resize(maxSize, defaultHint);

        // XXXX manuelk - need to come up with some stats and a real strategy...
        _capacityHints[3]=std::max(10, refTables.GetNumFaces(0));
        _capacityHints[5]=std::max(10, nvertstotal/500);
        _capacityHints[6]=std::max(10, nvertstotal/100);
        _capacityHints[7]=std::max(10, nvertstotal/2);
    }

    { // allocate initial block for coarse vertices (stencils have a single weight=1)
        int nverts = refTables.GetNumVertices(0);
        allocateBlock(1, nverts);
    }
}

// Allocate a FarStencil
inline FarStencil
StencilAllocator::Allocate(int size) {

    int blockIndex = size-1;

    assert(size<(int)_blocks.size());

    BlockList & blocklist = _blocks[blockIndex];

    Block * block=0;
    if (blocklist.empty()) {
        block = allocateBlock(size, getCapacityHint(blockIndex));
    } else {
        block = blocklist.front();
    }
    if (block->IsFull()) {
        block = allocateBlock(size, getCapacityHint(blockIndex));
    }

    int offset = size * block->used,
        * indices = block->GetIndices() + offset;
    float * weights = block->GetWeights() + offset;

    ++block->used;

    if (blockIndex>0) {
        ++_numStencils;
        _numStencilElements+=size;
    }
    return FarStencil(&block->size, indices, weights);
}

// Gather all the stencil data from the allocator and copy it into vectors
void
StencilAllocator::PopulateTables(std::vector<FarStencil> const & farStencils,
    std::vector<unsigned char> & sizesVec, std::vector<int> & indicesVec,
        std::vector<float> & weightsVec) const {

    int nstencils = (int)farStencils.size(),
        nelements = 0;

    for (int i=0; i<nstencils; ++i) {
        nelements+=farStencils[i].GetSize();
    }

    sizesVec.resize(nstencils);
    indicesVec.resize(nelements);
    weightsVec.resize(nelements);

    unsigned char * sizes = &sizesVec.at(0);
    int * indices = &indicesVec.at(0);
    float * weights = &weightsVec.at(0);

    // Copy data
    for (int i=0; i<nstencils; ++i) {

        FarStencil const & stencil = farStencils[i];

        int n = stencil.GetSize();

        *sizes=(unsigned char)n;
        memcpy(indices, stencil.GetVertexIndices(), n*sizeof(int));
        memcpy(weights, stencil.GetWeights(), n*sizeof(float));

        ++sizes;
        indices+=n;
        weights+=n;
    }
}

// Gather the data for a selection of stencils and copy it from the
// allocator into vectors
void
StencilAllocator::PopulateTables(std::vector<unsigned char> & sizesVec,
     std::vector<int> & indicesVec, std::vector<float> & weightsVec) const {

    sizesVec.resize(GetNumStencils());
    indicesVec.resize(GetNumStencilElements());
    weightsVec.resize(GetNumStencilElements());

    unsigned char * sizes = &sizesVec.at(0);
    int * indices = &indicesVec.at(0);
    float * weights = &weightsVec.at(0);

    for (int size=FarStencilTablesFactory::GetMaxStencilSize()-1; size>0; --size) {

        BlockList const & blocklist = _blocks[size];

        for (BlockList::const_iterator it=blocklist.begin();
            it!=blocklist.end(); ++it) {

            Block * block = *it;

            // copy data
            for (int i=0; i<block->used; ++i) {
                sizes[i]=block->size;
            }

            int n = block->size * block->used;
            memcpy(indices, block->GetIndices(), n*sizeof(int));
            memcpy(weights, block->GetWeights(), n*sizeof(float));

            // advance pointers
            sizes+=block->used;
            indices+=n;
            weights+=n;
        }
    }
}

// Debug dumps
void
StencilAllocator::PrintStencils() const {

    for (int size=FarStencilTablesFactory::GetMaxStencilSize()-1; size>=0; --size) {

        BlockList const & blocklist = _blocks[size];

        for (BlockList::const_iterator it=blocklist.begin();
            it!=blocklist.end(); ++it) {

            Block * block = *it;

            for (int i=0; i<block->used; ++i) {
                printf("stencil size=%d { indices={ ", block->size);
                for (int j=0; j<block->size; ++j) {
                    printf("%d ", block->GetIndices()[j]);
                }
                printf("} weights={ ");
                for (int j=0; j<block->size; ++j) {
                    printf("%f ", block->GetWeights()[j]);
                }
                printf("} }\n");
            }
        }
    }
}

static void
PrintLine(std::vector<int> const & nblocks) {
    printf("    +----------------+");
    for (int i=0; i<(int)nblocks.size(); ++i) {
        if (nblocks[i])
            printf("------+");
    }
    printf("\n");
}

static void
PrintStat(char const * name, std::vector<int> const & nblocks,
    std::vector<int> const & data) {

    PrintLine(nblocks);
    printf("    | %-15s", name);
    for (int i=0; i<(int)nblocks.size(); ++i) {
        if (nblocks[i])
            printf("|% 6d", data[i]);
    }
    printf("|\n");
}

void
StencilAllocator::PrintStats() const {

    int maxSize = FarStencilTablesFactory::GetMaxStencilSize();

    std::vector<int> sizes(maxSize,0),
                     nstencils(maxSize,0),
                     nblocks(maxSize,0),
                     unused(maxSize,0);

    for (int i=0; i<maxSize; ++i) {
        BlockList const & block = _blocks[i];
        for (BlockList::const_iterator it=block.begin(); it!=block.end(); ++it) {
            sizes[i] = (*it)->size;
            nstencils[i] += (*it)->used;
            unused[i] += (*it)->capacity - (*it)->used;
            ++nblocks[i];
        }
    }

    PrintStat("Stencil Size", nblocks, sizes);
    PrintStat("NumBlocks",    nblocks, nblocks);
    PrintStat("NumStencils",  nblocks, nstencils);
    PrintStat("NumUnused",    nblocks, unused);
    PrintLine(nblocks);

    printf("\n    NumStencilsTotal=%d\n", GetNumStencils());
    printf("    NumStencilElementsTotal=%d\n\n", GetNumStencilElements());
}


class TempStencilAllocator;

// Temporary data structure to gather the stencils from supporting vertices
// that aren't fully discovered yet.
//
// Strategy: allocate up-front a data pool for supporting stencils of a size
// slightly above average. For the (rare) stencils that require more support
// vertices, switch to regular heap allocation (slower).
//
class TempStencil {

public:

    // Return stencil unique ID
    int GetID() const {
        return _ID;
    }

    // Set stencil weights to 0.0
    void Clear();

    // Weighted add of a FarStencil
    void AddWithWeight(FarStencil const & src, float weight);

    // Weighted add of a TempStencil
    void AddWithWeight(TempStencil const & src, float weight);

    // Returns the current size of the TempStencil
    int GetSize() const;

    // Returns a pointer to the vertex indices of the stencil
    int const * GetIndices() const;

    // Returns a pointer to the vertex weights of the stencil
    float const * GetWeights() const;

    void Print() const;

private:

    friend class TempStencilAllocator;

    // Find the location of vertex 'vertex' in the stencil indices.
    // Returns -1 if not found
    int findVertex(int vertex);

private:

    int _ID;                       // stencil ID in allocator

    TempStencilAllocator * _alloc; // make allocator a singleton and we can
                                   // get rid of this - but we lose concurrency
};


// TempStencil pool allocator
class TempStencilAllocator {

public:

    ~TempStencilAllocator() {
        for (int i=0; i<(int)_bigstencils.size(); ++i) {
            delete _bigstencils[i];
        }
    }

    TempStencilAllocator(FarRefineTables const & refTables, int capacity, int maxsize=-1) {

        // Make an educated guess as to what the max size should be
        if (maxsize<0) {

            SdcType type = refTables.GetSchemeType();

                   if (type==TYPE_BILINEAR) {
                _maxsize = 12;
            } else if (type==TYPE_CATMARK) {
                _maxsize = 12;
            } else if (type==TYPE_LOOP) {
                _maxsize = 12;
            }
        }

        // Pre-allocate the TempStencils
        _tempStencils.resize(capacity);
        for (int i=0; i<capacity; ++i) {
            _tempStencils[i]._ID = i;
            _tempStencils[i]._alloc = this;
        }

        int nelems = capacity * _maxsize;
        _sizes.resize(capacity,0);
        _indices.resize(nelems);
        _weights.resize(nelems);
    }

    // Returns an array of all the TempStencils in the allocator
    std::vector<TempStencil> & GetStencils() {
        return _tempStencils;
    }

    // Append a support vertex of index 'index' and weight 'weight' to the
    // TempStencil 'stencil' (use findVertex() to make sure it does not exist
    // yet)
    void PushBackVertex(TempStencil & stencil, int index, float weight) {

        int   * size    = getSize(stencil),
              * indices = getIndices(stencil);
        float * weights = getWeights(stencil);

        if (*size<(_maxsize-1)) {

            // The stencil still fits in pool memory, just copy the data
            indices[*size]=index;
            weights[*size]=weight;
        } else {

            // The stencil is now too big: fall back to heap memory
            BigStencil * dst=0;

            // Is this a new BigStencil or are we using an existing one ?
            if (*size==(_maxsize-1)) {
                stencil._ID = (int) _bigstencils.size();
                dst = new BigStencil(*size, indices, weights);
                _bigstencils.push_back(dst);
            } else {
                dst = _bigstencils[stencil.GetID()];
            }
            assert(dst);

            // push back the new vertex
            dst->indices.push_back(index);
            dst->weights.push_back(weight);
        }
        ++(*size);
    }

private:

    friend class TempStencil;

    // returns the size of the stencil
    int * getSize(TempStencil const & stencil) {
        assert(stencil.GetID()<(int)_sizes.size());
        return &_sizes[stencil.GetID()];
    }

    // returns the indices of the stencil
    int * getIndices(TempStencil const & stencil) {
        if (*getSize(stencil)<_maxsize) {
            return &_indices[stencil.GetID()*_maxsize];
        } else {
            return &_bigstencils[stencil.GetID()]->indices[0];
        }
    }

    // returns the weights of the stencil
    float * getWeights(TempStencil const & stencil) {
        if (*getSize(stencil)<_maxsize) {
            return &_weights[stencil.GetID()*_maxsize];
        } else {
            return &_bigstencils[stencil.GetID()]->weights[0];
        }
    }

private:

    int _maxsize; // maximum size of a pre-allocated stencil

    std::vector<TempStencil> _tempStencils;

    std::vector<int>   _sizes;    // temp stencils data (as SOA)
    std::vector<int>   _indices;
    std::vector<float> _weights;

    // When stencils exceed _maxsize, fall back to heap allocated "BigStencils"
    struct BigStencil {

        BigStencil(int size, int const * iindices, float const * iweights) {
            indices.resize(size+1);
            weights.resize(size+1);
            memcpy(&indices.at(0), iindices, size*sizeof(int) );
            memcpy(&weights.at(0), iweights, size*sizeof(int) );
        }

        std::vector<int>   indices;
        std::vector<float> weights;
    };

    std::vector<BigStencil *> _bigstencils;
};

// Set stencil weights to 0.0
void
TempStencil::Clear() {
    for (int i=0; i<*_alloc->getSize(*this); ++i) {
        float * weights = _alloc->getWeights(*this);
        weights[i]=0.0f;
    }
}

// Weighted add of a FarStencil
inline void
TempStencil::AddWithWeight(FarStencil const & src, float weight) {

    assert(weight>0.0f);

    for (int i=0; i<src.GetSize(); ++i) {

        int vertIndex = src.GetVertexIndices()[i];

        // Attempt to locate the vertex index in the list of supporting vertices
        // of the destination stencil.
        int n = findVertex(vertIndex);

        if (n<0) {
            _alloc->PushBackVertex(*this, vertIndex, weight * src.GetWeights()[i]);
        } else {
            assert(src.GetWeights()[i]);
            float * dst = _alloc->getWeights(*this);
            dst[n] += weight * src.GetWeights()[i];
        }
    }
}

// Weighted add of a TempStencil
inline void
TempStencil::AddWithWeight(TempStencil const & src, float weight) {

    assert(weight>0.0f);

    int const * size = _alloc->getSize(src),
              * srcIndices = _alloc->getIndices(src);
    float const * srcWeights = _alloc->getWeights(src);

    for (int i=0; i<*size; ++i) {

        int vertIndex = srcIndices[i];

        // Attempt to locate the vertex index in the list of supporting vertices
        // of the destination stencil.
        int n = findVertex(vertIndex);
        if (n<0) {
            _alloc->PushBackVertex(*this, vertIndex, weight * srcWeights[i]);
        } else {
            assert(src.GetWeights()[i]);
            float * dst = _alloc->getWeights(*this);
            dst[n] += weight * srcWeights[i];
        }
    }
}

// Returns the current size of the TempStencil
int
TempStencil::GetSize() const {
    return *_alloc->getSize(*this);
}

// Returns a pointer to the vertex indices of the stencil
int const *
TempStencil::GetIndices() const {
    return _alloc->getIndices(*this);
}

// Returns a pointer to the vertex weights of the stencil
float const *
TempStencil::GetWeights() const {
    return _alloc->getWeights(*this);
}

// Find the location of vertex 'vertex' in the stencil indices.
inline int
TempStencil::findVertex(int vertex) {

    // XXXX serial serial search for now...
    int * size    = _alloc->getSize(*this),
        * indices = _alloc->getIndices(*this);
    for (int i=0; i<*size; ++i) {
        if (indices[i]==vertex)
            return i;
    }
    return -1;
}

// Debug dump
void
TempStencil::Print() const {

    printf("tempStencil size=%d indices={ ", GetSize());
    for (int i=0; i<GetSize(); ++i) {
        printf("%d ", GetIndices()[i]);
    }
    printf("} weights={ ");
    for (int i=0; i<GetSize(); ++i) {
        printf("%f ", GetWeights()[i]);
    }
    printf("}\n");
}


//
// Returns the largest stencil size that can be accomodated by the allocators
//
int
FarStencilTablesFactory::GetMaxStencilSize() {
    // Stencil size is a uchar - so max stencil size is limited to 255!
    return MAX_STENCIL_SIZE;
}

//
// FarStencilTables factory
//
FarStencilTables const *
FarStencilTablesFactory::Create(FarRefineTables const & refTables,
    bool allLevels) {

    StencilAllocator alloc(refTables);

    int ncoarseverts = refTables.GetNumVertices(0);

    // generate stencils for coarse level
    std::vector<FarStencil> stencils(ncoarseverts);
    for (int vert=0; vert<ncoarseverts; ++vert) {
        stencils[vert] = alloc.Allocate(1);
        stencils[vert]._indices[0] = vert;
        stencils[vert]._weights[0] = 1.0f;
    }

    // interpolate stencils for each level
    for (int level=1;level<=refTables.GetMaxLevel(); ++level) {

        int nverts = refTables.GetNumVertices(level);

        TempStencilAllocator tempalloc(refTables, nverts);

        std::vector<TempStencil> & tempstencils = tempalloc.GetStencils();

        // interpolate stencils
        refTables.InterpolateLevel(level, &stencils[0], &tempstencils[0]);

        // copy accumulated support rings data into stencils
        {
            // FarStencil vector is recycled and used as source for next level
            stencils.resize(nverts);
            for (int vert=0; vert<nverts; ++vert) {

                TempStencil const & tempst = tempstencils[vert];

                int size = tempst.GetSize();
                stencils[vert] = alloc.Allocate(size);
                memcpy(stencils[vert]._indices, tempst.GetIndices(), size*sizeof(int));
                memcpy(stencils[vert]._weights, tempst.GetWeights(), size*sizeof(float));
            }
        }
    }

    FarStencilTables * result = new FarStencilTables;

    if (allLevels) {
        // these stencils are sorted by size
        alloc.PopulateTables(result->_sizes, result->_indices, result->_weights);
    } else {
        // these stencils are *NOT* sorted by size
        alloc.PopulateTables(stencils, result->_sizes, result->_indices, result->_weights);
    }

    //alloc.PrintStats();

    return result;
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
