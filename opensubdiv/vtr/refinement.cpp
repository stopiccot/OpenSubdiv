// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/crease.h"
#include "../sdc/scheme.h"
#include "../sdc/stencil.h"
#include "../sdc/faceNeighborhood.h"
#include "../sdc/edgeNeighborhood.h"
#include "../sdc/vertexNeighborhood.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"

#include <cassert>


//
//  NOTES on short-cuts and assumptions...
//      This was developed to prototype some ideas, and some issues were simplified
//  (or just plain ignored) in order to validate the more general cases first.  Use
//  the following list of keywords to search for notes in comments where these issues
//  are discussed:
//
//  CORRECTNESS:
//      - shortcuts taken when progress was more important than correctness
//
//  ORIENTATION:
//      - OSD/HBr orient subdivided faces in a specific way, on which hierarchical
//        edits strongly depend
//      - regular faces (quads) are split so that the vertex in the middle of the
//        faces is the i'th vertex in the i'th child face -- this preserves the
//        rectangular parameterization of the new collection of faces.
//      - extra-ordinary faces (including triangels) are split so that the vertex
//        in the middle of the faces is the 0'th vertex in all child faces.
//
//  N-GONS:
//      - OSD/HBr support N-sided faces at level 0, which will ultimately need support
//
namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Simple constructor and destructor -- consider inline if they remain simple...
//
VtrRefinement::VtrRefinement() :
    mParent(0),
    mChild(0),
    _schemeType(CATMARK),
    _schemeOptions(),
    mChildFaceFromFaceCount(0),
    mChildEdgeFromFaceCount(0),
    mChildEdgeFromEdgeCount(0),
    mChildVertFromFaceCount(0),
    mChildVertFromEdgeCount(0),
    mChildVertFromVertCount(0)
{
}

VtrRefinement::~VtrRefinement()
{
}

void
VtrRefinement::initialize(VtrLevel& parent, VtrLevel& child)
{
    //  Make sure we are getting a fresh child...
    assert((child.depth() == 0) && (child.vertCount() == 0));

    //
    //  Do we want anything more here for "initialization", e.g. the subd scheme,
    //  options, etc. -- or will those be specified/assigned on refinement?
    //
    mParent = &parent;
    mChild  = &child;

    child._depth = 1 + parent.depth();
}


//
//  Methods for preparing for refinement:
//
void
VtrRefinement::resizeAndMarkParentToChildMappings()
{
    //  What is the history behind this assertion?
    assert(mParent->mVertRule.size() == mParent->vertCount());

    //
    //  Initialize the vectors of indices mapping parent components to those child
    //  components that will originate from each.  Initialize all to be INVALID and
    //  then rely on marking to assign some other value.  Those detected as *not*
    //  INVALID will then be assigned a sequential index:
    //
    //  Eventually will mark all invalid and select, but for now init all as
    //  valid, which should probably be a special case anyway for uniform
    //  subdivision (one that could be optimized to mark and sequence the
    //  children in one pass instead of two):
    //
    //  REMEMBER -- there are some assumptions here about the type of topological
    //  split being performed -- a quad-split in this case.  The number of child
    //  faces for a face will be four in the case of a tri-split which differs
    //  from the three face-verts used here.
    //
    //Index initValue = VTR_INDEX_INVALID;
    VtrIndex initValue = 0;
    
    mFaceChildFaceIndices.resize(mParent->mFaceVertIndices.size(), initValue);
    mFaceChildEdgeIndices.resize(mParent->mFaceEdgeIndices.size(), initValue);
    mEdgeChildEdgeIndices.resize(mParent->mEdgeVertIndices.size(), initValue);

    mFaceChildVertIndex.resize(mParent->faceCount(), initValue);
    mEdgeChildVertIndex.resize(mParent->edgeCount(), initValue);
    mVertChildVertIndex.resize(mParent->vertCount(), initValue);
   
   //
   //  Mark components here -- accounting for holes or otherwise adaptive/selective:
   //
   //  It's unclear how best to do this, i.e. how to do it most efficiently in general.  We
   //  can first identify parent faces, then all incident vertices will need to have a child
   //  face for all incident parent faces.  Once all of the child faces have been identified,
   //  all associated parent components and child components can be identified and marked.
   //  It is possible to mark multiple component types as we go, but we may be better off
   //  only writing to one at a time (efficiency in general but also threading potential).
   //
   //  (Mark everything -- or assume marked -- for now)
   //      - see usage of initValue above.
   //
}

namespace {
    int
    sequenceSparseIndexVector(VtrIndexVector& indexVector, int baseValue = 0)
    {
        int validCount = 0;
        for (int i = 0; i < (int) indexVector.size(); ++i) {
            if (VtrIndexIsValid(indexVector[i])) {
                indexVector[i] = baseValue + validCount++;
            }
        }
        return validCount;
    }
}

void
VtrRefinement::sequenceAndCountChildComponents()
{
    //
    //  Assign the sequences of child components by inspecting what was marked:
    //
    //  Note that for vertices (and edges) the sequence is assembled from three source vectors
    //  for vertices originating from faces, edges and vertices:
    //
    //  child faces:
    mChildFaceFromFaceCount = sequenceSparseIndexVector(this->mFaceChildFaceIndices);

    //  child edges:
    mChildEdgeFromFaceCount = sequenceSparseIndexVector(this->mFaceChildEdgeIndices);
    mChildEdgeFromEdgeCount = sequenceSparseIndexVector(this->mEdgeChildEdgeIndices,
                                                        mChildEdgeFromFaceCount);

    //  child vertices:
    mChildVertFromFaceCount = sequenceSparseIndexVector(this->mFaceChildVertIndex);
    mChildVertFromEdgeCount = sequenceSparseIndexVector(this->mEdgeChildVertIndex,
                                                        mChildVertFromFaceCount);
    mChildVertFromVertCount = sequenceSparseIndexVector(this->mVertChildVertIndex,
                                                        mChildVertFromFaceCount +
                                                        mChildVertFromEdgeCount);

    //
    //  Determine the inventory of the child's components and assign to the child:
    //
    mChild->_faceCount = mChildFaceFromFaceCount;
    mChild->_edgeCount = mChildEdgeFromFaceCount + mChildEdgeFromEdgeCount;
    mChild->_vertCount = mChildVertFromFaceCount + mChildVertFromEdgeCount + mChildVertFromVertCount;
}


