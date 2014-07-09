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
#ifndef VTR_LEVEL_H
#define VTR_LEVEL_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/crease.h"
#include "../vtr/types.h"

#include <vector>
#include <cassert>
#include <cstring>


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//  Forward declarations for friends:
template <class MESH> class FarRefineTablesFactory;
class FarRefineTablesFactoryBase;
class FarRefineTables;
class VtrRefinement;

//
//  VtrLevel:
//      A refinement level includes a vectorized representation of the topology
//  for a particular subdivision level.  The topology is "complete" in that any
//  level can be used as the base level of another subdivision hierarchy and can
//  be considered a complete mesh independent of its ancestors.  It currently
//  does contain a "depth" member -- as some inferences can then be made about
//  the topology (i.e. all quads or all tris if not level 0) but that is still
//  under consideration (e.g. a "regular" flag would serve the same purpose, and
//  level 0 may even be regular).
//
//  This class is intended for private use within the library.  So really its
//  interface should be fully protected and only those library classes that need
//  it will be declared as friends, e.g. VtrRefinement.
//
//  The represenation of topology here is to store six topological relationships
//  in tables of integers.  Each is stored in its own array(s) so the result is
//  a SOA representation of the topology.  The six relations are:
//
//      - face-verts:  vertices incident/comprising a face
//      - face-edges:  edges incident a face
//      - edge-verts:  vertices incident/comprising an edge
//      - edge-faces:  faces incident an edge
//      - vert-faces:  faces incident a vertex
//      - vert-edges:  edges incident a vertex
//
//  There is some redundancy here but the intent is not that this be a minimal
//  represenation, the intent is that it be amenable to refinement.  Classes in
//  the Far layer essentially store 5 of these 6 in a permuted form -- we add
//  the face-edges here to simplify refinement.
//
//  Notes/limitations/stuff to do -- much of this code still reflects the early days
//  when it was being prototyped and so it does not conform to OSD standards in many
//  ways:
//      - superficial stylistic issues:
//          . replacing "m" prefix with "_" on member variables
//              - done
//          . replace "access" and "modify" prefixes on VtrArray methods with "get"
//              - done
//          - replace use of "...Count" with "GetNum..."
//          - use of Vertex vs Vert in non-local (or any?) methods
//      - review public/protected/friend accessibility
//
//  Most are more relevant to the refinement, which used to be part of this class:
//      - short cuts that need revisiting:
//          - support for face-vert counts > 4
//          - support for edge-face counts > 2
//          - some neighborhood searches avoidable with more "local indexing"
//      - identify (informally) where scheme-specific code will be needed, so far:
//          - topological splitting and associated marking
//              - really the only variable here is whether to generate tris or
//                quads from non-quads
//          - interpolation
//              - potentially not part of the pre-processing required for FarMesh
//        and where it appears *not* to be needed:
//          - subdivision of sharpness values
//          - classification of vertex type/mask
//      - apply classification of vertices (computing Rules or masks)
//          - apply to base/course level on conversion
//          - apply to child level after subdivision of sharpness
//      - keep in mind desired similarity with FarMesh tables for ease of transfer
//        (contradicts previous point to some degree)
//

class VtrLevel
{
public:
    //
    //  Simple nested types to hold the tags for each component type -- some of
    //  which are user-specified features (e.g. whether a face is a hole or not)
    //  while others indicate the topological nature of the component, how it
    //  is affected by creasing in its neighborhood, etc.
    //
    //  Most of these properties are passed down to child components during
    //  refinement, but some -- notably the designation of a component as semi-
    //  sharp -- require re-determination as sharpnes values are reduced at each
    //  level.
    //
    struct VTag {
        typedef unsigned short VTagSize;

        VTag() { }

        VTagSize _nonManifold  : 1;  // fixed
        VTagSize _xordinary    : 1;  // fixed
        VTagSize _boundary     : 1;  // fixed
        VTagSize _infSharp     : 1;  // fixed
        VTagSize _semiSharp    : 1;  // variable
        VTagSize _rule         : 4;  // variable when _semiSharp

        //  On deck -- coming soon...
        //VTagSize _constSharp   : 1;  // variable when _semiSharp
        //VTagSize _hasEdits     : 1;  // variable
        //VTagSize _editsApplied : 1;  // variable
    };
    struct ETag {
        typedef unsigned char ETagSize;

