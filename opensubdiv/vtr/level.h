// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef VTR_LEVEL_H
#define VTR_LEVEL_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/crease.h"
#include "../vtr/types.h"

#include <vector>


//  Forward declaration for friend class temporarily used for testing...
class FarDataTables;


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
//  Notes/limitations/stuff to do:
//      - verify public/protected/friend accessibility
//      - finish replacing local "m" member prefix with "_" (easier after above)
//      - keep in mind desired similarity with FarMesh tables for ease of transfer
//        (contradicts previous point to some degree)
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
//

class VtrLevel
{
public:
    VtrLevel();
    ~VtrLevel();

    //  Simple accessors:
    int depth() const { return _depth; }

    int vertCount() const { return _vertCount; }
    int faceCount() const { return _faceCount; }
    int edgeCount() const { return _edgeCount; }

    //  More global sizes may prove useful...
    int faceVertCount() const { return mFaceVertIndices.size(); }
    int faceEdgeCount() const { return mFaceEdgeIndices.size(); }
    int edgeVertCount() const { return mEdgeVertIndices.size(); }
    int edgeFaceCount() const { return mEdgeFaceIndices.size(); }
    int vertFaceCount() const { return mVertFaceIndices.size(); }
    int vertEdgeCount() const { return mVertEdgeIndices.size(); }

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
    VtrIndexAccessor      accessFaceVerts(VtrIndex faceIndex) const;
    VtrIndexAccessor      accessFaceEdges(VtrIndex faceIndex) const;
    VtrIndexAccessor      accessEdgeVerts(VtrIndex edgeIndex) const;
    VtrIndexAccessor      accessEdgeFaces(VtrIndex edgeIndex) const;
    VtrIndexAccessor      accessVertFaces(           VtrIndex vertIndex) const;
    VtrLocalIndexAccessor accessVertFaceLocalIndices(VtrIndex vertIndex) const;
    VtrIndexAccessor      accessVertEdges(           VtrIndex vertIndex) const;
    VtrLocalIndexAccessor accessVertEdgeLocalIndices(VtrIndex vertIndex) const;

    VtrSharpness edgeSharpness(VtrIndex edgeIndex) const;
    VtrSharpness vertSharpness(VtrIndex vertIndex) const;
    SdcRule      vertRule(     VtrIndex vertIndex) const;

protected:
    //  Note this is an external class being used for testing...
    friend class ::FarDataTables;

    template <class MESH>
    friend class FarRefineTablesFactory;
    friend class FarRefineTablesFactoryBase;
    friend class FarRefineTables;
    friend class VtrRefinement;

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
    VtrIndexModifier      modifyFaceVerts(VtrIndex faceIndex);
    VtrIndexModifier      modifyFaceEdges(VtrIndex faceIndex);
    VtrIndexModifier      modifyEdgeVerts(VtrIndex edgeIndex);
    VtrIndexModifier      modifyEdgeFaces(VtrIndex edgeIndex);
    VtrIndexModifier      modifyVertFaces(           VtrIndex vertIndex);
    VtrLocalIndexModifier modifyVertFaceLocalIndices(VtrIndex vertIndex);
    VtrIndexModifier      modifyVertEdges(           VtrIndex vertIndex);
    VtrLocalIndexModifier modifyVertEdgeLocalIndices(VtrIndex vertIndex);

    VtrSharpness& edgeSharpness(VtrIndex edgeIndex);
    VtrSharpness& vertSharpness(VtrIndex vertIndex);
    SdcRule&      vertRule(     VtrIndex vertIndex);

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
    //  Be sure this flag is set to indicate the presence of full valid topology
    //  (it may eventually become a mask to indicate which relations are present
    //  if refinement is allowed to generate a subset):
    bool _hasTopology;

    //  Per-face:
    std::vector<VtrIndex> mFaceVertCountsAndOffsets;  // 2 per face, redundant after level 0
    std::vector<VtrIndex> mFaceVertIndices;           // 3 or 4 per face, variable at level 0
    std::vector<VtrIndex> mFaceEdgeIndices;           // matches face-vert indices

    //  Per-edge:
    std::vector<VtrIndex> mEdgeVertIndices;           // 2 per edge
    std::vector<VtrIndex> mEdgeFaceCountsAndOffsets;  // 2 per edge
    std::vector<VtrIndex> mEdgeFaceIndices;           // varies with faces per edge

    std::vector<VtrSharpness> mEdgeSharpness;             // 1 per edge

    //  Per-vertex:
    std::vector<VtrIndex>      mVertFaceCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      mVertFaceIndices;           // varies with valence
    std::vector<VtrLocalIndex> mVertFaceLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrIndex>      mVertEdgeCountsAndOffsets;  // 2 per vertex
    std::vector<VtrIndex>      mVertEdgeIndices;           // varies with valence
    std::vector<VtrLocalIndex> mVertEdgeLocalIndices;      // varies with valence, 8-bit for now