//
//  Before we can refine the topological relations, we want to have the vectors
//  sized appropriately so that we can (potentially) distribute the computation
//  over chunks of these vectors.
//
//  This is non-trivial in the case of sparse subdivision, but there are still
//  some opportunities for concurrency within these...
//
//  We will know the maximal size of each relation based on the origin of the
//  child component -- so we could over-allocate.  If the refinement is very
//  sparse the greatest savings will come from the small number of components
//  and the overhead in the relations of each should be miminal (there should
//  be a mix between maximal and minimal size given the mix of both interior
//  and exterior components when sparse).
//
//  Is it worth populating the counts concurrently, then making a single
//  pass through them to assemble the offsets?
//
void
VtrRefinement::initializeChildFaceVertCountsAndOffsets()
{
    VtrLevel& child = *mChild;

    //
    //  Be aware of scheme-specific decisions here, e.g. the current use
    //  of 4 for quads for Catmark -- must adjust for Loop, Bilinear and
    //  account for possibility of both quads and tris...
    //
    child.mFaceVertCountsAndOffsets.resize(child.faceCount() * 2);
    for (int i = 0; i < child.faceCount(); ++i) {
        child.mFaceVertCountsAndOffsets[i*2 + 0] = 4;
        child.mFaceVertCountsAndOffsets[i*2 + 1] = i << 2;
    }
}
void
VtrRefinement::initializeChildEdgeFaceCountsAndOffsets()
{
    //
    //  Be aware of scheme-specific decisions here, e.g.:
    //      - inspection of sparse child faces for edges from faces
    //      - no guaranteed "neighborhood" around Bilinear verts from verts
    //
    //  If uniform subdivision, face count of a child edge will be:
    //      - 2 for edges from parent faces
    //      - same as parent edge for edges from parent edges
    //  If sparse subdivision, face count of a child edge will be:
    //      - 1 or 2 depending on child faces in parent face
    //          - requires inspection if not all child faces present
    //      ? same as parent edge for edges from parent edges
    //          - given end vertex must have its full set of child faces
    //          - not for Bilinear -- only if neighborhood is non-zero
    //
}
void
VtrRefinement::initializeChildVertFaceCountsAndOffsets()
{
    //
    //  Be aware of scheme-specific decisions here, e.g.:
    //      - no verts from parent faces for Loop
    //      - more interior edges and faces for verts from parent edges for Loop
    //      - no guaranteed "neighborhood" around Bilinear verts from verts
    //
    //  If uniform subdivision, vert-face count will be:
    //      - 4 for verts from parent faces (for catmark)
    //      - 2x number in parent edge for verts from parent edges
    //      - same as parent vert for verts from parent verts
    //  If sparse subdivision, vert-face count will be:
    //      - the number of child faces in parent face
    //      - 1 or 2x number in parent edge for verts from parent edges
    //          - where the 1 or 2 is number of child edges of parent edge
    //      - same as parent vert for verts from parent verts (catmark)
    //
}
void
VtrRefinement::initializeChildVertEdgeCountsAndOffsets()
{
    //
    //  Be aware of scheme-specific decisions here, e.g.:
    //      - no verts from parent faces for Loop
    //      - more interior edges and faces for verts from parent edges for Loop
    //      - no guaranteed "neighborhood" around Bilinear verts from verts
    //
    //  If uniform subdivision, vert-edge count will be:
    //      - 4 for verts from parent faces (for catmark)
    //      - 2 + N faces incident parent edge for verts from parent edges
    //      - same as parent vert for verts from parent verts
    //  If sparse subdivision, vert-edge count will be:
    //      - non-trivial function of child faces in parent face
    //          - 1 child face will always result in 2 child edges
    //          * 2 child faces can mean 3 or 4 child edges
    //          - 3 child faces will always result in 4 child edges
    //      - 1 or 2 + N faces incident parent edge for verts from parent edges
    //          - where the 1 or 2 is number of child edges of parent edge
    //          - any end vertex will require all N child faces (catmark)
    //      - same as parent vert for verts from parent verts (catmark)
    //
}


//
//  Face-vert and face-edge topology propogation -- faces only originate from faces:
//
inline void
VtrRefinement::populateChildFaceVertsFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    //
    //  Algorithm:
    //    - iterate through parent face-child-face vector (could use back-vector)
    //    - use parent components incident the parent face:
    //        - use the interior face-vert, corner vert-vert and two edge-verts
    //
    VtrIndexAccessor pFaceVerts = parent.accessFaceVerts(pFace);
    VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexAccessor pFaceChildren = this->accessFaceChildFaces(pFace);

    int pFaceVertCount  = pFaceVerts.size();
    for (int j = 0; j < pFaceVertCount; ++j) {
        VtrIndex cFace = pFaceChildren[j];
        if (VtrIndexIsValid(cFace)) {
            VtrIndexModifier cFaceVerts = child.modifyFaceVerts(cFace);

            int jPrev = j ? (j - 1) : (pFaceVertCount - 1);

            //  Note ORIENTATION dependency -- regular vs extra-ordinary...
            cFaceVerts[0] = this->mFaceChildVertIndex[pFace];
            cFaceVerts[1] = this->mEdgeChildVertIndex[pFaceEdges[jPrev]];
            cFaceVerts[2] = this->mVertChildVertIndex[pFaceVerts[j]];
            cFaceVerts[3] = this->mEdgeChildVertIndex[pFaceEdges[j]];
        }
    }
}

inline void
VtrRefinement::populateChildFaceEdgesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    //
    //  Algorithm:
    //    - iterate through parent face-child-face vector (could use back-vector)
    //    - use parent components incident the parent face:
    //        - use the two interior face-edges and the two boundary edge-edges
    //
    VtrIndexAccessor pFaceVerts = parent.accessFaceVerts(pFace);
    VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexAccessor pFaceChildFaces = this->accessFaceChildFaces(pFace);
    VtrIndexAccessor pFaceChildEdges = this->accessFaceChildEdges(pFace);

    int pFaceValence = pFaceVerts.size();

    for (int j = 0; j < pFaceValence; ++j) {
        VtrIndex cFace = pFaceChildFaces[j];
        if (VtrIndexIsValid(cFace)) {
            VtrIndexModifier cFaceEdges = child.modifyFaceEdges(cFace);

            int jPrev = j ? (j - 1) : (pFaceValence - 1);

            //  Note ORIENTATION dependency -- regular vs extra-ordinary...

            //  First, the two child-edges of the parent face:
            cFaceEdges[0] = pFaceChildEdges[jPrev];
            cFaceEdges[3] = pFaceChildEdges[j];

            VtrIndex pCornerVert = pFaceVerts[j];

            VtrIndex         pPrevEdge      = pFaceEdges[jPrev];
            VtrIndexAccessor pPrevEdgeVerts = parent.accessEdgeVerts(pPrevEdge);

            VtrIndex         pNextEdge      = pFaceEdges[j];
            VtrIndexAccessor pNextEdgeVerts = parent.accessEdgeVerts(pNextEdge);

            int cornerInPrevEdge = (pPrevEdgeVerts[0] != pCornerVert);
            int cornerInNextEdge = (pNextEdgeVerts[0] != pCornerVert);

            cFaceEdges[1] = this->accessEdgeChildEdges(pPrevEdge)[cornerInPrevEdge];
            cFaceEdges[2] = this->accessEdgeChildEdges(pNextEdge)[cornerInNextEdge];
        }
    }
}

//
//  Edge-vert topology propogation -- two functions for face or edge origin:
//
inline void
VtrRefinement::populateChildEdgeVertsFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    //
    //  For each parent face's edge-children:
    //    - identify parent face's vert-child (note it is shared by all)
    //    - identify parent edge perpendicular to face's child edge:
    //        - identify parent edge's vert-child
    //
    VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexAccessor pFaceChildren = this->accessFaceChildEdges(pFace);

    for (int j = 0; j < pFaceEdges.size(); ++j) {
        VtrIndex cEdge = pFaceChildren[j];
        if (VtrIndexIsValid(cEdge)) {
            VtrIndexModifier cEdgeVerts = child.modifyEdgeVerts(cEdge);

            cEdgeVerts[0] = this->mFaceChildVertIndex[pFace];
            cEdgeVerts[1] = this->mEdgeChildVertIndex[pFaceEdges[j]];
        }
    }
}