        ETag() { }

        ETagSize _nonManifold  : 1;  // fixed
        ETagSize _boundary     : 1;  // fixed
        ETagSize _infSharp     : 1;  // fixed
        ETagSize _semiSharp    : 1;  // variable
    };
    struct FTag {
        typedef unsigned char FTagSize;

        FTag() { }

        FTagSize _hole  : 1;  // fixed

        //  On deck -- coming soon...
        //FTagSize _hasEdits : 1;  // variable
    };

    VTag getFaceCompositeVTag(VtrIndexArray const& faceVerts) const;

public:
    VtrLevel();
    ~VtrLevel();

    //  Simple accessors:
    int getDepth() const { return _depth; }

    int getNumVertices() const { return _vertCount; }
    int getNumFaces() const    { return _faceCount; }
    int getNumEdges() const    { return _edgeCount; }

    //  More global sizes may prove useful...
    int getNumFaceVerticesTotal() const { return (int) _faceVertIndices.size(); }
    int getNumFaceEdgesTotal() const    { return (int) _faceEdgeIndices.size(); }
    int getNumEdgeVerticesTotal() const { return (int) _edgeVertIndices.size(); }
    int getNumEdgeFacesTotal() const    { return (int) _edgeFaceIndices.size(); }
    int getNumVertexFacesTotal() const  { return (int) _vertFaceIndices.size(); }
    int getNumVertexEdgesTotal() const  { return (int) _vertEdgeIndices.size(); }

    //  Methods to access the relation tables/indices -- note that for some relations
    //  we store an additional "local index", e.g. for the case of vert-faces if one
    //  of the faces F[i] is incident a vertex V, then L[i] is the "local index" in
    //  F[i] of vertex V.  Once have only quads (or tris), this local index need only
    //  occupy two bits and could conceivably be packed into the same integer as the
    //  face index, but for now, given the need to support faces of potentially high
    //  valence we'll us an 8- or 16-bit integer.
    //
    //  Methods to access the six topological relations:
    VtrIndexArray const getFaceVertices(VtrIndex faceIndex) const;
    VtrIndexArray const getFaceEdges(VtrIndex faceIndex) const;
    VtrIndexArray const getEdgeVertices(VtrIndex edgeIndex) const;
    VtrIndexArray const getEdgeFaces(VtrIndex edgeIndex) const;
    VtrIndexArray const getVertexFaces(VtrIndex vertIndex) const;
    VtrIndexArray const getVertexEdges(VtrIndex vertIndex) const;

    VtrLocalIndexArray const getVertexFaceLocalIndices(VtrIndex vertIndex) const;
    VtrLocalIndexArray const getVertexEdgeLocalIndices(VtrIndex vertIndex) const;

    //  Replace these with access to sharpness buffers/arrays rather than elements:
    VtrSharpness getEdgeSharpness(VtrIndex edgeIndex) const;
    VtrSharpness getVertexSharpness(VtrIndex vertIndex) const;
    SdcRule      getVertexRule(     VtrIndex vertIndex) const;

    VtrIndex findEdge(VtrIndex v0Index, VtrIndex v1Index) const;
    
    //  Should compute and cache this internally...
    int findMaxValence() const;

public:
    //  Debugging aides -- unclear what will persist...
    bool validateTopology() const;

    void print(const VtrRefinement* parentRefinement = 0) const;

public:
    //  High-level topology queries -- these are likely to be moved elsewhere, but here
    //  is the best place for them for now...

    //  Irritating that the PatchTables use "unsigned int" for indices instead of "int":
    int gatherQuadRegularInteriorPatchVertices(VtrIndex fIndex, unsigned int patchVerts[], int rotation = 0) const;
    int gatherQuadRegularBoundaryPatchVertices(VtrIndex fIndex, unsigned int patchVerts[], int boundaryEdgeInFace) const;
    int gatherQuadRegularCornerPatchVertices(  VtrIndex fIndex, unsigned int patchVerts[], int cornerVertInFace) const;

    int gatherManifoldVertexRingFromIncidentQuads(VtrIndex vIndex, int ringVerts[]) const;

protected:
    template <class MESH>
    friend class FarRefineTablesFactory;
    friend class FarRefineTablesFactoryBase;
    friend class FarRefineTables;
    friend class FarPatchTablesFactory;
    friend class VtrRefinement;

