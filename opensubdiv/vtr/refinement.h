// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef VTR_REFINEMENT_H
#define VTR_REFINEMENT_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../vtr/types.h"
#include "../vtr/level.h"

#include <vector>

//
//  Declaration for the main refinement class (VtrRefinement) and its pre-requisites:
//
namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  VtrRefinement:
//      A refinement is a mapping between two levels -- relating the components in
//  the original (parent) level to the one refined (child).  The refinement may be
//  complete (uniform) or sparse (adaptive or otherwise selective), so not all
//  components in the parent level will spawn components in the child level.
//
class VtrRefinement
{
public:
    VtrRefinement();
    ~VtrRefinement();

    void initialize(VtrLevel& parent, VtrLevel& child);

    //  Need to replace the single bool with either a more extensive set of
    //  pararameters or settings on the instance itself...
    void refine(bool includeTopology = true, bool computeStencils = false);

    //  The following two methods may overlap in purpose...
    void resizeAndMarkParentToChildMappings();
    void sequenceAndCountChildComponents();

    void subdivideTopology();
    void subdivideVertSharpness();
    void subdivideEdgeSharpness();
    void subdivideVertMask();

public:  //  eventually protect access here...

    //  Methods for sizing the child topology vectors (note we only need four of the
    //  expected six here as face-edge shares face-vert and edge-vert is trivial):
    void initializeChildFaceVertCountsAndOffsets();
    void initializeChildEdgeFaceCountsAndOffsets();
    void initializeChildVertFaceCountsAndOffsets();
    void initializeChildVertEdgeCountsAndOffsets();

    //  Methods for populating sections of child topology relations based on
    //  their origin in the parent -- 12 in all:
    void populateFaceVertsFromParentFaces();
    void populateFaceEdgesFromParentFaces();
    void populateEdgeVertsFromParentFaces();
    void populateEdgeVertsFromParentEdges();
    void populateEdgeFacesFromParentFaces();
    void populateEdgeFacesFromParentEdges();
    void populateVertFacesFromParentFaces();
    void populateVertFacesFromParentEdges();
    void populateVertFacesFromParentVerts();
    void populateVertEdgesFromParentFaces();
    void populateVertEdgesFromParentEdges();
    void populateVertEdgesFromParentVerts();

    //  Methods for populating a child relation for a single parent component:
    //
    //  Note to self:  these methods and those above (for all components) were
    //  created in the midst of threading experimentation that was never finished.
    //  The intent was for each of the above 12 methods to be split out into one
    //  that could be invoked for a single component to allow different kinds of
    //  iteration -- with 12 corresponding methods for all components that would
    //  manage all and iterate/distribute accordingly.
    //
    //  So there really should be 12 methods below.  It turns out though that
    //  breaking them up this way may not be ideal for efficiency.  For some a
    //  common set of information is retrieved for the parent component which
    //  all derived child components can use.
    //
    //  When sparse refinement is driven more by iteration through the child
    //  components that are generated (rather than iterating through all parent
    //  components and skipping those children not generated) we should revist
    //  and refactor this.
    //
    void populateChildFaceVertsFromParentFace(VtrIndex parentFace);
    void populateChildFaceEdgesFromParentFace(VtrIndex parentFace);
    void populateChildEdgeVertsFromParentFace(VtrIndex parentFace);
    void populateChildEdgeVertsFromParentEdge(VtrIndex parentEdge);
    void populateChildEdgeFacesFromParentFace(VtrIndex parentFace);
    void populateChildEdgeFacesFromParentEdge(VtrIndex parentEdge);

    void classifyChildVertsFromParentFaces();
    void classifyChildVertsFromParentEdges();
    void classifyChildVertsFromParentVerts();

    //  Experimentation -- computing and storing of stencil weights needs far
    //  more consideration than the naive storage scheme used here...
    void computeStencilWeights();

    VtrIndexAccessor accessFaceChildFaces(VtrIndex parentFace) const;
    VtrIndexAccessor accessFaceChildEdges(VtrIndex parentFace) const;
    VtrIndexAccessor accessEdgeChildEdges(VtrIndex parentEdge) const;

public:  //  eventually protect member access...
    VtrLevel* mParent;
    VtrLevel* mChild;

    SdcType    _schemeType;
    SdcOptions _schemeOptions;