inline void
VtrRefinement::populateChildEdgeVertsFromParentEdge(VtrIndex pEdge)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    //
    //  For each parent edge's edge-children:
    //    - identify parent edge's vert-child (potentially shared by both)
    //    - identify parent vert at end of child edge:
    //        - identify parent vert's vert-child
    //
    VtrIndexAccessor pEdgeVerts = parent.accessEdgeVerts(pEdge);

    VtrIndexAccessor pEdgeChildren = this->accessEdgeChildEdges(pEdge);

    //  May want to unroll this trivial loop of 2...
    for (int j = 0; j < 2; ++j) {
        VtrIndex cEdge = pEdgeChildren[j];
        if (VtrIndexIsValid(cEdge)) {
            VtrIndexModifier cEdgeVerts = child.modifyEdgeVerts(cEdge);

            cEdgeVerts[0] = this->mEdgeChildVertIndex[pEdge];
            cEdgeVerts[1] = this->mVertChildVertIndex[pEdgeVerts[j]];
        }
    }
}

//
//  Edge-face topology propogation -- two functions for face or edge origin:
//
inline void
VtrRefinement::populateChildEdgeFacesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    VtrIndexAccessor pFaceChildFaces = this->accessFaceChildFaces(pFace);
    VtrIndexAccessor pFaceChildEdges = this->accessFaceChildEdges(pFace);

    int pFaceValence = parent.accessFaceVerts(pFace).size();

    for (int j = 0; j < pFaceValence; ++j) {
        VtrIndex cEdge = pFaceChildEdges[j];
        if (VtrIndexIsValid(cEdge)) {
            //
            //  Reserve enough edge-faces, populate and trim as needed:
            //
            child.resizeEdgeFaces(cEdge, 2);

            VtrIndexModifier cEdgeFaces = child.modifyEdgeFaces(cEdge);

            //  One or two child faces may be assigned:
            int jNext = ((j + 1) < pFaceValence) ? (j + 1) : 0;

            int cEdgeFaceCount = 0;
            if (VtrIndexIsValid(pFaceChildFaces[j])) {
                cEdgeFaces[cEdgeFaceCount++] = pFaceChildFaces[j];
            }
            if (VtrIndexIsValid(pFaceChildFaces[jNext])) {
                cEdgeFaces[cEdgeFaceCount++] = pFaceChildFaces[jNext];
            }
            child.trimEdgeFaces(cEdge, cEdgeFaceCount);
        }
    }
}

inline void
VtrRefinement::populateChildEdgeFacesFromParentEdge(VtrIndex pEdge)
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    VtrIndexAccessor pEdgeVerts = parent.accessEdgeVerts(pEdge);
    VtrIndexAccessor pEdgeFaces = parent.accessEdgeFaces(pEdge);

    VtrIndexAccessor pEdgeChildEdges = this->accessEdgeChildEdges(pEdge);

    for (int j = 0; j < 2; ++j) {
        VtrIndex cEdge = pEdgeChildEdges[j];
        if (!VtrIndexIsValid(cEdge)) continue;

        //
        //  Reserve enough edge-faces, populate and trim as needed:
        //
        child.resizeEdgeFaces(cEdge, pEdgeFaces.size());

        VtrIndexModifier cEdgeFaces = child.modifyEdgeFaces(cEdge);

        //
        //  Each parent face may contribute an incident child face:
        //
        //      This is awkward, and would be greatly simplified by storing the
        //  "edge in face" for each edge-face (as we do for "vert in face" of
        //  the vert-faces, etc.).  For each incident face we then immediately
        //  know the two child faces that are associated with the two child
        //  edges -- we just need to identify how to pair them based on the
        //  edge direction.
        //
        //      Note also here, that we could identify the pairs of child faces
        //  once for the parent before dealing with each child edge (we do the
        //  "find edge in face search" twice here as a result).  We will
        //  generally have 2 or 1 incident face to the parent edge so we
        //  can put the child-pairs on the stack.
        //
        //      Here's a more promising alternative -- instead of iterating
        //  through the child edges to "pull" data from the parent, iterate
        //  through the parent edges' faces and apply valid child faces to
        //  the appropriate child edge.  We should be able to use end-verts
        //  of the parent edge to get the corresponding child face for each,
        //  but we can't avoid a vert-in-face search and a subsequent parity
        //  test of the end-vert.
        //
        int cEdgeFaceCount = 0;

        for (int i = 0; i < pEdgeFaces.size(); ++i) {
            VtrIndex pFace = pEdgeFaces[i];

            VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFace);
            VtrIndexAccessor pFaceVerts = parent.accessFaceVerts(pFace);

            VtrIndexAccessor pFaceChildren = this->accessFaceChildFaces(pFace);

            int pFaceValence = pFaceVerts.size();

            int edgeInFace = 0;
            for ( ; pFaceEdges[edgeInFace] != pEdge; ++edgeInFace) ;

            //  Inspect either this child of the face or the next:
            int childInFace = edgeInFace + (pFaceVerts[edgeInFace] != pEdgeVerts[j]);
            if (childInFace == pFaceValence) childInFace = 0;

            if (VtrIndexIsValid(pFaceChildren[childInFace])) {
                cEdgeFaces[cEdgeFaceCount++] = pFaceChildren[childInFace];
            }
        }
        child.trimEdgeFaces(cEdge, cEdgeFaceCount);
    }
}