    //  Sizing methods used to construct a level to populate:
    void resizeFaces(       int numFaces);
    void resizeFaceVertices(int numFaceVertsTotal);
    void resizeFaceEdges(   int numFaceEdgesTotal);

    void resizeEdges(    int numEdges);
    void resizeEdgeVertices();  // always 2*edgeCount
    void resizeEdgeFaces(int numEdgeFacesTotal);

    void resizeVertices(   int numVertices);
    void resizeVertexFaces(int numVertexFacesTotal);
    void resizeVertexEdges(int numVertexEdgesTotal);

    //  Modifiers to populate the relations for each component:
    VtrIndexArray      getFaceVertices(VtrIndex faceIndex);
    VtrIndexArray      getFaceEdges(VtrIndex faceIndex);
    VtrIndexArray      getEdgeVertices(VtrIndex edgeIndex);
    VtrIndexArray      getEdgeFaces(VtrIndex edgeIndex);
    VtrIndexArray      getVertexFaces(           VtrIndex vertIndex);
    VtrLocalIndexArray getVertexFaceLocalIndices(VtrIndex vertIndex);
    VtrIndexArray      getVertexEdges(           VtrIndex vertIndex);
    VtrLocalIndexArray getVertexEdgeLocalIndices(VtrIndex vertIndex);

    //  Replace these with access to sharpness buffers/arrays rather than elements:
    VtrSharpness& getEdgeSharpness(VtrIndex edgeIndex);
    VtrSharpness& getVertexSharpness(VtrIndex vertIndex);

    //  Counts and offsets for all relation types:
    //      - these may be unwarranted if we let VtrRefinement access members directly...
    int getNumFaceVertices(     VtrIndex faceIndex) const { return _faceVertCountsAndOffsets[2*faceIndex]; }
    int getOffsetOfFaceVertices(VtrIndex faceIndex) const { return _faceVertCountsAndOffsets[2*faceIndex + 1]; }

    int getNumFaceEdges(     VtrIndex faceIndex) const { return getNumFaceVertices(faceIndex); }
    int getOffsetOfFaceEdges(VtrIndex faceIndex) const { return getOffsetOfFaceVertices(faceIndex); }

    int getNumEdgeVertices(     VtrIndex )          const { return 2; }
    int getOffsetOfEdgeVertices(VtrIndex edgeIndex) const { return 2 * edgeIndex; }

    int getNumEdgeFaces(     VtrIndex edgeIndex) const { return _edgeFaceCountsAndOffsets[2*edgeIndex]; }
    int getOffsetOfEdgeFaces(VtrIndex edgeIndex) const { return _edgeFaceCountsAndOffsets[2*edgeIndex + 1]; }

    int getNumVertexFaces(     VtrIndex vertIndex) const { return _vertFaceCountsAndOffsets[2*vertIndex]; }
    int getOffsetOfVertexFaces(VtrIndex vertIndex) const { return _vertFaceCountsAndOffsets[2*vertIndex + 1]; }

    int getNumVertexEdges(     VtrIndex vertIndex) const { return _vertEdgeCountsAndOffsets[2*vertIndex]; }
    int getOffsetOfVertexEdges(VtrIndex vertIndex) const { return _vertEdgeCountsAndOffsets[2*vertIndex + 1]; }


    //
    //  Note that for some relations, the size of the relations for a child component
    //  can vary radically from its parent due to the sparsity of the refinement.  So
    //  in these cases a few additional utilities are provided to help define the set
    //  of incident components.  Assuming adequate memory has been allocated, the
    //  "resize" methods here initialize the set of incident components by setting
    //  both the size and the appropriate offset, while "trim" is use to quickly lower
    //  the size from an upper bound and nothing else.
    //
    void resizeFaceVertices(VtrIndex FaceIndex, int count);

    void resizeEdgeFaces(VtrIndex edgeIndex, int count);
    void trimEdgeFaces(  VtrIndex edgeIndex, int count);

    void resizeVertexFaces(VtrIndex vertIndex, int count);
    void trimVertexFaces(  VtrIndex vertIndex, int count);

