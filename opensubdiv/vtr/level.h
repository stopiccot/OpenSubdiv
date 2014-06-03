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


//  Forward declaration for friend class temporarily used for testing...
class FarDataTables;
class VtrBuilder;


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//  Forward declarations for friends:
template <class MESH> class FarRefineTablesFactory;
class FarRefineTablesFactoryBase;
class FarRefineTables;
class VtrRefinement;
class VtrSelector;

//
//  VtrLevel:
//      A refinement level includes a vectorized representation of the topology
//  for a particular subdivision level.  The topology is "complete" in that any
//  level can be used as the base level of another subdivision hierarchy and can
//  be considered a complete mesh independent of its ancestors.  It currently
//  does contain a "depth" member -- as some inferences can then be made about
//  the topology (i.e. all quads or all tris if not level 0) but that is still
//  under consideration.
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
//          - replacing "m" prefix with "_" on member variables
//          - use of Vertex vs Vert in non-local methods
//          - removal of "access" and "modify" prefixes on VtrArray methods
//      - review public/protected/friend accessibility
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
    int depth() const { return _depth; }

    int vertCount() const { return _vertCount; }
    int faceCount() const { return _faceCount; }
    int edgeCount() const { return _edgeCount; }

    //  More global sizes may prove useful...
    int faceVertCount() const { return (int) mFaceVertIndices.size(); }
    int faceEdgeCount() const { return (int) mFaceEdgeIndices.size(); }
    int edgeVertCount() const { return (int) mEdgeVertIndices.size(); }
    int edgeFaceCount() const { return (int) mEdgeFaceIndices.size(); }
    int vertFaceCount() const { return (int) mVertFaceIndices.size(); }
    int vertEdgeCount() const { return (int) mVertEdgeIndices.size(); }

    //  Methods to access the relation tables/indices -- note that for some relations
    //  we store an additional "local index", e.g. for the case of vert-faces if one
    //  of the faces F[i] is incident a vertex V, then L[i] is the "local index" in
    //  F[i] of vertex V.  Once have only quads (or tris), this local index need only
    //  occupy two bits and could conceivably be packed into the same integer as the
    //  face index, but for now, given the need to support faces of potentially high
    //  valence we'll us an 8- or 16-bit integer.
    //
    //  Methods to access and modify the six topological relations:
    //
    VtrIndexArray const accessFaceVerts(VtrIndex faceIndex) const;
    VtrIndexArray const accessFaceEdges(VtrIndex faceIndex) const;
    VtrIndexArray const accessEdgeVerts(VtrIndex edgeIndex) const;
    VtrIndexArray const accessEdgeFaces(VtrIndex edgeIndex) const;
    VtrIndexArray const accessVertFaces(VtrIndex vertIndex) const;
    VtrIndexArray const accessVertEdges(VtrIndex vertIndex) const;

    VtrLocalIndexArray const accessVertFaceLocalIndices(VtrIndex vertIndex) const;
    VtrLocalIndexArray const accessVertEdgeLocalIndices(VtrIndex vertIndex) const;

    VtrSharpness edgeSharpness(VtrIndex edgeIndex) const;
    VtrSharpness vertSharpness(VtrIndex vertIndex) const;
    SdcRule      vertRule(     VtrIndex vertIndex) const;