//
//  Populating the entire incident index vectors -- these just wrap the
//  iteration over all components.
//
void
VtrRefinement::populateFaceEdgesFromParentFaces()
{
    for (int fIndex = 0; fIndex < mParent->faceCount(); ++fIndex) {
        populateChildFaceEdgesFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateFaceVertsFromParentFaces()
{
    for (int fIndex = 0; fIndex < mParent->faceCount(); ++fIndex) {
        populateChildFaceVertsFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateEdgeVertsFromParentFaces()
{
    for (int fIndex = 0; fIndex < mParent->faceCount(); ++fIndex) {
        populateChildEdgeVertsFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateEdgeVertsFromParentEdges()
{
    for (int eIndex = 0; eIndex < mParent->edgeCount(); ++eIndex) {
        populateChildEdgeVertsFromParentEdge(eIndex);
    }
}
void
VtrRefinement::populateEdgeFacesFromParentFaces()
{
    //
    //  Note -- the edge-face counts/offsets vector is not known
    //  ahead of time and is populated incrementally, so we cannot
    //  thread this yet...
    //
    for (int fIndex = 0; fIndex < mParent->faceCount(); ++fIndex) {
        populateChildEdgeFacesFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateEdgeFacesFromParentEdges()
{
    //
    //  Note -- the edge-face counts/offsets vector is not known
    //  ahead of time and is populated incrementally, so we cannot
    //  thread this yet...
    //
    for (int eIndex = 0; eIndex < mParent->edgeCount(); ++eIndex) {
        populateChildEdgeFacesFromParentEdge(eIndex);
    }
}


//
//  Vert-face topology propogation -- three functions for face, edge or vert origin:
//
//  Remember for these that the corresponding counts/offsets for each component are
//  not yet known and so are also populated by these functions.  This does impose an
//  ordering requirement here and inhibits concurrency.
//
void
VtrRefinement::populateVertFacesFromParentFaces()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int fIndex = 0; fIndex < parent.faceCount(); ++fIndex) {
        int cVertIndex = this->mFaceChildVertIndex[fIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent face first:
        //
        int pFaceVertCount  = parent.accessFaceVerts(fIndex).size();

        VtrIndexAccessor pFaceChildren = this->accessFaceChildFaces(fIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, pFaceVertCount);

        VtrIndexModifier      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexModifier cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int j = 0; j < pFaceVertCount; ++j) {
            //
            //  There should eventually be a conditional here so support sparse refinement:
            //
            cVertFaces[j] = pFaceChildren[j];

            //  Note ORIENTATION dependency -- this will eventually need to vary
            //  with the valence of the face to be consistent with hier edits...
            //
            // cVertInFace[j] = (pFaceVertCount == 4) ? j : 0;
            cVertInFace[j] = 0;
            cVertFaceCount++;
        }
        child.trimVertFaces(cVertIndex, cVertFaceCount);
    }
}
void
VtrRefinement::populateVertFacesFromParentEdges()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int pEdgeIndex = 0; pEdgeIndex < parent.edgeCount(); ++pEdgeIndex) {
        int cVertIndex = this->mEdgeChildVertIndex[pEdgeIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent edge first:
        //
        VtrIndexAccessor pEdgeFaces = parent.accessEdgeFaces(pEdgeIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, 2 * pEdgeFaces.size());

        VtrIndexModifier      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexModifier cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int i = 0; i < pEdgeFaces.size(); ++i) {
            //
            //  identify the parent edge within this parent face -- this is where
            //  augmenting the edge-face relation with the "child index" is useful:
            //
            VtrIndex pFaceIndex  = pEdgeFaces[i];

            VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFaceIndex);

            VtrIndexAccessor pFaceChildren = this->accessFaceChildFaces(pFaceIndex);

            //
            //  Identify the corresponding two child faces for this parent face and
            //  assign those of the two that are valid:
            //
            int faceChild0 = 0;
            for ( ; pFaceEdges[faceChild0] != pEdgeIndex; ++faceChild0) ;

            int faceChild1 = faceChild0 + 1;
            if (faceChild1 == pFaceEdges.size()) faceChild1 = 0;

            //  For counter-clockwise ordering of faces, consider the second face
            //  first:
            if (VtrIndexIsValid(pFaceChildren[faceChild1])) {
                cVertFaces[cVertFaceCount] = pFaceChildren[faceChild1];
                cVertInFace[cVertFaceCount] = 1;
                cVertFaceCount++;
            }
            if (VtrIndexIsValid(pFaceChildren[faceChild0])) {
                cVertFaces[cVertFaceCount] = pFaceChildren[faceChild0];
                cVertInFace[cVertFaceCount] = 3;
                cVertFaceCount++;
            }
        }
        child.trimVertFaces(cVertIndex, cVertFaceCount);
    }
}
void
VtrRefinement::populateVertFacesFromParentVerts()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int vIndex = 0; vIndex < parent.vertCount(); ++vIndex) {
        int cVertIndex = this->mVertChildVertIndex[vIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent vert's faces:
        //
        VtrIndexAccessor      pVertFaces  = parent.accessVertFaces(vIndex);
        VtrLocalIndexAccessor pVertInFace = parent.accessVertFaceLocalIndices(vIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, pVertFaces.size());

        VtrIndexModifier      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexModifier cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int i = 0; i < pVertFaces.size(); ++i) {
            VtrIndex      pFaceIndex  = pVertFaces[i];
            VtrLocalIndex pFaceVert = pVertInFace[i];

            //  Note ORIENTATION dependency -- this will eventually need to vary
            //  with the valence of the face to be consistent with hier edits...
            int pFaceCount = parent.accessFaceVerts(pFaceIndex).size();

            VtrIndex pFaceChildIndex = this->accessFaceChildFaces(pFaceIndex)[pFaceVert];
            if (VtrIndexIsValid(pFaceChildIndex)) {
                cVertFaces[cVertFaceCount] = pFaceChildIndex;
                cVertInFace[cVertFaceCount] = (pFaceCount > 4) ? pFaceVert : 2;
                cVertFaceCount++;
            }
        }
        child.trimVertFaces(cVertIndex, cVertFaceCount);
    }
}

//
//  Vert-edge topology propogation -- three functions for face, edge or vert origin:
//
void
VtrRefinement::populateVertEdgesFromParentFaces()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int fIndex = 0; fIndex < parent.faceCount(); ++fIndex) {
        int cVertIndex = this->mFaceChildVertIndex[fIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent face first:
        //
        VtrIndexAccessor pFaceVerts = parent.accessFaceVerts(fIndex);

        VtrIndexAccessor pFaceChildren = this->accessFaceChildEdges(fIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pFaceVerts.size());

        VtrIndexModifier      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexModifier cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

        int cVertEdgeCount = 0;
        for (int j = 0; j < pFaceVerts.size(); ++j) {
            if (VtrIndexIsValid(pFaceChildren[j])) {
                cVertEdges[cVertEdgeCount] = pFaceChildren[j];
                cVertInEdge[cVertEdgeCount] = 0;
                cVertEdgeCount++;
            }
        }
        child.trimVertEdges(cVertIndex, cVertEdgeCount);
    }
}
void
VtrRefinement::populateVertEdgesFromParentEdges()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int eIndex = 0; eIndex < parent.edgeCount(); ++eIndex) {
        int cVertIndex = this->mEdgeChildVertIndex[eIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  First inspect the parent edge -- its parent faces then its child edges:
        //
        VtrIndexAccessor pEdgeFaces = parent.accessEdgeFaces(eIndex);

        VtrIndexAccessor pEdgeChild = this->accessEdgeChildEdges(eIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pEdgeFaces.size() + 2);

        VtrIndexModifier      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexModifier cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

        //
        //  For each face incident the parent edge -- assign the edge within
        //  that parent face (perpendicular to the parent edge) if valid:
        //
        int cVertEdgeCount = 0;

        for (int i = 0; i < pEdgeFaces.size(); ++i) {
            VtrIndex pFace = pEdgeFaces[i];

            VtrIndexAccessor pFaceEdges = parent.accessFaceEdges(pFace);

            VtrIndexAccessor pFaceChildEdges = this->accessFaceChildEdges(pFace);

            //
            //  Identify the parent edge within this parent face -- this is where
            //  augmenting the edge-face relation with the "child index" is useful:
            //
            int edgeInFace = 0;
            for ( ; pFaceEdges[edgeInFace] != eIndex; ++edgeInFace) ;

            if (VtrIndexIsValid(pFaceChildEdges[edgeInFace])) {
                cVertEdges[cVertEdgeCount] = pFaceChildEdges[edgeInFace];
                cVertInEdge[cVertEdgeCount] = 1;
                cVertEdgeCount++;
            }
        }
        int edgeFromFaceCount = cVertEdgeCount;

        //
        //  For the two possible child edges of the parent edge -- assign if valid:
        //
        for (int i = 0; i < 2; ++i) {
            if (VtrIndexIsValid(pEdgeChild[i])) {
                cVertEdges[cVertEdgeCount] = pEdgeChild[i];
                cVertInEdge[cVertEdgeCount] = 0;
                cVertEdgeCount++;
            }
        }
        int edgeFromEdgeCount = cVertEdgeCount - edgeFromFaceCount;

        //
        //  Note we have ignored the ordering of the edges here -- generating those
        //  perpendicular to the parent edge first and then its children.  A simple
        //  permutation of the results can produce a more desirable ordering, but we
        //  need a bit more information gathered above, e.g. (f0,f1,ex) is ambiguous
        //  as to whether it should be (f0,e0,f1) or (f1,e1,f0)...
        //
        //  Do we need to bother here?  Is ordering of vert-edges ever necessary?
        //
        if (cVertEdgeCount == 4) {
        } else if (edgeFromFaceCount == 2) {
        } else if (edgeFromEdgeCount == 2) {
        }

        child.trimVertEdges(cVertIndex, cVertEdgeCount);
    }
}
void
VtrRefinement::populateVertEdgesFromParentVerts()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (int vIndex = 0; vIndex < parent.vertCount(); ++vIndex) {
        int cVertIndex = this->mVertChildVertIndex[vIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent vert's edges first:
        //
        VtrIndexAccessor      pVertEdges  = parent.accessVertEdges(vIndex);
        VtrLocalIndexAccessor pVertInEdge = parent.accessVertEdgeLocalIndices(vIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pVertEdges.size());

        VtrIndexModifier      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexModifier cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

        int cVertEdgeCount = 0;
        for (int i = 0; i < pVertEdges.size(); ++i) {
            VtrIndex      pEdgeIndex  = pVertEdges[i];
            VtrLocalIndex pEdgeVert = pVertInEdge[i];

            VtrIndex pEdgeChildIndex = this->accessEdgeChildEdges(pEdgeIndex)[pEdgeVert];
            if (VtrIndexIsValid(pEdgeChildIndex)) {
                cVertEdges[cVertEdgeCount] = pEdgeChildIndex;
                cVertInEdge[cVertEdgeCount] = 1;
                cVertEdgeCount++;
            }
        }
        child.trimVertEdges(cVertIndex, cVertEdgeCount);
    }
}