    void resizeVertexEdges(VtrIndex vertIndex, int count);
    void trimVertexEdges(  VtrIndex vertIndex, int count);

protected:
    //
    //  Plans where to have a few specific friend classes properly construct the topology,
    //  e.g. the VtrRefinement class.  There is now clearly a need to have some class
    //  construct full topology given only a simple face-vertex list.  That can be done
    //  externally (either a Factory outside Vtr or another Vtr construction helper), but
    //  until we decide where, the required implementation is defined here.
    //
    void completeTopologyFromFaceVertices();
    VtrIndex findEdge(VtrIndex v0, VtrIndex v1, VtrIndexArray const& v0Edges) const;

    //  Methods supporting the above:
    void orientIncidentComponents();
    bool orderVertexFacesAndEdges(int vIndex);
    void populateLocalIndices();

protected:
    //  Its debatable whether we should retain a Type or Options associated with
    //  a subdivision scheme here.  A Level is pure topology now.  The Refinement
    //  that create it was influenced by subdivision Type and Options, and both
    //  are now stored as members of the Refinement.
    //
    //SdcType    _schemeType;
    //SdcOptions _schemeOptions;

    //  Simple members for inventory, etc.
    int _faceCount;
    int _edgeCount;
    int _vertCount;

    //  TBD - "depth" is clearly useful in both the topological splitting and the
    //  stencil queries so could be valuable in both.  As face-vert valence becomes
    //  constant there is no need to store face-vert and face-edge counts so it has
    //  value in VtrLevel, though perhaps specified as something other than "depth"
    int   _depth;

    //
    //  Topology vectors:
    //      Note that of all of these, only data for the face-edge relation is not
    //      stored in the osd::FarTables in any form.  The FarTable vectors combine
    //      the edge-vert and edge-face relations.  The eventual goal is that this
    //      data be part of the osd::Far classes and be a superset of the FarTable
    //      vectors, i.e. no data duplication or conversion.  The fact that FarTable
    //      already stores 5 of the 6 possible relations should make the topology
    //      storage as a whole a non-issue.
    //
    //      The vert-face-child and vert-edge-child indices are also arguably not
    //      a topology relation but more one for parent/child relations.  But it is
    //      a topological relationship, and if named differently would not likely
    //      raise this.  It has been named with "child" in the name as it does play
    //      a more significant role during subdivision in mapping between parent
    //      and child components, and so has been named to reflect that more clearly.
    //

    //  Per-face:
    std::vector<VtrIndex> _faceVertCountsAndOffsets;  // 2 per face, redundant after level 0
    std::vector<VtrIndex> _faceVertIndices;           // 3 or 4 per face, variable at level 0
    std::vector<VtrIndex> _faceEdgeIndices;           // matches face-vert indices
    std::vector<FTag>     _faceTags;                  // 1 per face:  includes "hole" tag

    //  Per-edge:
    std::vector<VtrIndex> _edgeVertIndices;           // 2 per edge
    std::vector<VtrIndex> _edgeFaceCountsAndOffsets;  // 2 per edge
    std::vector<VtrIndex> _edgeFaceIndices;           // varies with faces per edge

    std::vector<VtrSharpness> _edgeSharpness;             // 1 per edge
    std::vector<ETag>         _edgeTags;                  // 1 per edge:  manifold, boundary, etc.

    //  Per-vertex:
    std::vector<VtrIndex>      _vertFaceCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      _vertFaceIndices;           // varies with valence
    std::vector<VtrLocalIndex> _vertFaceLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrIndex>      _vertEdgeCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      _vertEdgeIndices;           // varies with valence
    std::vector<VtrLocalIndex> _vertEdgeLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrSharpness>  _vertSharpness;             // 1 per vertex
    std::vector<VTag>          _vertTags;                  // 1 per vertex:  manifold, SdcRule, etc.
};