protected:
    //  Note this is an external class being used for testing...
    friend class ::FarDataTables;
    friend class ::VtrBuilder;

    template <class MESH>
    friend class FarRefineTablesFactory;
    friend class FarRefineTablesFactoryBase;
    friend class FarRefineTables;
    friend class VtrRefinement;
    friend class VtrSelector;

    //
    //  Sizing methods used to construct a level to populate:
    //
    void resizeFaces(    int faceCount);
    void resizeFaceVerts(int totalFaceVertCount);
    void resizeFaceEdges(int totalFaceEdgeCount);

    void resizeEdges(    int edgeCount);
    void resizeEdgeVerts();  // always 2*edgeCount
    void resizeEdgeFaces(int totalEdgeFaceCount);

    void resizeVerts(    int vertCount);
    void resizeVertFaces(int totalVertFaceCount);
    void resizeVertEdges(int totalVertEdgeCount);

    //
    //  Modifiers to populate the relations for each component:
    //
    VtrIndexArray      modifyFaceVerts(VtrIndex faceIndex);
    VtrIndexArray      modifyFaceEdges(VtrIndex faceIndex);
    VtrIndexArray      modifyEdgeVerts(VtrIndex edgeIndex);
    VtrIndexArray      modifyEdgeFaces(VtrIndex edgeIndex);
    VtrIndexArray      modifyVertFaces(           VtrIndex vertIndex);
    VtrLocalIndexArray modifyVertFaceLocalIndices(VtrIndex vertIndex);
    VtrIndexArray      modifyVertEdges(           VtrIndex vertIndex);
    VtrLocalIndexArray modifyVertEdgeLocalIndices(VtrIndex vertIndex);

    VtrSharpness& edgeSharpness(VtrIndex edgeIndex);
    VtrSharpness& vertSharpness(VtrIndex vertIndex);

    //
    //  Counts and offsets for all relation types:
    //
    int faceVertCount( VtrIndex faceIndex) const { return mFaceVertCountsAndOffsets[2*faceIndex]; }
    int faceVertOffset(VtrIndex faceIndex) const { return mFaceVertCountsAndOffsets[2*faceIndex + 1]; }

    int faceEdgeCount( VtrIndex faceIndex) const { return faceVertCount(faceIndex); }
    int faceEdgeOffset(VtrIndex faceIndex) const { return faceVertOffset(faceIndex); }

    int edgeVertCount( VtrIndex )          const { return 2; }
    int edgeVertOffset(VtrIndex edgeIndex) const { return 2 * edgeIndex; }

    int edgeFaceCount( VtrIndex edgeIndex) const { return mEdgeFaceCountsAndOffsets[2*edgeIndex]; }
    int edgeFaceOffset(VtrIndex edgeIndex) const { return mEdgeFaceCountsAndOffsets[2*edgeIndex + 1]; }

    int vertFaceCount( VtrIndex vertIndex) const { return mVertFaceCountsAndOffsets[2*vertIndex]; }
    int vertFaceOffset(VtrIndex vertIndex) const { return mVertFaceCountsAndOffsets[2*vertIndex + 1]; }

    int vertEdgeCount( VtrIndex vertIndex) const { return mVertEdgeCountsAndOffsets[2*vertIndex]; }
    int vertEdgeOffset(VtrIndex vertIndex) const { return mVertEdgeCountsAndOffsets[2*vertIndex + 1]; }


    //
    //  Note that for some relations, the size of the relations for a child component
    //  can vary radically from its parent due to the sparsity of the refinement.  So
    //  in these cases a few additional utilities are provided to help define the set
    //  of incident components.  Assuming adequate memory has been allocated, the
    //  "resize" methods here initialize the set of incident components by setting
    //  both the size and the appropriate offset, while "trim" is use to quickly lower
    //  the size from an upper bound and nothing else.
    //
    void resizeFaceVerts(VtrIndex FaceIndex, int count);

    void resizeEdgeFaces(VtrIndex edgeIndex, int count);
    void trimEdgeFaces(  VtrIndex edgeIndex, int count);

    void resizeVertFaces(VtrIndex vertIndex, int count);
    void trimVertFaces(  VtrIndex vertIndex, int count);

    void resizeVertEdges(VtrIndex vertIndex, int count);
    void trimVertEdges(  VtrIndex vertIndex, int count);

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
    std::vector<VtrIndex> mFaceVertCountsAndOffsets;  // 2 per face, redundant after level 0
    std::vector<VtrIndex> mFaceVertIndices;           // 3 or 4 per face, variable at level 0
    std::vector<VtrIndex> mFaceEdgeIndices;           // matches face-vert indices
    std::vector<FTag>     mFaceTags;                  // 1 per face:  includes "hole" tag

    //  Per-edge:
    std::vector<VtrIndex> mEdgeVertIndices;           // 2 per edge
    std::vector<VtrIndex> mEdgeFaceCountsAndOffsets;  // 2 per edge
    std::vector<VtrIndex> mEdgeFaceIndices;           // varies with faces per edge

    std::vector<VtrSharpness> mEdgeSharpness;             // 1 per edge
    std::vector<ETag>         mEdgeTags;                  // 1 per edge:  manifold, boundary, etc.

    //  Per-vertex:
    std::vector<VtrIndex>      mVertFaceCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      mVertFaceIndices;           // varies with valence
    std::vector<VtrLocalIndex> mVertFaceLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrIndex>      mVertEdgeCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      mVertEdgeIndices;           // varies with valence
    std::vector<VtrLocalIndex> mVertEdgeLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrSharpness>  mVertSharpness;             // 1 per vertex
    std::vector<VTag>          mVertTags;                  // 1 per vertex:  manifold, SdcRule, etc.

    //  int mRuleCounts[RULE_MAX];
    //  Note that for the above classification (Type), it is useful later to group those
    //  vertices sharing the same Type.  This results in an expensive sort() operation
    //  in the construction of the FarMesh tables.  We can help here by at least counting
    //  the number of vertices of each Type as we assign it.  We should then be able to
    //  size a vector, partition it according it to the counts for all Types, and assign
    //  the vertices by iterating through all of the types -- thereby "sorting" them.
    //
    //  (There's actually a bit more to this...  The vertices are not grouped/sorted just
    //  by the value of the Type itself, but by their "ranking" -- which is a measure of
    //  the Type at both the previous and next level.  (For more details, see the function
    //  used for sorting -- FarSubdivisionTablesFactory<T,U>::compareVertices()).  So it
    //  may be useful to compute/store this "ranking" as the Type is determined for a
    //  vertex at the next level, and store counts associated with these instead.)