//
//  The main refinement method...
//
//  Note the kinds of child components that are generated:
//    - faces generate child faces, edges and verts
//    - edges generate child edges and verts
//    - verts generate child verts only
//
//  The general approach here will be as follows:
//    - mark incident child components to be generated:
//        - assume all for now but need this for holes, adaptive, etc.
//        - unclear what strategy of child component marking is best:
//            - beginning with a set of parent faces, hole markers, etc.
//            - need to mark children of all parent faces, but also all/some
//              of the adjacent parent faces
//            - marking of child faces needs to also identify/mark child edges
//            - marking of child faces needs to also identify/mark child verts
//    - take inventory of what is marked to resize child vectors
//        ? new members to retain these counts for other purposes:
//            - mFaceChildFaceCount
//            - mFaceChildEdgeCount
//            - mFaceChildVertCount
//            - mEdgeChildEdgeCount
//            - mEdgeChildVertCount
//            - mVertChildVertCount
//        - child components the sum of the above:
//            - new face count = face-faces
//            - new edge count = face-edges + edge-edges
//            - new vert count = face-verts + edge-verts + vert-verts
//    - generate/identify child components between parent and child level:
//        - child vertices from all parent faces
//        - child vertices from all parent edges
//        - child vertices from all parent vertices
//        - child edges from all parent edges
//        - child edges from all parent faces
//        - child faces from all parent faces
//    - populate topology relations:
//        - iterate through each child of each parent component
//            - populate "uninherited" relations directly 
//                - includes face-vert, edge-vert, face-edge
//                - these will always be completely defined
//            - populate "inherited" relations by mapping parent->child
//                - includes vert-face, vert-edge, edge-face
//                - assume one-to-one mapping with parent relation for now
//                    - will be subset of parent for holes, adaptive, etc.
//        - need full complement of parent-child relations
//    - subdivide sharpness values for edges or verts, if present
//        ? hier-edits for sharpness to be applied before/after?
//    ? re-classify/propogate vertex "masks"
//    ? compute stencil weights for all child verts
//        - should mirror the parents topological relations
//    * interpolate -- now deferred externally
//
void
VtrRefinement::refine(bool includeTopology, bool computeStencils)
{
    assert(mParent && mChild);

    //
    //  There's room for experimentation and improvement here -- if uniformly
    //  subdividing we can "mark" and "create" more efficiently in one pass.
    //
    resizeAndMarkParentToChildMappings();
    sequenceAndCountChildComponents();

    //
    //  We can often suppress full topology generation in the last level -- the
    //  parent topology and subdivided sharpness values are enough to be able
    //  to interpolate vertex data.  How to best identify and specify what
    //  aspects should be refined is still unclear.
    //
    if (includeTopology) {
        subdivideTopology();
    } else {
       //  This is a subset of subdivideTopology() for the child's face-verts:
       initializeChildFaceVertCountsAndOffsets();
       mChild->mFaceVertIndices.resize(mChild->faceCount() * 4);
       populateFaceVertsFromParentFaces();
    }

    //
    //  Subdividing sharpness values and classifying child vertices -- note the
    //  issue relating the subdivision of edge-sharpness and classification of
    //  the vertex of a parent edge:  a case can be made for classifying while
    //  computing sharpness values.
    //
    subdivideVertSharpness();
    subdivideEdgeSharpness();

    subdivideVertMask();

    //
    //  Optionally compute the stencil weights -- this is preliminary and needs
    //  to deal with the different schemes appropriately.  Assume Catmark for
    //  now.  Note also that computing stencils for Bilinear is trivial, but the
    //  application of them may end up far more costly than necessary without
    //  specializing the application of them.
    //
    if (computeStencils) {
        computeStencilWeights();
    }
}