//
//  Access/modify the vertices indicent a given face:
//
inline VtrIndexArray const
VtrLevel::getFaceVertices(VtrIndex faceIndex) const
{
    return VtrIndexArray(&_faceVertIndices[_faceVertCountsAndOffsets[faceIndex*2+1]],
                          _faceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexArray
VtrLevel::getFaceVertices(VtrIndex faceIndex)
{
    return VtrIndexArray(&_faceVertIndices[_faceVertCountsAndOffsets[faceIndex*2+1]],
                          _faceVertCountsAndOffsets[faceIndex*2]);
}

inline void
VtrLevel::resizeFaceVertices(VtrIndex faceIndex, int count)
{
    assert(count < 256);

    int* countOffsetPair = &_faceVertCountsAndOffsets[faceIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (faceIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}

//
//  Access/modify the edges indicent a given face:
//
inline VtrIndexArray const
VtrLevel::getFaceEdges(VtrIndex faceIndex) const
{
    return VtrIndexArray(&_faceEdgeIndices[_faceVertCountsAndOffsets[faceIndex*2+1]],
                          _faceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexArray
VtrLevel::getFaceEdges(VtrIndex faceIndex)
{
    return VtrIndexArray(&_faceEdgeIndices[_faceVertCountsAndOffsets[faceIndex*2+1]],
                          _faceVertCountsAndOffsets[faceIndex*2]);
}

//
//  Access/modify the faces indicent a given vertex:
//
inline VtrIndexArray const
VtrLevel::getVertexFaces(VtrIndex vertIndex) const
{
    return VtrIndexArray(&_vertFaceIndices[_vertFaceCountsAndOffsets[vertIndex*2+1]],
                          _vertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexArray
VtrLevel::getVertexFaces(VtrIndex vertIndex)
{
    return VtrIndexArray(&_vertFaceIndices[_vertFaceCountsAndOffsets[vertIndex*2+1]],
                          _vertFaceCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexArray const
VtrLevel::getVertexFaceLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexArray(&_vertFaceLocalIndices[_vertFaceCountsAndOffsets[vertIndex*2+1]],
                               _vertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexArray
VtrLevel::getVertexFaceLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexArray(&_vertFaceLocalIndices[_vertFaceCountsAndOffsets[vertIndex*2+1]],
                               _vertFaceCountsAndOffsets[vertIndex*2]);
}

inline void
VtrLevel::resizeVertexFaces(VtrIndex vertIndex, int count)
{
    int* countOffsetPair = &_vertFaceCountsAndOffsets[vertIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (vertIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimVertexFaces(VtrIndex vertIndex, int count)
{
    _vertFaceCountsAndOffsets[vertIndex*2] = count;
}

//
//  Access/modify the edges indicent a given vertex:
//
inline VtrIndexArray const
VtrLevel::getVertexEdges(VtrIndex vertIndex) const
{
    return VtrIndexArray(&_vertEdgeIndices[_vertEdgeCountsAndOffsets[vertIndex*2+1]],
                          _vertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexArray
VtrLevel::getVertexEdges(VtrIndex vertIndex)
{
    return VtrIndexArray(&_vertEdgeIndices[_vertEdgeCountsAndOffsets[vertIndex*2+1]],
                          _vertEdgeCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexArray const
VtrLevel::getVertexEdgeLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexArray(&_vertEdgeLocalIndices[_vertEdgeCountsAndOffsets[vertIndex*2+1]],
                               _vertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexArray
VtrLevel::getVertexEdgeLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexArray(&_vertEdgeLocalIndices[_vertEdgeCountsAndOffsets[vertIndex*2+1]],
                               _vertEdgeCountsAndOffsets[vertIndex*2]);
}

inline void
VtrLevel::resizeVertexEdges(VtrIndex vertIndex, int count)
{
    int* countOffsetPair = &_vertEdgeCountsAndOffsets[vertIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (vertIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimVertexEdges(VtrIndex vertIndex, int count)
{
    _vertEdgeCountsAndOffsets[vertIndex*2] = count;
}

//
//  Access/modify the vertices indicent a given edge:
//
inline VtrIndexArray const
VtrLevel::getEdgeVertices(VtrIndex edgeIndex) const
{
    return VtrIndexArray(&_edgeVertIndices[edgeIndex*2], 2);
}
inline VtrIndexArray
VtrLevel::getEdgeVertices(VtrIndex edgeIndex)
{
    return VtrIndexArray(&_edgeVertIndices[edgeIndex*2], 2);
}

//
//  Access/modify the faces indicent a given edge:
//
inline VtrIndexArray const
VtrLevel::getEdgeFaces(VtrIndex edgeIndex) const
{
    return VtrIndexArray(&_edgeFaceIndices[_edgeFaceCountsAndOffsets[edgeIndex*2+1]],
                          _edgeFaceCountsAndOffsets[edgeIndex*2]);
}
inline VtrIndexArray
VtrLevel::getEdgeFaces(VtrIndex edgeIndex)
{
    return VtrIndexArray(&_edgeFaceIndices[_edgeFaceCountsAndOffsets[edgeIndex*2+1]],
                          _edgeFaceCountsAndOffsets[edgeIndex*2]);
}

inline void
VtrLevel::resizeEdgeFaces(VtrIndex edgeIndex, int count)
{
    int* countOffsetPair = &_edgeFaceCountsAndOffsets[edgeIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (edgeIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimEdgeFaces(VtrIndex edgeIndex, int count)
{
    _edgeFaceCountsAndOffsets[edgeIndex*2] = count;
}

//
//  Access/modify sharpness values:
//
inline VtrSharpness
VtrLevel::getEdgeSharpness(VtrIndex edgeIndex) const
{
    return _edgeSharpness[edgeIndex];
}
inline VtrSharpness&
VtrLevel::getEdgeSharpness(VtrIndex edgeIndex)
{
    return _edgeSharpness[edgeIndex];
}

inline VtrSharpness
VtrLevel::getVertexSharpness(VtrIndex vertIndex) const
{
    return _vertSharpness[vertIndex];
}
inline VtrSharpness&
VtrLevel::getVertexSharpness(VtrIndex vertIndex)
{
    return _vertSharpness[vertIndex];
}

inline SdcRule
VtrLevel::getVertexRule(VtrIndex vertIndex) const
{
    return (SdcRule) _vertTags[vertIndex]._rule;
}

//
//  Sizing methods to allocate space:
//
inline void
VtrLevel::resizeFaces(int faceCount)
{
    _faceCount = faceCount;
    _faceVertCountsAndOffsets.resize(2 * faceCount);

    _faceTags.resize(faceCount);
    std::memset(&_faceTags[0], 0, _faceCount * sizeof(FTag));
}
inline void
VtrLevel::resizeFaceVertices(int totalFaceVertCount)
{
    _faceVertIndices.resize(totalFaceVertCount);
}
inline void
VtrLevel::resizeFaceEdges(int totalFaceEdgeCount)
{
    _faceEdgeIndices.resize(totalFaceEdgeCount);
}

inline void
VtrLevel::resizeEdges(int edgeCount)
{
    _edgeCount = edgeCount;
    _edgeFaceCountsAndOffsets.resize(2 * edgeCount);

    _edgeSharpness.resize(edgeCount);
    _edgeTags.resize(edgeCount);
    std::memset(&_edgeTags[0], 0, _edgeCount * sizeof(ETag));
}
inline void
VtrLevel::resizeEdgeVertices()
{
    _edgeVertIndices.resize(2 * _edgeCount);
}
inline void
VtrLevel::resizeEdgeFaces(int totalEdgeFaceCount)
{
    _edgeFaceIndices.resize(totalEdgeFaceCount);
}

inline void
VtrLevel::resizeVertices(int vertCount)
{
    _vertCount = vertCount;
    _vertFaceCountsAndOffsets.resize(2 * vertCount);
    _vertEdgeCountsAndOffsets.resize(2 * vertCount);

    _vertSharpness.resize(vertCount);
    _vertTags.resize(vertCount);
    std::memset(&_vertTags[0], 0, _vertCount * sizeof(VTag));
}
inline void
VtrLevel::resizeVertexFaces(int totalVertFaceCount)
{
    _vertFaceIndices.resize(totalVertFaceCount);
    _vertFaceLocalIndices.resize(totalVertFaceCount);
}
inline void
VtrLevel::resizeVertexEdges(int totalVertEdgeCount)
{
    _vertEdgeIndices.resize(totalVertEdgeCount);
    _vertEdgeLocalIndices.resize(totalVertEdgeCount);
}

//
//  We should really be computing this either on construction (via the factory) or
//  refinement.  The base level should always provide an upper bound on the max as
//  long as the face-vertex counts are considered, so there is little point doing
//  anything for any higher level.
//
inline int
VtrLevel::findMaxValence() const
{
    int maxValence = 0;
    for (int i = 0; i < _vertCount; ++i) {
        maxValence = std::max(maxValence, _vertEdgeCountsAndOffsets[2*i]);
    }
    if (_depth == 0) {
        for (int i = 0; i < _faceCount; ++i) {
            maxValence = std::max(maxValence, _faceVertCountsAndOffsets[2*i]);
        }
    }
    return maxValence;
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* VTR_LEVEL_H */