    std::vector<VtrSharpness>  mVertSharpness;             // 1 per vertex

    std::vector<SdcRule>       mVertRule;  // classification of vertex based on topology and sharpness

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

protected:
    //
    //  Debugging aides -- unclear what will remain...
    //
    void print(const VtrRefinement* parentRefinement = 0) const;
    bool validateTopology() const;
};

//
//  Access/modify the vertices indicent a given face:
//
inline VtrIndexAccessor
VtrLevel::accessFaceVerts(VtrIndex faceIndex) const
{
    return VtrIndexAccessor(&mFaceVertIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                             mFaceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexModifier
VtrLevel::modifyFaceVerts(VtrIndex faceIndex)
{
    return VtrIndexModifier(&mFaceVertIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
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
inline VtrIndexAccessor
VtrLevel::accessFaceEdges(VtrIndex faceIndex) const
{
    return VtrIndexAccessor(&mFaceEdgeIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                             mFaceVertCountsAndOffsets[faceIndex*2]);
}
inline VtrIndexModifier
VtrLevel::modifyFaceEdges(VtrIndex faceIndex)
{
    return VtrIndexModifier(&mFaceEdgeIndices[mFaceVertCountsAndOffsets[faceIndex*2+1]],
                             mFaceVertCountsAndOffsets[faceIndex*2]);
}

//
//  Access/modify the faces indicent a given vertex:
//
inline VtrIndexAccessor
VtrLevel::accessVertFaces(VtrIndex vertIndex) const
{
    return VtrIndexAccessor(&mVertFaceIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                             mVertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexModifier
VtrLevel::modifyVertFaces(VtrIndex vertIndex)
{
    return VtrIndexModifier(&mVertFaceIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                             mVertFaceCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexAccessor
VtrLevel::accessVertFaceLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexAccessor(&mVertFaceLocalIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
                             mVertFaceCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexModifier
VtrLevel::modifyVertFaceLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexModifier(&mVertFaceLocalIndices[mVertFaceCountsAndOffsets[vertIndex*2+1]],
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
inline VtrIndexAccessor
VtrLevel::accessVertEdges(VtrIndex vertIndex) const
{
    return VtrIndexAccessor(&mVertEdgeIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                             mVertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrIndexModifier
VtrLevel::modifyVertEdges(VtrIndex vertIndex)
{
    return VtrIndexModifier(&mVertEdgeIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                             mVertEdgeCountsAndOffsets[vertIndex*2]);
}

inline VtrLocalIndexAccessor
VtrLevel::accessVertEdgeLocalIndices(VtrIndex vertIndex) const
{
    return VtrLocalIndexAccessor(&mVertEdgeLocalIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
                                 mVertEdgeCountsAndOffsets[vertIndex*2]);
}
inline VtrLocalIndexModifier
VtrLevel::modifyVertEdgeLocalIndices(VtrIndex vertIndex)
{
    return VtrLocalIndexModifier(&mVertEdgeLocalIndices[mVertEdgeCountsAndOffsets[vertIndex*2+1]],
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
inline VtrIndexAccessor
VtrLevel::accessEdgeVerts(VtrIndex edgeIndex) const
{
    return VtrIndexAccessor(&mEdgeVertIndices[edgeIndex*2], 2);
}
inline VtrIndexModifier
VtrLevel::modifyEdgeVerts(VtrIndex edgeIndex)
{
    return VtrIndexModifier(&mEdgeVertIndices[edgeIndex*2], 2);
}

//
//  Access/modify the faces indicent a given edge:
//
inline VtrIndexAccessor
VtrLevel::accessEdgeFaces(VtrIndex edgeIndex) const
{
    return VtrIndexAccessor(&mEdgeFaceIndices[mEdgeFaceCountsAndOffsets[edgeIndex*2+1]],
                             mEdgeFaceCountsAndOffsets[edgeIndex*2]);
}
inline VtrIndexModifier
VtrLevel::modifyEdgeFaces(VtrIndex edgeIndex)
{
    return VtrIndexModifier(&mEdgeFaceIndices[mEdgeFaceCountsAndOffsets[edgeIndex*2+1]],
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
    return mVertRule[vertIndex];
}
inline SdcRule&
VtrLevel::vertRule(VtrIndex vertIndex)
{
    return mVertRule[vertIndex];
}

//
//  Sizing methods to allocate space:
//
inline void
VtrLevel::resizeFaces(int faceCount)
{
    _faceCount = faceCount;
    mFaceVertCountsAndOffsets.resize(2 * faceCount);
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
    mVertRule.resize(vertCount);
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

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