//
//  The main method to subdivide the topology -- requires sizing and initialization of
//  child topology vectors and then appropriate assignment of them.
//
void
VtrRefinement::subdivideTopology()
{
    const VtrLevel& parent = *mParent;
          VtrLevel& child  = *mChild;

    //
    //  Sizing and population of the topology vectors:
    //
    //  We have 6 relations to populate, but the way a child component's topology is
    //  determined depends on the nature of its origin from its parent components:
    //
    //      - face-verts:  originate from parent faces
    //      - face-edges:  originate from parent faces
    //      - edge-verts:  originate from parent faces or edges
    //      - edge-faces:  originate from parent faces or edges
    //      - vert-faces:  originate from parent faces or edges or verts
    //      - vert-edges:  originate from parent faces or edges or verts
    //
    //  Each relation has 1-3 cases of origin and so 1-3 methods of population.
    //
    //  It may be worth pairing up the two relations for each child component type,
    //  i.e. populating them together as they share the same type of origin, but keeping
    //  them separate may prove more efficient in general (writing to one destination at
    //  at a time) as well as provide opportunities for threading.
    //
    //  Note on sizing:
    //      All faces now quads or tris, so the face-* relation sizes should be known.
    //  For edge and vert relations, where the number of incident components is more
    //  variable, we allocate/offset assuming 1-to-1 correspondence between child and
    //  parent incident components (which will be true for uniform subdivision), then
    //  "compress" later on-the-fly as we determine the subset of parent components
    //  that contribute children to the neighborhood (not really "compressing" here
    //  but more initializing as we go and adjusting the counts and offsets to reflect
    //  the actual valences -- so we never pack/move the vector elements).
    //      Some of the above inhibits concurrency in the mapping of the child indices
    //  from the parent.  Knowing all of the counts/offsets before applying the mapping
    //  would be helpful.  Current count/offset vectors are:
    //
    //      - face-verts, also used by/shared with face-edges
    //      - edge-faces
    //      - vert-faces
    //      - vert-edges, not shared with vert-faces to allow non-manifold cases
    //
    //  So we have 6 relations to transform and 4 count/offset vectors required -- 3
    //  of which are currently assembled incrementally.  The two that are not build:
    //
    //      - face-edges, we share the face-vert counts/offsets
    //      - edge-verts, no need -- constant size of 2
    //
    //  There are many times were using a constant offset may be preferable, e.g. when
    //  all faces are quads or tris, there are no fin-edges, etc.  Not retrieving the
    //  offsets from memory would be beneficial, but branching at a fine-grain level
    //  to do so may defeat the purpose...
    //
    initializeChildFaceVertCountsAndOffsets();

    //  These are currently empty stubs -- these are assembled incrementally...
    initializeChildEdgeFaceCountsAndOffsets();
    initializeChildVertFaceCountsAndOffsets();
    initializeChildVertEdgeCountsAndOffsets();


    //
    //  Face relations -- both face-verts and face-edges share the same counts/offsets:
    //
    //  Face-verts -- allocate and populate:
    child.mFaceVertIndices.resize(child.faceCount() * 4);

    populateFaceVertsFromParentFaces();

    //  Face-edges -- allocate and populate:
    child.mFaceEdgeIndices.resize(child.faceCount() * 4);

    populateFaceEdgesFromParentFaces();

    //
    //  Edge relations -- edge-verts and edge-faces can be populated independently:
    //
    //  Edge-verts -- allocate and populate:
    child.mEdgeVertIndices.resize(child.edgeCount() * 2);

    populateEdgeVertsFromParentFaces();
    populateEdgeVertsFromParentEdges();

    //  Edge-faces:
    //      NOTE we do not know the exact counts/offsets here because of the
    //  potentially sparse subdivision.  If we must compute this incrementally,
    //  i.e. as we populate the edge-face indices, we will not be able to thread
    //  that operation.  See populateEdgeFaceCountsAndOffsets() for possibilities
    //  of pre-computing these.
    //
    int childEdgeFaceIndexSizeEstimate = parent.mFaceVertIndices.size() * 2 +
                                         parent.mEdgeFaceIndices.size() * 2;

    child.mEdgeFaceCountsAndOffsets.resize(child.edgeCount() * 2);
    child.mEdgeFaceIndices.resize(         childEdgeFaceIndexSizeEstimate);

    populateEdgeFacesFromParentFaces();
    populateEdgeFacesFromParentEdges();

    //  Revise the over-allocated estimate based on what is used (as indicated in the
    //  count/offset for the last vertex) and trim the index vector accordingly:
    childEdgeFaceIndexSizeEstimate = child.mEdgeFaceCountsAndOffsets[(child.edgeCount()-1)*2]
                                   + child.mEdgeFaceCountsAndOffsets[(child.edgeCount()-1)*2 + 1];
    child.mEdgeFaceIndices.resize(childEdgeFaceIndexSizeEstimate);

    //
    //  Vert relations -- vert-faces and vert-edges can be populated independently:
    //
    //  Vert-faces:
    //      We know the number of counts/offsets required, but we do not yet know how
    //  many total incident faces we will have -- given we are generating a potential
    //  subset of the number of children possible, allocate for the maximum and trim
    //  once its determined how many were used.
    //      The upper-bound for allocation is determined by the incidence vectors for
    //  the parent components, e.g. assuming fully populated, the total number of faces
    //  incident all vertices that are generated from parent faces is equal to the size
    //  of the parent's face-vert index vector.  Similar deductions can be made for
    //  for those originating from parent edges and verts.
    //      If this over-allocation proves excessive, we can get a more reasonable
    //  estimate, or even the exact size, by iterating though topology/child vectors.
    //
    //      The bigger issue here though is that not having the counts/offsets known
    //  and updating it incrementally inhibits threading.  So consider figuring out the
    //  counts/offsets before remapping the topology:
    //      - if uniform subdivision, vert-face count will be:
    //          - 4 for verts from parent faces (for catmark)
    //          - 2x number in parent edge for verts from parent edges
    //          - same as parent vert for verts from parent verts
    //      - if sparse subdivision, vert-face count will be:
    //          - the number of child faces in parent face
    //          - 1 or 2x number in parent edge for verts from parent edges
    //              - where the 1 or 2 is number of child edges of parent edge
    //          - same as parent vert for verts from parent verts (catmark)
    //
    //  Note a couple of these metrics are Catmark-dependent, i.e. assuming 4 child
    //  faces per face, but more subtely the assumption that a child vert from a
    //  parent vert will have the same valence -- this will only be true if the
    //  subd Scheme requires a "neighborhood of 1", which will have been taken into
    //  account when marking/generating children.
    //
    int childVertFaceIndexSizeEstimate = parent.mFaceVertIndices.size()
                                        + parent.mEdgeFaceIndices.size() * 2 
                                        + parent.mVertFaceIndices.size();

    child.mVertFaceCountsAndOffsets.resize(child.vertCount() * 2);
    child.mVertFaceIndices.resize(         childVertFaceIndexSizeEstimate);
    child.mVertFaceLocalIndices.resize(    childVertFaceIndexSizeEstimate);

    populateVertFacesFromParentFaces();
    populateVertFacesFromParentEdges();
    populateVertFacesFromParentVerts();

    //  Revise the over-allocated estimate based on what is used (as indicated in the
    //  count/offset for the last vertex) and trim the index vectors accordingly:
    childVertFaceIndexSizeEstimate = child.mVertFaceCountsAndOffsets[(child.vertCount()-1)*2]
                                   + child.mVertFaceCountsAndOffsets[(child.vertCount()-1)*2 + 1];
    child.mVertFaceIndices.resize(     childVertFaceIndexSizeEstimate);
    child.mVertFaceLocalIndices.resize(childVertFaceIndexSizeEstimate);

    //  Vert-edges:
    //      As for vert-faces, we know the number of counts/offsets required, but we do
    //  not yet know how many total incident edges we will have.  So over-estimate and
    //  trim after population, similar to above for face-verts.
    //
    //  See also notes above regarding attempts to determine counts/offsets before we
    //  start to remap the components:
    //      - if uniform subdivision, vert-edge count will be:
    //          - 4 for verts from parent faces (for catmark)
    //          - 2 + N faces incident parent edge for verts from parent edges
    //          - same as parent vert for verts from parent verts
    //      - if sparse subdivision, vert-edge count will be:
    //          - non-trivial function of child faces in parent face
    //              - 1 child face will always result in 2 child edges
    //              * 2 child faces can mean 3 or 4 child edges
    //              - 3 child faces will always result in 4 child edges
    //          - 1 or 2 + N faces incident parent edge for verts from parent edges
    //              - where the 1 or 2 is number of child edges of parent edge
    //              - any end vertex will require all N child faces (catmark)
    //          - same as parent vert for verts from parent verts (catmark)
    //
    int childVertEdgeIndexSizeEstimate = parent.mFaceVertIndices.size()
                                       + parent.mEdgeFaceIndices.size() + parent.edgeCount() * 2 
                                       + parent.mVertEdgeIndices.size();

    child.mVertEdgeCountsAndOffsets.resize(child.vertCount() * 2);
    child.mVertEdgeIndices.resize(         childVertEdgeIndexSizeEstimate);
    child.mVertEdgeLocalIndices.resize(    childVertEdgeIndexSizeEstimate);

    populateVertEdgesFromParentFaces();
    populateVertEdgesFromParentEdges();
    populateVertEdgesFromParentVerts();

    //  Revise the over-allocated estimate based on what is used (as indicated in the
    //  count/offset for the last vertex) and trim the index vectors accordingly:
    childVertEdgeIndexSizeEstimate = child.mVertEdgeCountsAndOffsets[(child.vertCount()-1)*2]
                                   + child.mVertEdgeCountsAndOffsets[(child.vertCount()-1)*2 + 1];
    child.mVertEdgeIndices.resize(     childVertEdgeIndexSizeEstimate);
    child.mVertEdgeLocalIndices.resize(childVertEdgeIndexSizeEstimate);

    //
    //  Remember to set the member flag indicating full topology:
    //
    child._hasTopology = true;
}