    //
    //  Hierarchy vectors:
    //      - unclear yet if this should be bi-directional
    //
    //  References to components in a child level:
    //      Note that faces and edges will not spawn all children in cases of "selective"
    //  refinement (e.g. adaptive, including holes or otherwise select), so some children
    //  may be marked as invalid/unused.  These vectors are sized to match the topology
    //  relations and so the same counts and offsets apply.
    //
    //  The sizes of these vectors will dictate/correspond to the sizes of vectors in the
    //  child level.
    //
    //  Note that in most cases, but not all -- we can use the same offsets for the
    //  parent components children as are used for its incident components, e.g. we can
    //  use the face-verts offsets to find the child faces corresponding to each face-
    //  vert.  But in the case of Loop subdivision, the norm will be for each triagular
    //  face to have four child faces, so these child vectors may need their own set of
    //  offsets in some cases (in the Loop case, provided faces other than triangles are
    //  not supported -- which is currently the case -- integer multiples can be used
    //  insteadof offsets).
    //
    VtrIndexVector mFaceChildFaceIndices;  // *cannot* always use face-vert counts/offsets
    VtrIndexVector mFaceChildEdgeIndices;  // can use face-vert counts/offsets
    VtrIndexVector mFaceChildVertIndex;

    VtrIndexVector mEdgeChildEdgeIndices;  // trivial/corresponding pair for each
    VtrIndexVector mEdgeChildVertIndex;

    VtrIndexVector mVertChildVertIndex;

    //
    //  Members to indicate the origin of child components.  These are determined as we
    //  size the vectors of child components and its useful to retain them for later use:
    //
    int mChildFaceFromFaceCount;  // arguably redundant (all faces originate from faces)
    int mChildEdgeFromFaceCount;
    int mChildEdgeFromEdgeCount;
    int mChildVertFromFaceCount;
    int mChildVertFromEdgeCount;
    int mChildVertFromVertCount;

    //  References to components in a parent level:
    //      Referring to parent components already specified by the child of a parent may
    //  be redundant/unnecessary, but the parent of vertices is often desired (why?).
    //  Note that the parent of a vertex may be a vertex, edge or face.  Given the way we
    //  arrange these, we should be able to tell from the index of a vertex at this level
    //  and the knowledge of children in its parent level, whether the vertex comes from
    //  a face, edge or vertex (child vertices are defined first from faces, then edges,
    //  then vertices).  We are considering maintaining the counts of these when children
    //  are generated for use in the parent level.  Otherwise a separate tag/indicator
    //  will be required.
    /*
    VtrIndexVector mFaceParentIndex;
    VtrIndexVector mEdgeParentIndex;
    VtrIndexVector mVertParentIndex;
    */

    //
    //  TEMPORARY FOR TESTING...
    //
    //  Stencil for the child vertices stored relative to parent topology, i.e. weights
    //  for a child face-vertex are stored relative to the parent face -- a weight for
    //  each of the parent face's vertices.
    //
    //  Currently the full complement of weights is used and expected to be applied, e.g.
    //  for a crease in the interior, there may be no face-vert weights in the stencil
    //  and so no need to apply them, but we require all and so set these to zero for now.
    //  We will need some kind of stencil type associated with each child vertex if we
    //  are to avoid doing so in order to detect the difference.
    //
    //  Note this is potentially extremely wasteful in terms of memory when the set of
    //  refined components in the child is small relative to the parent.  All possible
    //  stencil weights (i.e. for uniform refinement) will be allocated here if the
    //  corresonding counts/offset of the parent are to be used.
    //
    std::vector<float> mFaceVertWeights;  // matches parent face vert counts and offsets
    std::vector<float> mEdgeVertWeights;  // trivially 2 per parent edge
    std::vector<float> mEdgeFaceWeights;  // matches parent edge face counts and offsets
    std::vector<float> mVertVertWeights;  // trivially 1 per parent vert
    std::vector<float> mVertEdgeWeights;  // matches parent vert edge counts and offsets
    std::vector<float> mVertFaceWeights;  // matches parent vert face counts and offsets

    //  References to components in the base level (top-most ancestor) may be useful
    //  to copy non-interpolatible properties to all descendant components.
};

inline VtrIndexAccessor
VtrRefinement::accessFaceChildFaces(VtrIndex parentFace) const
{
    //
    //  Note this will need to vary based on the topological split applied...
    //
    const int* countOffsetPair = &mParent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexAccessor(&mFaceChildFaceIndices[countOffsetPair[1]], countOffsetPair[0]);
}
inline VtrIndexAccessor
VtrRefinement::accessFaceChildEdges(VtrIndex parentFace) const
{
    //
    //  Note this *may* need to vary based on the topological split applied...
    //
    const int* countOffsetPair = &mParent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexAccessor(&mFaceChildEdgeIndices[countOffsetPair[1]], countOffsetPair[0]);
}

inline VtrIndexAccessor
VtrRefinement::accessEdgeChildEdges(VtrIndex parentEdge) const
{
    return VtrIndexAccessor(&mEdgeChildEdgeIndices[parentEdge*2], 2);
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* VTR_REFINEMENT_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
