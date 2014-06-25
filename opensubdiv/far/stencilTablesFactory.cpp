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

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class StencilAllocator {

public:

    StencilAllocator(FarRefineTables const & refTables);

    ~StencilAllocator();

    FarStencil Allocate(int size);

    void PopulateTables(std::vector<char> & sizes, std::vector<int> & indices,
        std::vector<float> & weights) const;

    void PopulateTables(std::vector<FarStencil> const & farStencils,
        std::vector<char> & sizesVec, std::vector<int> & indicesVec,
            std::vector<float> & weightsVec) const;

    int GetNumStencils() const {
        return _numStencils;
    }

    int GetNumStencilElements() const {
        return _numStencilElements;
    }


    void PrintStencils() const;

    void PrintStats() const;

    static int GetMaxStencilSize() {
        return 40;
    }

private:

    // A fixed-size block of stencils that all share the same size
    struct Block {

        // Constructor
        Block(int size, int capacity) :
            size(size), capacity(capacity), used(0) { }

        FarStencil Allocate();

        void CopyData(int ** sizes, int ** indices, float ** weights);

        // Returns a pointer to the aggregated stencil indices
        int * GetIndices() {
            return data;
        }

        // Returns a pointer to the aggregated stencil weights
        float * GetWeights() {
            return reinterpret_cast<float *>(data + capacity*size);
        }

        bool IsFull() const {
            return (used==capacity);
        }

        char size;
        int capacity,
            used;
        int data[1];
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

    int getCapacityHint(int size) const {
        assert(size<(int)_capacityHints.size());
        return _capacityHints[size];
    }

private:

    int _numStencils,         // total number of stencils allocated
        _numStencilElements;  // total number of stencil elements

    typedef std::list<Block *> BlockList;

    std::vector<BlockList> _blocks;

    std::vector<int> _capacityHints;
};

// Destructor
StencilAllocator::~StencilAllocator() {

    for (int i=0; i<GetMaxStencilSize(); ++i) {
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

    _blocks.resize(GetMaxStencilSize());

    { // populate capacity hints
        int nvertstotal = refTables.GetNumVerticesTotal();

        int defaultHint = std::max(10, nvertstotal/10);
        _capacityHints.resize(GetMaxStencilSize(), defaultHint);

        // XXXX manuelk - i need to come up with a real strategy
        _capacityHints[3]=std::max(10, refTables.GetNumFaces(0));
        _capacityHints[5]=std::max(10, nvertstotal/5);
        _capacityHints[6]=std::max(10, nvertstotal/2);
        _capacityHints[7]=std::max(10, nvertstotal/3);
    }

    { // setup block for coarse vertices (stencils have a single weight=1)
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

void
StencilAllocator::PopulateTables(std::vector<FarStencil> const & farStencils,
    std::vector<char> & sizesVec, std::vector<int> & indicesVec,
        std::vector<float> & weightsVec) const {

    int nstencils = (int)farStencils.size(),
        nelements = 0;

    for (int i=0; i<nstencils; ++i) {
        nelements+=farStencils[i].GetSize();
    }

    sizesVec.resize(nstencils);
    indicesVec.resize(nelements);
    weightsVec.resize(nelements);

    char * sizes = &sizesVec.at(0);
    int * indices = &indicesVec.at(0);
    float * weights = &weightsVec.at(0);

    // copy data
    for (int i=0; i<nstencils; ++i) {

        FarStencil const & stencil = farStencils[i];

        int n = stencil.GetSize();

        *sizes=(char)n;
        memcpy(indices, stencil.GetVertexIndices(), n*sizeof(int));
        memcpy(weights, stencil.GetWeights(), n*sizeof(float));

        ++sizes;
        indices+=n;
        weights+=n;
    }
}

void
StencilAllocator::PopulateTables(std::vector<char> & sizesVec,
     std::vector<int> & indicesVec, std::vector<float> & weightsVec) const {

    sizesVec.resize(GetNumStencils());
    indicesVec.resize(GetNumStencilElements());
    weightsVec.resize(GetNumStencilElements());

    char * sizes = &sizesVec.at(0);
    int * indices = &indicesVec.at(0);
    float * weights = &weightsVec.at(0);

    for (int size=GetMaxStencilSize()-1; size>0; --size) {

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

void
StencilAllocator::PrintStencils() const {

    for (int size=GetMaxStencilSize()-1; size>=0; --size) {

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

void
StencilAllocator::PrintStats() const {

    std::vector<int> nstencils(GetMaxStencilSize(),0),
                     nblocks(GetMaxStencilSize(),0),
                     unused(GetMaxStencilSize(),0);

    for (int i=0; i<GetMaxStencilSize(); ++i) {
        BlockList const & block = _blocks[i];
        for (BlockList::const_iterator it=block.begin(); it!=block.end(); ++it) {
            nstencils[i] += (*it)->used;
            unused[i] += (*it)->capacity - (*it)->used;
            ++nblocks[i];
        }
    }

    printf("Stencil Size    : ");
    for (int i=0; i<(int)nstencils.size(); ++i) {
        printf("|%-4d", i+1);
    }
    printf("|\nN Blocks        : ");
    for (int i=0; i<(int)nstencils.size(); ++i) {
        printf("|%-4d", nblocks[i]);
    }
    printf("|\nN Stencils      : ");
    for (int i=0; i<(int)nstencils.size(); ++i) {
        printf("|%-4d", nstencils[i]);
    }
    printf("|\nN Unused        : ");
    for (int i=0; i<(int)nstencils.size(); ++i) {
        printf("|%-4d", unused[i]);
    }
    printf("|\n");
}


class TempStencilAllocator;

// Temporary data structure to gather the stencils from supporting vertices
// that aren't fully discovered yet.
// Strategy: allocate up-front a data pool for supporting stencils of slightly
// above than average size. For (rare) stencils that require more support
// vertices, switch to (much slower) dynamic allocation.
class TempStencil {

public:

    int GetID() const {
        return _ID;
    }

    void Clear();

    void AddWithWeight(FarStencil const & stencil, float weight);

    void AddWithWeight(TempStencil const & stencil, float weight);

    int GetSize() const;

    int const * GetIndices() const;

    float const * GetWeights() const;

    void Print() const;

private:

    friend class TempStencilAllocator;

    // returns local stencil index of vertex or -1 if not found
    int findVertex(int idx);

private:

    int _ID;                       // stencil ID in allocator

    TempStencilAllocator * _alloc; // make allocater a singleton and we can
                                   // get rid of this - but lose concurrency
};


// TempStencil pool allocator
class TempStencilAllocator {

public:

    ~TempStencilAllocator() {
        for (int i=0; i<(int)_bigstencils.size(); ++i) {
            delete _bigstencils[i];
        }
    }

    TempStencilAllocator(int nstencils, int maxvalues=40) :
        _nstencils(nstencils), _maxvalues(maxvalues) {

        _tempStencils.resize(nstencils);
        for (int i=0; i<nstencils; ++i) {
            _tempStencils[i]._ID = i;
            _tempStencils[i]._alloc = this;
        }

        int nelems = nstencils * maxvalues;
        _sizes.resize(nstencils,0);
        _indices.resize(nelems);
        _weights.resize(nelems);
    }

    std::vector<TempStencil> & GetStencils() {
        return _tempStencils;
    }

    void PushBackVertex(TempStencil & stencil, int index, float weight) {

        int   * size    = getSize(stencil),
              * indices = getIndices(stencil);
        float * weights = getWeights(stencil);

        // try fast alloc path first
        if (*size<(_maxvalues-1)) {
            // we have space left for a new vertex
            indices[*size]=index;
            weights[*size]=weight;
        } else {
            // this is a big stencil - use slow alloc path
            BigStencil * dst=0;
            if (stencil.GetID()<(int)_bigstencils.size()) {
                dst = _bigstencils[stencil.GetID()];
            } else {
                _bigstencils.push_back( dst=new BigStencil );
                // copy existing stencil data
                for (int i=0; i<*size; ++i) {
                    dst->indices.push_back(indices[i]);
                    dst->weights.push_back(weights[i]);
                }
            }
            dst->indices.push_back(index);
            dst->weights.push_back(weight);
        }
        ++(*size);
    }

private:

    friend class TempStencil;

    int * getSize(TempStencil const & stencil) {
        assert(stencil.GetID()<(int)_sizes.size());
        return &_sizes[stencil.GetID()];
    }

    int * getIndices(TempStencil const & stencil) {
        if (*getSize(stencil)<_maxvalues) {
            return &_indices[stencil.GetID()*_maxvalues];
        } else {
            return &_bigstencils[stencil.GetID()]->indices[0];
        }
    }

    float * getWeights(TempStencil const & stencil) {
        if (*getSize(stencil)<_maxvalues) {
            return &_weights[stencil.GetID()*_maxvalues];
        } else {
            return &_bigstencils[stencil.GetID()]->weights[0];
        }
    }

private:

    int _nstencils,
        _maxvalues,
        _current;

    std::vector<TempStencil> _tempStencils;

    std::vector<int>   _sizes;
    std::vector<int>   _indices;
    std::vector<float> _weights;

    struct BigStencil {
        std::vector<int>   indices;
        std::vector<float> weights;
    };

    std::vector<BigStencil *> _bigstencils;
};

void
TempStencil::Clear() {
    for (int i=0; i<*_alloc->getSize(*this); ++i) {
        float * weights = _alloc->getWeights(*this);
        weights[i]=0.0f;
    }
}

void
TempStencil::AddWithWeight(FarStencil const & stencil, float weight) {

    assert(weight>0.0f);

    for (int i=0; i<stencil.GetSize(); ++i) {

        int vertIndex = stencil.GetVertexIndices()[i],
            n = findVertex(vertIndex);

        if (n<0) {
            _alloc->PushBackVertex(*this, vertIndex, weight);
        } else {
            float * weights = _alloc->getWeights(*this);
            weights[n] += weight * stencil.GetWeights()[i];
        }
    }
}

void
TempStencil::AddWithWeight(TempStencil const & stencil, float weight) {

    assert(weight>0.0f);

    int const * size    = _alloc->getSize(stencil),
              * indices = _alloc->getIndices(stencil);
    float const * weights = _alloc->getWeights(stencil);

    for (int i=0; i<*size; ++i) {

        int vertIndex = indices[i],
            n = findVertex(vertIndex);

        if (n<0) {
            _alloc->PushBackVertex(*this, vertIndex, weight);
        } else {
            float * dst = _alloc->getWeights(*this);
            dst[n] += weight * weights[i];
        }
    }
}

int
TempStencil::GetSize() const {
    return *_alloc->getSize(*this);
}

int const *
TempStencil::GetIndices() const {
    return _alloc->getIndices(*this);
}

float const *
TempStencil::GetWeights() const {
    return _alloc->getWeights(*this);
}

inline int
TempStencil::findVertex(int idx) {
    // XXXX serial serial search for now...
    int * size    = _alloc->getSize(*this),
        * indices = _alloc->getIndices(*this);
    for (int i=0; i<*size; ++i) {
        if (indices[i]==idx)
            return i;
    }
    return -1;
}

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



FarStencilTables const *
FarStencilTablesFactory::Create(FarRefineTables const & refTables) {

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

        TempStencilAllocator tempalloc(nverts);

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

    //alloc.PopulateTables(result->_sizes, result->_indices, result->_weights);
    alloc.PopulateTables(stencils, result->_sizes, result->_indices, result->_weights);

//alloc.PrintStencils();
//alloc.PrintStats();

    return result;
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