//
//  Sharpness subdivision methods:
//      Subdividing vertex sharpness is relatively trivial -- most complexity is in
//  the edge sharpness given its varying computation methods.
//
void
VtrRefinement::subdivideVertSharpness()
{
    SdcCrease creasing(_schemeOptions);

    mChild->mVertSharpness.clear();
    mChild->mVertSharpness.resize(mChild->vertCount(), SdcCrease::SMOOTH);

    //
    //  All child-verts originating from faces or edges are initialized as smooth
    //  above.  Only those originating from vertices require "subdivided" values:
    //
    for (VtrIndex pVert = 0; pVert < mParent->vertCount(); ++pVert) {
        VtrIndex cVert = this->mVertChildVertIndex[pVert];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrSharpness const& pSharpness = mParent->mVertSharpness[pVert];
        VtrSharpness&       cSharpness = mChild->mVertSharpness[cVert];

        cSharpness = creasing.SubdivideVertexSharpness(pSharpness);
    }
}

void
VtrRefinement::subdivideEdgeSharpness()
{
    SdcCrease creasing(_schemeOptions);

    mChild->mEdgeSharpness.clear();
    mChild->mEdgeSharpness.resize(mChild->edgeCount(), SdcCrease::SMOOTH);

    //
    //  Edge sharpness is passed to child-edges using the parent edge and the
    //  parent vertex for which the child corresponds.  Child-edges are created
    //  from both parent faces and parent edges, but those child-edges created
    //  from a parent face should be within the face's interior and so smooth
    //  (and so previously initialized).
    //
    //  The presence/validity of each parent edges child vert indicates one or
    //  more child edges.
    //
    //  NOTE -- It is also useful at this time to classify the child vert of
    //  this edge based on the creasing information here, particularly when a
    //  non-trivial creasing method like Chaikin is used.  This is not being
    //  done now but is worth considering...
    //
    for (VtrIndex pEdge = 0; pEdge < mParent->edgeCount(); ++pEdge) {
        VtrIndex cVert = this->mEdgeChildVertIndex[pEdge];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrIndexAccessor cEdges = this->accessEdgeChildEdges(pEdge);

        VtrSharpness pSharpness = mParent->mEdgeSharpness[pEdge];
        if (pSharpness <= SdcCrease::SMOOTH) {
            if (VtrIndexIsValid(cEdges[0])) mChild->mEdgeSharpness[cEdges[0]] = SdcCrease::SMOOTH;
            if (VtrIndexIsValid(cEdges[1])) mChild->mEdgeSharpness[cEdges[1]] = SdcCrease::SMOOTH;
        } else if (pSharpness >= SdcCrease::INFINITE) {
            if (VtrIndexIsValid(cEdges[0])) mChild->mEdgeSharpness[cEdges[0]] = SdcCrease::INFINITE;
            if (VtrIndexIsValid(cEdges[1])) mChild->mEdgeSharpness[cEdges[1]] = SdcCrease::INFINITE;
        } else if (creasing.IsSimple()) {
            VtrSharpness cSharpness = std::max<float>(pSharpness - 1.0f, SdcCrease::SMOOTH);
            if (VtrIndexIsValid(cEdges[0])) mChild->mEdgeSharpness[cEdges[0]] = cSharpness;
            if (VtrIndexIsValid(cEdges[1])) mChild->mEdgeSharpness[cEdges[1]] = cSharpness;
        } else {
            //
            //  Non-simple sharpness computation requires gathering the sharpness values around
            //  each end vertex and computing the sharpness for each child edge according to
            //  its adjoining vertex.  So gather the sharpness values and compute for each end:
            //
            //  NOTE -- currently one of the two child edges may be ignore here.  If we want to
            //  determine the Rule associated with the vertex subdivided from this edge, we will
            //  need to compute both to classify the vertex, though only one may be retained...
            //
            VtrIndexAccessor pEdgeVerts = mParent->accessEdgeVerts(pEdge);

            for (int i = 0; i < 2; ++i) {
                if (VtrIndexIsValid(cEdges[i])) {
                    VtrIndexAccessor pVertEdges = mParent->accessVertEdges(pEdgeVerts[i]);

                    float pVertEdgeSharpness[pVertEdges.size() + 1];
                    for (int j = 0; j < pVertEdges.size(); ++j) {
                        pVertEdgeSharpness[j] = mParent->mEdgeSharpness[pVertEdges[j]];
                    }

                    mChild->mEdgeSharpness[cEdges[i]] = creasing.SubdivideEdgeSharpnessAtVertex(
                            pSharpness, pVertEdges.size(), pVertEdgeSharpness);
                }
            }
        }
    }
}


//
//  Methods to classify the type of child vertices for later stencil evaluation:
//
void
VtrRefinement::classifyChildVertsFromParentFaces()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (VtrIndex pFace = 0; pFace < parent.faceCount(); ++pFace) {
        VtrIndex cVert = this->mFaceChildVertIndex[pFace];
        if (!VtrIndexIsValid(cVert)) continue;

        child.mVertRule[cVert] = SdcCrease::RULE_SMOOTH;
    }
}
void
VtrRefinement::classifyChildVertsFromParentEdges()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (VtrIndex pEdge = 0; pEdge < parent.edgeCount(); ++pEdge) {
        VtrIndex cVert = this->mEdgeChildVertIndex[pEdge];
        if (!VtrIndexIsValid(cVert)) continue;

        //
        //  We really need to look at the sharpnesses of the two child edges here, but we
        //  may have the situation that only one child edge of the parent was was generated.
        //  In such cases the sharpness of the other child is not available and we cannot
        //  truly know the nature of this edge-vert without it.
        //
        //  But this will only matter when one child-edge is sharp while the other is smooth,
        //  which can only happen with Chaiken creasing and only when the vertex was on the
        //  outer edge of the one-ring neighborhood required to define a more interior face.
        //  So if only one edge is present we will assume the missing edge is of the same
        //  sharpness.  It should not matter that this vert might be mis-classified as further
        //  refinement should ignore such a vert, i.e. not generate a child-vertex from it.
        //
        VtrIndexAccessor cEdges = this->accessEdgeChildEdges(pEdge);

        child.mVertRule[cVert] = SdcCrease::RULE_SMOOTH;
        if (VtrIndexIsValid(cEdges[0]) && (child.mEdgeSharpness[cEdges[0]] > SdcCrease::SMOOTH)) {
            child.mVertRule[cVert] = SdcCrease::RULE_CREASE;
        } else if (VtrIndexIsValid(cEdges[1]) && (child.mEdgeSharpness[cEdges[1]] > SdcCrease::SMOOTH)) {
            child.mVertRule[cVert] = SdcCrease::RULE_CREASE;
        }
    }
}
void
VtrRefinement::classifyChildVertsFromParentVerts()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    for (VtrIndex pVert = 0; pVert < parent.vertCount(); ++pVert) {
        VtrIndex cVert = this->mVertChildVertIndex[pVert];
        if (!VtrIndexIsValid(cVert)) continue;

        //
        //  We really only only need to compute a new value here for vertices
        //  that are semi-sharp (or "volatil" in Hbr's terms) -- which we could
        //  flag in the parent vertex and propogate to its children.  In all
        //  other cases we should be able to inherit the parent's Rules.
        //
        //  For now, we can at least do that for a vertex that is smooth, which
        //  will be most of them...
        //
        if (parent.mVertRule[pVert] == SdcCrease::RULE_SMOOTH) {
            child.mVertRule[cVert] = parent.mVertRule[pVert];
        } else if (child.mVertSharpness[cVert] > 0.0) {
            child.mVertRule[cVert] = SdcCrease::RULE_CORNER;
        } else {
            SdcCrease creasing(_schemeOptions);

            int   incEdgeCount = parent.accessVertEdges(pVert).size();
            float incEdgeSharpness[incEdgeCount + 1];

            float const* childEdgeSharpness = &child.mEdgeSharpness[0];
            
            //
            //  Gather the child edge sharpness -- using the child's topology if present,
            //  otherwise, using the parent's incident edges to identify the subdivided
            //  sharpness values for its subedges:
            //
            if (child._hasTopology) {
                VtrIndexAccessor childEdges = child.accessVertEdges(cVert);
                assert(childEdges.size() == incEdgeCount);

                for (int i = 0; i < incEdgeCount; ++i) {
                    incEdgeSharpness[i] = childEdgeSharpness[childEdges[i]];
                }
            } else {
                VtrIndexAccessor parentEdges = parent.accessVertEdges(pVert);
                assert(parentEdges.size() == incEdgeCount);

                for (int i = 0; i < incEdgeCount; ++i) {
                    //  CORRECTNESS -- need to choose the correct child-edge here based on cVert:
                    VtrIndex childEdge = this->accessEdgeChildEdges(parentEdges[i])[0];

                    incEdgeSharpness[i] = childEdgeSharpness[childEdge];
                }
            }
            child.mVertRule[cVert] = creasing.DetermineVertexVertexRule(0.0, incEdgeCount, incEdgeSharpness);
        }
    }
}