public:
    //
    //  Debugging aides -- unclear what will remain...
    //
    void print(const VtrRefinement* parentRefinement = 0) const;
    bool validateTopology() const;
};

//
//  Access/modify the vertices indicent a given face:
//
inline VtrIndexArray const
VtrLevel::accessFaceVerts(VtrIndex faceIndex) const
{
    return VtrIndexArray(&mFaceVertIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                          mFaceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexArray
VtrLevel::modifyFaceVerts(VtrIndex faceIndex)
{
    return VtrIndexArray(&mFaceVertIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                          mFaceVertCountsAndOffsets[faceIndex*2]);
}

inline void
VtrLevel::resizeFaceVerts(VtrIndex faceIndex, int count)
{
    int* countOffsetPair = &mFaceVertCountsAndOffsets[faceIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (faceIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}

//
//  Access/modify the edges indicent a given face:
//
inline VtrIndexArray const
VtrLevel::accessFaceEdges(VtrIndex faceIndex) const
{
    return VtrIndexArray(&mFaceEdgeIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                          mFaceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexArray
VtrLevel::modifyFaceEdges(VtrIndex faceIndex)
{
    return VtrIndexArray(&mFaceEdgeIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                          mFaceVertCountsAndOffsets[faceIndex*2]);
}

//
//  Access/modify the faces indicent a given vertex:
//
inline VtrIndexArray const
VtrLevel::accessVertFaces(VtrIndex vertIndex) const
{
    return VtrIndexArray(&mVertFaceIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                          mVertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexArray
VtrLevel::modifyVertFaces(VtrIndex vertIndex)
{
    return VtrIndexArray(&mVertFaceIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                          mVertFaceCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexArray const
VtrLevel::accessVertFaceLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexArray(&mVertFaceLocalIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                               mVertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexArray
VtrLevel::modifyVertFaceLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexArray(&mVertFaceLocalIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                               mVertFaceCountsAndOffsets[vertIndex*2]);
}

inline void
VtrLevel::resizeVertFaces(VtrIndex vertIndex, int count)
{
    int* countOffsetPair = &mVertFaceCountsAndOffsets[vertIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (vertIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimVertFaces(VtrIndex vertIndex, int count)
{
    mVertFaceCountsAndOffsets[vertIndex*2] = count;
}

//
//  Access/modify the edges indicent a given vertex:
//
inline VtrIndexArray const
VtrLevel::accessVertEdges(VtrIndex vertIndex) const
{
    return VtrIndexArray(&mVertEdgeIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                          mVertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexArray
VtrLevel::modifyVertEdges(VtrIndex vertIndex)
{
    return VtrIndexArray(&mVertEdgeIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                          mVertEdgeCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexArray const
VtrLevel::accessVertEdgeLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexArray(&mVertEdgeLocalIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                               mVertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexArray
VtrLevel::modifyVertEdgeLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexArray(&mVertEdgeLocalIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                               mVertEdgeCountsAndOffsets[vertIndex*2]);
}

inline void
VtrLevel::resizeVertEdges(VtrIndex vertIndex, int count)
{
    int* countOffsetPair = &mVertEdgeCountsAndOffsets[vertIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (vertIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimVertEdges(VtrIndex vertIndex, int count)
{
    mVertEdgeCountsAndOffsets[vertIndex*2] = count;
}

//
//  Access/modify the vertices indicent a given edge:
//
inline VtrIndexArray const
VtrLevel::accessEdgeVerts(VtrIndex edgeIndex) const
{
    return VtrIndexArray(&mEdgeVertIndices[edgeIndex*2], 2);
}
inline VtrIndexArray
VtrLevel::modifyEdgeVerts(VtrIndex edgeIndex)
{
    return VtrIndexArray(&mEdgeVertIndices[edgeIndex*2], 2);
}

//
//  Access/modify the faces indicent a given edge:
//
inline VtrIndexArray const
VtrLevel::accessEdgeFaces(VtrIndex edgeIndex) const
{
    return VtrIndexArray(&mEdgeFaceIndices[mEdgeFaceCountsAndOffsets[edgeIndex*2+1]],
                          mEdgeFaceCountsAndOffsets[edgeIndex*2]);
}
inline VtrIndexArray
VtrLevel::modifyEdgeFaces(VtrIndex edgeIndex)
{
    return VtrIndexArray(&mEdgeFaceIndices[mEdgeFaceCountsAndOffsets[edgeIndex*2+1]],
                          mEdgeFaceCountsAndOffsets[edgeIndex*2]);
}

inline void
VtrLevel::resizeEdgeFaces(VtrIndex edgeIndex, int count)
{
    int* countOffsetPair = &mEdgeFaceCountsAndOffsets[edgeIndex*2];

    countOffsetPair[0] = count;
    countOffsetPair[1] = (edgeIndex == 0) ? 0 : (countOffsetPair[-2] + countOffsetPair[-1]);
}
inline void
VtrLevel::trimEdgeFaces(VtrIndex edgeIndex, int count)
{
    mEdgeFaceCountsAndOffsets[edgeIndex*2] = count;
}

//
//  Access/modify sharpness values:
//
inline VtrSharpness
VtrLevel::edgeSharpness(VtrIndex edgeIndex) const
{
    return mEdgeSharpness[edgeIndex];
}
inline VtrSharpness&
VtrLevel::edgeSharpness(VtrIndex edgeIndex)
{
    return mEdgeSharpness[edgeIndex];
}

inline VtrSharpness
VtrLevel::vertSharpness(VtrIndex vertIndex) const
{
    return mVertSharpness[vertIndex];
}
inline VtrSharpness&
VtrLevel::vertSharpness(VtrIndex vertIndex)
{
    return mVertSharpness[vertIndex];
}

inline SdcRule
VtrLevel::vertRule(VtrIndex vertIndex) const
{
    return (SdcRule) mVertTags[vertIndex]._rule;
}

//
//  Sizing methods to allocate space:
//
inline void
VtrLevel::resizeFaces(int faceCount)
{
    _faceCount = faceCount;
    mFaceVertCountsAndOffsets.resize(2 * faceCount);

    mFaceTags.resize(faceCount);
}
inline void
VtrLevel::resizeFaceVerts(int totalFaceVertCount)
{
    mFaceVertIndices.resize(totalFaceVertCount);
}
inline void
VtrLevel::resizeFaceEdges(int totalFaceEdgeCount)
{
    mFaceEdgeIndices.resize(totalFaceEdgeCount);
}

inline void
VtrLevel::resizeEdges(int edgeCount)
{
    _edgeCount = edgeCount;
    mEdgeFaceCountsAndOffsets.resize(2 * edgeCount);

    mEdgeSharpness.resize(edgeCount);
    mEdgeTags.resize(edgeCount);
}
inline void
VtrLevel::resizeEdgeVerts()
{
    mEdgeVertIndices.resize(2 * _edgeCount);
}
inline void
VtrLevel::resizeEdgeFaces(int totalEdgeFaceCount)
{
    mEdgeFaceIndices.resize(totalEdgeFaceCount);
}

inline void
VtrLevel::resizeVerts(int vertCount)
{
    _vertCount = vertCount;
    mVertFaceCountsAndOffsets.resize(2 * vertCount);
    mVertEdgeCountsAndOffsets.resize(2 * vertCount);

    mVertSharpness.resize(vertCount);
    mVertTags.resize(vertCount);
}
inline void
VtrLevel::resizeVertFaces(int totalVertFaceCount)
{
    mVertFaceIndices.resize(totalVertFaceCount);
    mVertFaceLocalIndices.resize(totalVertFaceCount);
}
inline void
VtrLevel::resizeVertEdges(int totalVertEdgeCount)
{
    mVertEdgeIndices.resize(totalVertEdgeCount);
    mVertEdgeLocalIndices.resize(totalVertEdgeCount);
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* VTR_LEVEL_H */