void
VtrRefinement::subdivideVertMask()
{
    mChild->mVertRule.clear();
    mChild->mVertRule.resize(mChild->vertCount());

    classifyChildVertsFromParentFaces();
    classifyChildVertsFromParentEdges();
    classifyChildVertsFromParentVerts();
}

void
VtrRefinement::computeStencilWeights()
{
    const VtrLevel& parent = *this->mParent;
          VtrLevel& child  = *this->mChild;

    assert(child.vertCount() != 0);

    mFaceVertWeights.resize(parent.faceVertCount());
    mEdgeVertWeights.resize(parent.edgeVertCount());
    mEdgeFaceWeights.resize(parent.edgeFaceCount());
    mVertVertWeights.resize(parent.vertCount());
    mVertEdgeWeights.resize(parent.vertEdgeCount());
    mVertFaceWeights.resize(parent.vertEdgeCount());

    //
    //  Hard-coding this for Catmark temporarily for testing...
    //
    assert(child.vertCount() != 0);
    assert(_schemeType == CATMARK);

    SdcScheme<CATMARK> scheme(_schemeOptions);

    if (mChildVertFromFaceCount) {
        SdcFaceNeighborhood  fHood;
        SdcStencil<float>    fStencil;

        for (int pFace = 0; pFace < parent.faceCount(); ++pFace) {
            VtrIndex cVert = mFaceChildVertIndex[pFace];
            if (!VtrIndexIsValid(cVert)) continue;

            int fVertCount  = parent.mFaceVertCountsAndOffsets[2*pFace];
            int fVertOffset = parent.mFaceVertCountsAndOffsets[2*pFace + 1];

            fStencil.SetVertexWeights(&mFaceVertWeights[fVertOffset]);

            fHood.SetVertexCount(fVertCount);

            scheme.ComputeFaceVertexStencil(fHood, fStencil);
        }
    }
    if (mChildVertFromEdgeCount) {
        SdcEdgeNeighborhood  eHood;
        SdcStencil<float>    eStencil;

        for (int pEdge = 0; pEdge < parent.edgeCount(); ++pEdge) {
            VtrIndex cVert = mEdgeChildVertIndex[pEdge];
            if (!VtrIndexIsValid(cVert)) continue;

            //
            //  Update the locations for the stencil weights:
            //
            int    eFaceCount   = parent.mEdgeFaceCountsAndOffsets[2*pEdge];
            int    eFaceOffset  = parent.mEdgeFaceCountsAndOffsets[2*pEdge + 1];
            float* eFaceWeights = &mEdgeFaceWeights[eFaceOffset];
            float* eVertWeights = &mEdgeVertWeights[2 * pEdge];

            eStencil.SetVertexWeights(eVertWeights);
            eStencil.SetFaceWeights(eFaceWeights);

            //
            //  Initialize the neighborhood around the parent edge -- we won't have to deal
            //  with child-edge sharpness as the child Rule is known:
            //
            //  NOTE - missing inspection of TriSub option and associated face valences...
            //
            float eSharpness = parent.edgeSharpness(pEdge);

            eHood.SetSharpness(eSharpness);
            eHood.SetFaceCount(eFaceCount);

            //
            //  Identify the parent and child and compute weights -- note that the face
            //  weights may not be populated, so set them to zero if not:
            //
            SdcRule pRule = (eSharpness > 0.0) ? SdcRule::RULE_CREASE : SdcRule::RULE_SMOOTH;
            SdcRule cRule = child.vertRule(cVert);

            scheme.ComputeEdgeVertexStencil(eHood, eStencil, pRule, cRule);

            if (eStencil._faceCount == 0) {
                std::fill(eFaceWeights, eFaceWeights + eFaceCount, 0.0);
            }
        }
    }
    if (mChildVertFromVertCount) {
        SdcVertexNeighborhood  vHood;
        SdcStencil<float>      vStencil;

        for (int pVert = 0; pVert < parent.vertCount(); ++pVert) {
            VtrIndex cVert = mVertChildVertIndex[pVert];
            if (!VtrIndexIsValid(cVert)) continue;

            //
            //  Update the locations for the stencil weights:
            //
            float* vVertWeights = &mVertVertWeights[pVert];

            int    vEdgeCount   = parent.mVertEdgeCountsAndOffsets[2*pVert];
            int    vEdgeOffset  = parent.mVertEdgeCountsAndOffsets[2*pVert + 1];
            float* vEdgeWeights = &mVertEdgeWeights[vEdgeOffset];

            int    vFaceCount   = parent.mVertFaceCountsAndOffsets[2*pVert];
            int    vFaceOffset  = parent.mVertFaceCountsAndOffsets[2*pVert + 1];
            float* vFaceWeights = &mVertFaceWeights[vFaceOffset];

            vStencil.SetVertexWeights(vVertWeights);
            vStencil.SetFaceWeights(vFaceWeights);
            vStencil.SetEdgeWeights(vEdgeWeights);

            //
            //  Initialize the neighborhood around the parent vertex -- inspect the Rules
            //  and gather sharpness values as needed:
            //
            vHood.SetEdgeCount(vEdgeCount);
            vHood.SetFaceCount(vFaceCount);

            float pSharpness[vEdgeCount];
            float cSharpness[vEdgeCount];

            SdcRule pRule = parent.vertRule(pVert);
            SdcRule cRule = child.vertRule(cVert);

            if ((cRule != pRule) || (pRule == SdcCrease::RULE_CREASE)) {
                VtrIndexAccessor vEdges = parent.accessVertEdges(pVert);
                for (int i = 0; i < vEdgeCount; ++i) {
                    pSharpness[i] = parent.edgeSharpness(vEdges[i]);
                }
                if (cRule != pRule) {
                    //  Likely faster to just recompute sharpness locally than gather:
                    SdcCrease(_schemeOptions).SubdivideEdgeSharpnessAroundVertex(vEdgeCount, pSharpness, cSharpness);

                    vHood.SetChildSharpness(child.vertSharpness(cVert), cSharpness);
                }
                vHood.SetSharpness(parent.vertSharpness(pVert), pSharpness);
            }

            scheme.ComputeVertexVertexStencil(vHood, vStencil, pRule, cRule);

            if (vStencil._edgeCount == 0) {
                std::fill(vEdgeWeights, vEdgeWeights + vEdgeCount, 0.0);
            }
            if (vStencil._faceCount == 0) {
                std::fill(vFaceWeights, vFaceWeights + vFaceCount, 0.0);
            }
        }
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
