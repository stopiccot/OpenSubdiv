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
#include "../sdc/crease.h"
#include "../sdc/catmarkScheme.h"
#include "../sdc/bilinearScheme.h"
#include "../vtr/types.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"
#include "../vtr/maskInterfaces.h"

#include <cassert>
#include <cstdio>


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
    _parent(0),
    _child(0),
    _schemeType(TYPE_CATMARK),
    _schemeOptions(),
    _quadSplit(true),
    _childFaceFromFaceCount(0),
    _childEdgeFromFaceCount(0),
    _childEdgeFromEdgeCount(0),
    _childVertFromFaceCount(0),
    _childVertFromEdgeCount(0),
    _childVertFromVertCount(0)
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
    _parent = &parent;
    _child  = &child;

    child._depth = 1 + parent.depth();
}


//
//  Methods for preparing for refinement:
//
void
VtrRefinement::initializeParentToChildMappings(VtrIndex initValue)
{
    //  What is the history behind this assertion?
    assert(_parent->vertCount() == (int)_parent->mVertRule.size());

    //
    //  Initialize the vectors of indices mapping parent components to those child components
    //  that will originate from each.  The decision to initialize all to be INVALID or not is
    //  determined externally and the initial value provided.
    //
    //  Notes on intial value and marking:
    //      When sparse, we typically want INVALID as the initial value, which will remain for
    //  what is expected to be the majority of components not marked.  For non-sparse, i.e.
    //  uniform, there is no point having separate initialization and sequencing operations and
    //  the mapping should be allocated without an initial value and explicitly initialized
    //  without any per-component conditions.  So this will eventually only serve sparse
    //  refinement.
    //
    //  More on marking for sparse refinement:
    //      It's unclear how best to do this, i.e. how to do it most efficiently in general.  We
    //  can first identify parent faces, then all incident vertices will need to have a child
    //  face for all incident parent faces.  Once all of the child faces have been identified,
    //  all associated parent components and child components can be identified and marked.  It
    //  is possible to mark multiple component types as we go, but we may be better off only
    //  writing to one at a time (efficiency in general but also threading potential).
    //
    if (_quadSplit) {
        _faceChildFaceIndices.resize(_parent->mFaceVertIndices.size(), initValue);
        _faceChildEdgeIndices.resize(_parent->mFaceEdgeIndices.size(), initValue);
        _edgeChildEdgeIndices.resize(_parent->mEdgeVertIndices.size(), initValue);

        _faceChildVertIndex.resize(_parent->faceCount(), initValue);
        _edgeChildVertIndex.resize(_parent->edgeCount(), initValue);
        _vertChildVertIndex.resize(_parent->vertCount(), initValue);
    } else {
        assert("Non-quad splitting not yet supported\n" == 0);

        _faceChildFaceIndices.resize(_parent->faceCount() * 4,         initValue);
        _faceChildEdgeIndices.resize(_parent->mFaceEdgeIndices.size(), initValue);
        _edgeChildEdgeIndices.resize(_parent->mEdgeVertIndices.size(), initValue);

        _faceChildVertIndex.resize(0,                    initValue);
        _edgeChildVertIndex.resize(_parent->edgeCount(), initValue);
        _vertChildVertIndex.resize(_parent->vertCount(), initValue);
    }
}
void
VtrRefinement::printParentToChildMappings() const
{
    printf("Parent-to-child component mapping:\n");
    for (VtrIndex pFace = 0; pFace < _parent->faceCount(); ++pFace) {
        printf("  Face %d:\n", pFace);
        printf("    Child vert:  %d\n", _faceChildVertIndex[pFace]);

        printf("    Child faces: ");
        VtrIndexArray const childFaces = faceChildFaces(pFace);
        for (int i = 0; i < childFaces.size(); ++i) {
            printf(" %d", childFaces[i]);
        }
        printf("\n");

        printf("    Child edges: ");
        VtrIndexArray const childEdges = faceChildEdges(pFace);
        for (int i = 0; i < childEdges.size(); ++i) {
            printf(" %d", childEdges[i]);
        }
        printf("\n");
    }
    for (VtrIndex pEdge = 0; pEdge < _parent->edgeCount(); ++pEdge) {
        printf("  Edge %d:\n", pEdge);
        printf("    Child vert:  %d\n", _edgeChildVertIndex[pEdge]);

        VtrIndexArray const childEdges = edgeChildEdges(pEdge);
        printf("    Child edges: %d %d\n", childEdges[0], childEdges[1]);
    }
    for (VtrIndex pVert = 0; pVert < _parent->vertCount(); ++pVert) {
        printf("  Vert %d:\n", pVert);
        printf("    Child vert:  %d\n", _vertChildVertIndex[pVert]);
    }
}


namespace {
    bool isSparseIndexMarked(VtrIndex index)   { return index != 0; }

    int
    sequenceSparseIndexVector(VtrIndexVector& indexVector, int baseValue = 0)
    {
        int validCount = 0;
        for (int i = 0; i < (int) indexVector.size(); ++i) {
            indexVector[i] = isSparseIndexMarked(indexVector[i])
                           ? (baseValue + validCount++)
                           : VTR_INDEX_INVALID;
        }
        return validCount;
    }
    int
    sequenceFullIndexVector(VtrIndexVector& indexVector, int baseValue = 0)
    {
        int indexCount = (int) indexVector.size();
        for (int i = 0; i < indexCount; ++i) {
            indexVector[i] = baseValue++;
        }
        return indexCount;
    }
}

void
VtrRefinement::identifySparseChildComponents()
{
    //  At minimum, initializing the parent-to-child mapping should result in child
    //  vertices from parent vertices...
    assert(_vertChildVertIndex.size());

    //
    //  Assign the sequences of child components by inspecting what was marked:
    //
    //  Note that for vertices (and edges) the sequence is assembled from three source vectors
    //  for vertices originating from faces, edges and vertices:
    //
    //  child faces:
    _childFaceFromFaceCount = sequenceSparseIndexVector(_faceChildFaceIndices);

    //  child edges:
    _childEdgeFromFaceCount = sequenceSparseIndexVector(_faceChildEdgeIndices);
    _childEdgeFromEdgeCount = sequenceSparseIndexVector(_edgeChildEdgeIndices, _childEdgeFromFaceCount);

    //  child vertices:
    _childVertFromFaceCount = sequenceSparseIndexVector(_faceChildVertIndex);
    _childVertFromEdgeCount = sequenceSparseIndexVector(_edgeChildVertIndex, _childVertFromFaceCount);
    _childVertFromVertCount = sequenceSparseIndexVector(_vertChildVertIndex, _childVertFromFaceCount +
                                                                             _childVertFromEdgeCount);
}

void
VtrRefinement::identifyUniformChildComponents()
{
    //
    //  We should be able to replace this separate initialization and sequencing in one
    //  iteration -- we just need to separate the allocation and initialization that are
    //  currently combined in the following initialization method:
    //
    initializeParentToChildMappings(0);

    //  child faces:
    _childFaceFromFaceCount = sequenceFullIndexVector(_faceChildFaceIndices);

    //  child edges:
    _childEdgeFromFaceCount = sequenceFullIndexVector(_faceChildEdgeIndices);
    _childEdgeFromEdgeCount = sequenceFullIndexVector(_edgeChildEdgeIndices, _childEdgeFromFaceCount);

    //  child vertices:
    _childVertFromFaceCount = sequenceFullIndexVector(_faceChildVertIndex);
    _childVertFromEdgeCount = sequenceFullIndexVector(_edgeChildVertIndex, _childVertFromFaceCount);
    _childVertFromVertCount = sequenceFullIndexVector(_vertChildVertIndex, _childVertFromFaceCount +
                                                                           _childVertFromEdgeCount);
}

void
VtrRefinement::assignChildComponentInventory()
{
    //
    //  Assign the child's component counts/inventory based on the child components identified:
    //
    _child->_faceCount = _childFaceFromFaceCount;
    _child->_edgeCount = _childEdgeFromFaceCount + _childEdgeFromEdgeCount;
    _child->_vertCount = _childVertFromFaceCount + _childVertFromEdgeCount + _childVertFromVertCount;
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
VtrRefinement::initializeChildFaceVertexCountsAndOffsets()
{
    VtrLevel& child = *_child;

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
VtrRefinement::initializeChildVertexFaceCountsAndOffsets()
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
VtrRefinement::initializeChildVertexEdgeCountsAndOffsets()
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
VtrRefinement::populateChildFaceVerticesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    //
    //  Algorithm:
    //    - iterate through parent face-child-face vector (could use back-vector)
    //    - use parent components incident the parent face:
    //        - use the interior face-vert, corner vert-vert and two edge-verts
    //
    VtrIndexArray const pFaceVerts = parent.accessFaceVerts(pFace);
    VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexArray const pFaceChildren = this->faceChildFaces(pFace);

    int pFaceVertCount  = pFaceVerts.size();
    for (int j = 0; j < pFaceVertCount; ++j) {
        VtrIndex cFace = pFaceChildren[j];
        if (VtrIndexIsValid(cFace)) {
            VtrIndexArray cFaceVerts = child.modifyFaceVerts(cFace);

            int jPrev = j ? (j - 1) : (pFaceVertCount - 1);

            //  Note ORIENTATION dependency -- regular vs extra-ordinary...
            cFaceVerts[0] = this->_faceChildVertIndex[pFace];
            cFaceVerts[1] = this->_edgeChildVertIndex[pFaceEdges[jPrev]];
            cFaceVerts[2] = this->_vertChildVertIndex[pFaceVerts[j]];
            cFaceVerts[3] = this->_edgeChildVertIndex[pFaceEdges[j]];
        }
    }
}

inline void
VtrRefinement::populateChildFaceEdgesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    //
    //  Algorithm:
    //    - iterate through parent face-child-face vector (could use back-vector)
    //    - use parent components incident the parent face:
    //        - use the two interior face-edges and the two boundary edge-edges
    //
    VtrIndexArray const pFaceVerts = parent.accessFaceVerts(pFace);
    VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexArray const pFaceChildFaces = this->faceChildFaces(pFace);
    VtrIndexArray const pFaceChildEdges = this->faceChildEdges(pFace);

    int pFaceValence = pFaceVerts.size();

    for (int j = 0; j < pFaceValence; ++j) {
        VtrIndex cFace = pFaceChildFaces[j];
        if (VtrIndexIsValid(cFace)) {
            VtrIndexArray cFaceEdges = child.modifyFaceEdges(cFace);

            int jPrev = j ? (j - 1) : (pFaceValence - 1);

            //  Note ORIENTATION dependency -- regular vs extra-ordinary...

            //  First, the two child-edges of the parent face:
            cFaceEdges[0] = pFaceChildEdges[jPrev];
            cFaceEdges[3] = pFaceChildEdges[j];

            VtrIndex pCornerVert = pFaceVerts[j];

            VtrIndex            pPrevEdge      = pFaceEdges[jPrev];
            VtrIndexArray const pPrevEdgeVerts = parent.accessEdgeVerts(pPrevEdge);

            VtrIndex            pNextEdge      = pFaceEdges[j];
            VtrIndexArray const pNextEdgeVerts = parent.accessEdgeVerts(pNextEdge);

            int cornerInPrevEdge = (pPrevEdgeVerts[0] != pCornerVert);
            int cornerInNextEdge = (pNextEdgeVerts[0] != pCornerVert);

            cFaceEdges[1] = this->edgeChildEdges(pPrevEdge)[cornerInPrevEdge];
            cFaceEdges[2] = this->edgeChildEdges(pNextEdge)[cornerInNextEdge];
        }
    }
}

//
//  Edge-vert topology propogation -- two functions for face or edge origin:
//
inline void
VtrRefinement::populateChildEdgeVerticesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    //
    //  For each parent face's edge-children:
    //    - identify parent face's vert-child (note it is shared by all)
    //    - identify parent edge perpendicular to face's child edge:
    //        - identify parent edge's vert-child
    //
    VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFace);

    VtrIndexArray const pFaceChildren = this->faceChildEdges(pFace);

    for (int j = 0; j < pFaceEdges.size(); ++j) {
        VtrIndex cEdge = pFaceChildren[j];
        if (VtrIndexIsValid(cEdge)) {
            VtrIndexArray cEdgeVerts = child.modifyEdgeVerts(cEdge);

            cEdgeVerts[0] = this->_faceChildVertIndex[pFace];
            cEdgeVerts[1] = this->_edgeChildVertIndex[pFaceEdges[j]];
        }
    }
}

inline void
VtrRefinement::populateChildEdgeVerticesFromParentEdge(VtrIndex pEdge)
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    //
    //  For each parent edge's edge-children:
    //    - identify parent edge's vert-child (potentially shared by both)
    //    - identify parent vert at end of child edge:
    //        - identify parent vert's vert-child
    //
    VtrIndexArray const pEdgeVerts = parent.accessEdgeVerts(pEdge);

    VtrIndexArray const pEdgeChildren = this->edgeChildEdges(pEdge);

    //  May want to unroll this trivial loop of 2...
    for (int j = 0; j < 2; ++j) {
        VtrIndex cEdge = pEdgeChildren[j];
        if (VtrIndexIsValid(cEdge)) {
            VtrIndexArray cEdgeVerts = child.modifyEdgeVerts(cEdge);

            cEdgeVerts[0] = this->_edgeChildVertIndex[pEdge];
            cEdgeVerts[1] = this->_vertChildVertIndex[pEdgeVerts[j]];
        }
    }
}

//
//  Edge-face topology propogation -- two functions for face or edge origin:
//
inline void
VtrRefinement::populateChildEdgeFacesFromParentFace(VtrIndex pFace)
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    VtrIndexArray const pFaceChildFaces = this->faceChildFaces(pFace);
    VtrIndexArray const pFaceChildEdges = this->faceChildEdges(pFace);

    int pFaceValence = parent.accessFaceVerts(pFace).size();

    for (int j = 0; j < pFaceValence; ++j) {
        VtrIndex cEdge = pFaceChildEdges[j];
        if (VtrIndexIsValid(cEdge)) {
            //
            //  Reserve enough edge-faces, populate and trim as needed:
            //
            child.resizeEdgeFaces(cEdge, 2);

            VtrIndexArray cEdgeFaces = child.modifyEdgeFaces(cEdge);

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
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    VtrIndexArray const pEdgeVerts = parent.accessEdgeVerts(pEdge);
    VtrIndexArray const pEdgeFaces = parent.accessEdgeFaces(pEdge);

    VtrIndexArray const pEdgeChildEdges = this->edgeChildEdges(pEdge);

    for (int j = 0; j < 2; ++j) {
        VtrIndex cEdge = pEdgeChildEdges[j];
        if (!VtrIndexIsValid(cEdge)) continue;

        //
        //  Reserve enough edge-faces, populate and trim as needed:
        //
        child.resizeEdgeFaces(cEdge, pEdgeFaces.size());

        VtrIndexArray cEdgeFaces = child.modifyEdgeFaces(cEdge);

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

            VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFace);
            VtrIndexArray const pFaceVerts = parent.accessFaceVerts(pFace);

            VtrIndexArray const pFaceChildren = this->faceChildFaces(pFace);

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
    for (int fIndex = 0; fIndex < _parent->faceCount(); ++fIndex) {
        populateChildFaceEdgesFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateFaceVerticesFromParentFaces()
{
    for (int fIndex = 0; fIndex < _parent->faceCount(); ++fIndex) {
        populateChildFaceVerticesFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateEdgeVerticesFromParentFaces()
{
    for (int fIndex = 0; fIndex < _parent->faceCount(); ++fIndex) {
        populateChildEdgeVerticesFromParentFace(fIndex);
    }
}
void
VtrRefinement::populateEdgeVerticesFromParentEdges()
{
    for (int eIndex = 0; eIndex < _parent->edgeCount(); ++eIndex) {
        populateChildEdgeVerticesFromParentEdge(eIndex);
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
    for (int fIndex = 0; fIndex < _parent->faceCount(); ++fIndex) {
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
    for (int eIndex = 0; eIndex < _parent->edgeCount(); ++eIndex) {
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
VtrRefinement::populateVertexFacesFromParentFaces()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int fIndex = 0; fIndex < parent.faceCount(); ++fIndex) {
        int cVertIndex = this->_faceChildVertIndex[fIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent face first:
        //
        int pFaceVertCount  = parent.accessFaceVerts(fIndex).size();

        VtrIndexArray const pFaceChildren = this->faceChildFaces(fIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, pFaceVertCount);

        VtrIndexArray      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexArray cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int j = 0; j < pFaceVertCount; ++j) {
            if (VtrIndexIsValid(pFaceChildren[j])) {
                cVertFaces[cVertFaceCount] = pFaceChildren[j];

                //  Note ORIENTATION dependency -- this will eventually need to vary
                //  with the valence of the face to be consistent with hier edits...
                //
                // cVertInFace[j] = (pFaceVertCount == 4) ? j : 0;
                cVertInFace[cVertFaceCount] = 0;
                cVertFaceCount++;
            }
        }
        child.trimVertFaces(cVertIndex, cVertFaceCount);
    }
}
void
VtrRefinement::populateVertexFacesFromParentEdges()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int pEdgeIndex = 0; pEdgeIndex < parent.edgeCount(); ++pEdgeIndex) {
        int cVertIndex = this->_edgeChildVertIndex[pEdgeIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent edge first:
        //
        VtrIndexArray const pEdgeFaces = parent.accessEdgeFaces(pEdgeIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, 2 * pEdgeFaces.size());

        VtrIndexArray      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexArray cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int i = 0; i < pEdgeFaces.size(); ++i) {
            //
            //  identify the parent edge within this parent face -- this is where
            //  augmenting the edge-face relation with the "child index" is useful:
            //
            VtrIndex pFaceIndex  = pEdgeFaces[i];

            VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFaceIndex);

            VtrIndexArray const pFaceChildren = this->faceChildFaces(pFaceIndex);

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
VtrRefinement::populateVertexFacesFromParentVertices()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int vIndex = 0; vIndex < parent.vertCount(); ++vIndex) {
        int cVertIndex = this->_vertChildVertIndex[vIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent vert's faces:
        //
        VtrIndexArray const      pVertFaces  = parent.accessVertFaces(vIndex);
        VtrLocalIndexArray const pVertInFace = parent.accessVertFaceLocalIndices(vIndex);

        //
        //  Reserve enough vert-faces, populate and trim to the actual size:
        //
        child.resizeVertFaces(cVertIndex, pVertFaces.size());

        VtrIndexArray      cVertFaces  = child.modifyVertFaces(cVertIndex);
        VtrLocalIndexArray cVertInFace = child.modifyVertFaceLocalIndices(cVertIndex);

        int cVertFaceCount = 0;
        for (int i = 0; i < pVertFaces.size(); ++i) {
            VtrIndex      pFaceIndex  = pVertFaces[i];
            VtrLocalIndex pFaceVert = pVertInFace[i];

            //  Note ORIENTATION dependency -- this will eventually need to vary
            //  with the valence of the face to be consistent with hier edits...
            int pFaceCount = parent.accessFaceVerts(pFaceIndex).size();

            VtrIndex pFaceChildIndex = this->faceChildFaces(pFaceIndex)[pFaceVert];
            if (VtrIndexIsValid(pFaceChildIndex)) {
                cVertFaces[cVertFaceCount] = pFaceChildIndex;
                cVertInFace[cVertFaceCount] = (VtrLocalIndex)((pFaceCount > 4) ? pFaceVert : 2);
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
VtrRefinement::populateVertexEdgesFromParentFaces()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int fIndex = 0; fIndex < parent.faceCount(); ++fIndex) {
        int cVertIndex = this->_faceChildVertIndex[fIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent face first:
        //
        VtrIndexArray const pFaceVerts = parent.accessFaceVerts(fIndex);

        VtrIndexArray const pFaceChildren = this->faceChildEdges(fIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pFaceVerts.size());

        VtrIndexArray      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexArray cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

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
VtrRefinement::populateVertexEdgesFromParentEdges()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int eIndex = 0; eIndex < parent.edgeCount(); ++eIndex) {
        int cVertIndex = this->_edgeChildVertIndex[eIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  First inspect the parent edge -- its parent faces then its child edges:
        //
        VtrIndexArray const pEdgeFaces = parent.accessEdgeFaces(eIndex);

        VtrIndexArray const pEdgeChild = this->edgeChildEdges(eIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pEdgeFaces.size() + 2);

        VtrIndexArray      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexArray cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

        //
        //  For each face incident the parent edge -- assign the edge within
        //  that parent face (perpendicular to the parent edge) if valid:
        //
        int cVertEdgeCount = 0;

        for (int i = 0; i < pEdgeFaces.size(); ++i) {
            VtrIndex pFace = pEdgeFaces[i];

            VtrIndexArray const pFaceEdges = parent.accessFaceEdges(pFace);

            VtrIndexArray const pFaceChildEdges = this->faceChildEdges(pFace);

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
VtrRefinement::populateVertexEdgesFromParentVertices()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (int vIndex = 0; vIndex < parent.vertCount(); ++vIndex) {
        int cVertIndex = this->_vertChildVertIndex[vIndex];
        if (!VtrIndexIsValid(cVertIndex)) continue;

        //
        //  Inspect the parent vert's edges first:
        //
        VtrIndexArray const      pVertEdges  = parent.accessVertEdges(vIndex);
        VtrLocalIndexArray const pVertInEdge = parent.accessVertEdgeLocalIndices(vIndex);

        //
        //  Reserve enough vert-edges, populate and trim to the actual size:
        //
        child.resizeVertEdges(cVertIndex, pVertEdges.size());

        VtrIndexArray      cVertEdges  = child.modifyVertEdges(cVertIndex);
        VtrLocalIndexArray cVertInEdge = child.modifyVertEdgeLocalIndices(cVertIndex);

        int cVertEdgeCount = 0;
        for (int i = 0; i < pVertEdges.size(); ++i) {
            VtrIndex      pEdgeIndex  = pVertEdges[i];
            VtrLocalIndex pEdgeVert = pVertInEdge[i];

            VtrIndex pEdgeChildIndex = this->edgeChildEdges(pEdgeIndex)[pEdgeVert];
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
//    ? compute mask weights for all child verts
//        - should mirror the parents topological relations
//    * interpolate -- now deferred externally
//
/*
void
VtrRefinement::refine(bool sparse, bool includeTopology, bool computeMasks)
{
    Options refineOptions;

    refineOptions._sparse           = sparse;
    refineOptions._faceTopologyOnly = !includeTopology;
    refineOptions._computeMasks     = computeMasks;

    refine(refineOptions);
}
*/

void
VtrRefinement::refine(Options refineOptions)
{
    assert(_parent && _child);

    //
    //  There's room for experimentation and improvement here -- if uniformly
    //  subdividing we can "mark" and "create" more efficiently in one pass.
    //
    if (refineOptions._sparse) {
        identifySparseChildComponents();
    } else {
        identifyUniformChildComponents();
    }
    assignChildComponentInventory();

    //
    //  What should we do when there is nothing marked in a sparse refinement?  Just return...?
    //
    if (refineOptions._sparse && (_childVertFromVertCount == 0)) {
        printf("Warning in VtrRefinement::Refine() -- no selected components detected...\n");
        printf("    Reverting to uniform for now but need to consider alternate behavior.\n");
        identifyUniformChildComponents();
        assignChildComponentInventory();
    }

    //
    //  Allocate/intialize any desired tags now as they may be most efficiently populated during
    //  other methods (though post-refinement methods to populate currently exist):
    //
    if (refineOptions._parentTagging) {
        //  Note that selection for spares refinement may already have initialized some...
        initializeParentTags();
    }
    if (refineOptions._childTagging) {
        initializeChildTags();
    }

    //
    //  We can often suppress full topology generation in the last level -- the parent topology
    //  and subdivided sharpness values are enough to be able to interpolate vertex data.  How
    //  to best identify and specify what aspects should be refined is still unclear.
    //
    if (refineOptions._faceTopologyOnly) {
       //  This is a subset of subdivideTopology() for the child's face-verts:
       initializeChildFaceVertexCountsAndOffsets();
       _child->mFaceVertIndices.resize(_child->faceCount() * 4);
       populateFaceVerticesFromParentFaces();
    } else {
        subdivideTopology();
    }

    //
    //  Subdividing sharpness values and classifying child vertices -- note the issue relating
    //  the subdivision of edge-sharpness and classification of the vertex of a parent edge:  a
    //  case can be made for classifying while computing sharpness values.
    //
    subdivideVertexSharpness();
    subdivideEdgeSharpness();

    subdivideVertexRule();

    //
    //  Unclear at present if we will be updating parent and/or child tags as we refine, or as
    //  a post-process via inspection of the results -- for now this is the post-process:
    //
    if (refineOptions._parentTagging) {
        populateParentTags();
    }
    if (refineOptions._childTagging) {
        populateChildTags();
    }

    //
    //  This is intended to be temporary(!):
    //      Optionally compute the mask weights -- this is preliminary and needs to deal with
    //  the different schemes appropriately.  Assume Catmark for now.  Note also that computing
    //  masks for Bilinear is trivial, but the application of them may end up far more costly
    //  than necessary without specializing the application of them.
    //
    if (refineOptions._computeMasks) {
        computeMaskWeights();
    }
}


//
//  The main method to subdivide the topology -- requires sizing and initialization of
//  child topology vectors and then appropriate assignment of them.
//
void
VtrRefinement::subdivideTopology()
{
    const VtrLevel& parent = *_parent;
          VtrLevel& child  = *_child;

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
    initializeChildFaceVertexCountsAndOffsets();

    //  These are currently empty stubs -- these are assembled incrementally...
    initializeChildEdgeFaceCountsAndOffsets();
    initializeChildVertexFaceCountsAndOffsets();
    initializeChildVertexEdgeCountsAndOffsets();


    //
    //  Face relations -- both face-verts and face-edges share the same counts/offsets:
    //
    //  Face-verts -- allocate and populate:
    child.mFaceVertIndices.resize(child.faceCount() * 4);

    populateFaceVerticesFromParentFaces();

    //  Face-edges -- allocate and populate:
    child.mFaceEdgeIndices.resize(child.faceCount() * 4);

    populateFaceEdgesFromParentFaces();

    //
    //  Edge relations -- edge-verts and edge-faces can be populated independently:
    //
    //  Edge-verts -- allocate and populate:
    child.mEdgeVertIndices.resize(child.edgeCount() * 2);

    populateEdgeVerticesFromParentFaces();
    populateEdgeVerticesFromParentEdges();

    //  Edge-faces:
    //      NOTE we do not know the exact counts/offsets here because of the
    //  potentially sparse subdivision.  If we must compute this incrementally,
    //  i.e. as we populate the edge-face indices, we will not be able to thread
    //  that operation.  See populateEdgeFaceCountsAndOffsets() for possibilities
    //  of pre-computing these.
    //
    int childEdgeFaceIndexSizeEstimate = (int)parent.mFaceVertIndices.size() * 2 +
                                         (int)parent.mEdgeFaceIndices.size() * 2;

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
    int childVertFaceIndexSizeEstimate = (int)parent.mFaceVertIndices.size()
                                        + (int)parent.mEdgeFaceIndices.size() * 2
                                        + (int)parent.mVertFaceIndices.size();

    child.mVertFaceCountsAndOffsets.resize(child.vertCount() * 2);
    child.mVertFaceIndices.resize(         childVertFaceIndexSizeEstimate);
    child.mVertFaceLocalIndices.resize(    childVertFaceIndexSizeEstimate);

    populateVertexFacesFromParentFaces();
    populateVertexFacesFromParentEdges();
    populateVertexFacesFromParentVertices();

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
    int childVertEdgeIndexSizeEstimate = (int)parent.mFaceVertIndices.size()
                                       + (int)parent.mEdgeFaceIndices.size() + parent.edgeCount() * 2
                                       + (int)parent.mVertEdgeIndices.size();

    child.mVertEdgeCountsAndOffsets.resize(child.vertCount() * 2);
    child.mVertEdgeIndices.resize(         childVertEdgeIndexSizeEstimate);
    child.mVertEdgeLocalIndices.resize(    childVertEdgeIndexSizeEstimate);

    populateVertexEdgesFromParentFaces();
    populateVertexEdgesFromParentEdges();
    populateVertexEdgesFromParentVertices();

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
VtrRefinement::subdivideVertexSharpness()
{
    SdcCrease creasing(_schemeOptions);

    _child->mVertSharpness.clear();
    _child->mVertSharpness.resize(_child->vertCount(), SdcCrease::SMOOTH);

    //
    //  All child-verts originating from faces or edges are initialized as smooth
    //  above.  Only those originating from vertices require "subdivided" values:
    //
    for (VtrIndex pVert = 0; pVert < _parent->vertCount(); ++pVert) {
        VtrIndex cVert = this->_vertChildVertIndex[pVert];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrSharpness const& pSharpness = _parent->mVertSharpness[pVert];
        VtrSharpness&       cSharpness = _child->mVertSharpness[cVert];

        cSharpness = creasing.SubdivideVertexSharpness(pSharpness);
    }
}

void
VtrRefinement::subdivideEdgeSharpness()
{
    SdcCrease creasing(_schemeOptions);

    _child->mEdgeSharpness.clear();
    _child->mEdgeSharpness.resize(_child->edgeCount(), SdcCrease::SMOOTH);

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
    for (VtrIndex pEdge = 0; pEdge < _parent->edgeCount(); ++pEdge) {
        VtrIndex cVert = this->_edgeChildVertIndex[pEdge];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrIndexArray const cEdges = this->edgeChildEdges(pEdge);

        VtrSharpness pSharpness = _parent->mEdgeSharpness[pEdge];
        if (pSharpness <= SdcCrease::SMOOTH) {
            if (VtrIndexIsValid(cEdges[0])) _child->mEdgeSharpness[cEdges[0]] = SdcCrease::SMOOTH;
            if (VtrIndexIsValid(cEdges[1])) _child->mEdgeSharpness[cEdges[1]] = SdcCrease::SMOOTH;
        } else if (pSharpness >= SdcCrease::INFINITE) {
            if (VtrIndexIsValid(cEdges[0])) _child->mEdgeSharpness[cEdges[0]] = SdcCrease::INFINITE;
            if (VtrIndexIsValid(cEdges[1])) _child->mEdgeSharpness[cEdges[1]] = SdcCrease::INFINITE;
        } else if (creasing.IsUniform()) {
            VtrSharpness cSharpness = std::max<float>(pSharpness - 1.0f, SdcCrease::SMOOTH);
            if (VtrIndexIsValid(cEdges[0])) _child->mEdgeSharpness[cEdges[0]] = cSharpness;
            if (VtrIndexIsValid(cEdges[1])) _child->mEdgeSharpness[cEdges[1]] = cSharpness;
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
            VtrIndexArray const pEdgeVerts = _parent->accessEdgeVerts(pEdge);

            for (int i = 0; i < 2; ++i) {
                if (VtrIndexIsValid(cEdges[i])) {
                    VtrIndexArray const pVertEdges = _parent->accessVertEdges(pEdgeVerts[i]);

                    float pVertEdgeSharpness[pVertEdges.size() + 1];
                    for (int j = 0; j < pVertEdges.size(); ++j) {
                        pVertEdgeSharpness[j] = _parent->mEdgeSharpness[pVertEdges[j]];
                    }

                    _child->mEdgeSharpness[cEdges[i]] = creasing.SubdivideEdgeSharpnessAtVertex(
                            pSharpness, pVertEdges.size(), pVertEdgeSharpness);
                }
            }
        }
    }
}


//
//  Methods to classify the type of child vertices for later mask evaluation:
//
void
VtrRefinement::classifyChildVerticesFromParentFaces()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (VtrIndex pFace = 0; pFace < parent.faceCount(); ++pFace) {
        VtrIndex cVert = this->_faceChildVertIndex[pFace];
        if (!VtrIndexIsValid(cVert)) continue;

        child.mVertRule[cVert] = SdcCrease::RULE_SMOOTH;
    }
}
void
VtrRefinement::classifyChildVerticesFromParentEdges()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (VtrIndex pEdge = 0; pEdge < parent.edgeCount(); ++pEdge) {
        VtrIndex cVert = this->_edgeChildVertIndex[pEdge];
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
        VtrIndexArray const cEdges = this->edgeChildEdges(pEdge);

        child.mVertRule[cVert] = SdcCrease::RULE_SMOOTH;
        if (VtrIndexIsValid(cEdges[0]) && (child.mEdgeSharpness[cEdges[0]] > SdcCrease::SMOOTH)) {
            child.mVertRule[cVert] = SdcCrease::RULE_CREASE;
        } else if (VtrIndexIsValid(cEdges[1]) && (child.mEdgeSharpness[cEdges[1]] > SdcCrease::SMOOTH)) {
            child.mVertRule[cVert] = SdcCrease::RULE_CREASE;
        }
    }
}
void
VtrRefinement::classifyChildVerticesFromParentVertices()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    for (VtrIndex pVert = 0; pVert < parent.vertCount(); ++pVert) {
        VtrIndex cVert = this->_vertChildVertIndex[pVert];
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
            //
            //  We need to gather the sharpness values for all incident child edges and compute
            //  the rule from these sharpness values.
            //
            //  If the child has local topology, and it matches that of the parent, we can inspect
            //  the child edges directly.  Otherwise, we need the parent topology to identify the
            //  child edges through the parent edges, and we may need to recompute sharpness values
            //  for child edges of the parent that were not generated (due to sparse refinement).
            //
            SdcCrease creasing(_schemeOptions);

            //  Note that the child edges will be empty if child topology was not generated...
            VtrIndexArray const childEdges  = child.accessVertEdges(cVert);
            VtrIndexArray const parentEdges = parent.accessVertEdges(pVert);

            int edgeCount = parentEdges.size();

            float const* sourceChildSharpness = &child.mEdgeSharpness[0];
            float        localChildSharpness[edgeCount + 1];

            if (childEdges.size() == parentEdges.size()) {
                for (int i = 0; i < edgeCount; ++i) {
                    localChildSharpness[i] = sourceChildSharpness[childEdges[i]];
                }
            } else {
                //
                //  Unfortunately we can't tell if child edges were generated for each parent
                //  edge, so we have to test and branch within the loop rather than dealing with
                //  the two cases separately:
                //
                float const* sourceParentSharpness = &parent.mEdgeSharpness[0];

                for (int i = 0; i < edgeCount; ++i) {
                    VtrIndex pEdge = parentEdges[i];

                    int endVertex = (parent.accessEdgeVerts(pEdge)[1] == pVert);
                    VtrIndex cEdge = this->edgeChildEdges(pEdge)[endVertex];

                    if (VtrIndexIsValid(cEdge)) {
                        localChildSharpness[i] = sourceChildSharpness[cEdge];
                    } else if (creasing.IsUniform()) {
                        localChildSharpness[i] = creasing.SubdivideUniformSharpness(
                                sourceParentSharpness[pEdge]);
                    } else {
                        //
                        //  Trying to fill in missing sharpness values individually may not be
                        //  worth the effort for non-uniform creasing methods -- potentially
                        //  more efficient to just recompute them all at once:
                        //
                        float localParentSharpness[edgeCount];
                        for (int j = 0; j < edgeCount; ++j) {
                            localParentSharpness[j] = sourceParentSharpness[pEdge];
                        }
                        creasing.SubdivideEdgeSharpnessesAroundVertex(edgeCount,
                                localParentSharpness, localChildSharpness);
                        break;
                    }
                }
            }
            child.mVertRule[cVert] = creasing.DetermineVertexVertexRule(0.0, edgeCount, localChildSharpness);
        }
    }
}

void
VtrRefinement::subdivideVertexRule()
{
    _child->mVertRule.clear();
    _child->mVertRule.resize(_child->vertCount());

    classifyChildVerticesFromParentFaces();
    classifyChildVerticesFromParentEdges();
    classifyChildVerticesFromParentVertices();
}

void
VtrRefinement::initializeParentTags()
{
    //  We only have need of edge and face tags at present:

    _parentFaceTag.resize(_parent->faceCount());
    _parentEdgeTag.resize(_parent->edgeCount());
    //_parentVertexTag.resize(_parent->vertCount());
}

void
VtrRefinement::initializeChildTags()
{
    //  We only have need of vertex tags at present:

    //_childFaceTag.resize(_child->faceCount());
    //_childEdgeTag.resize(_child->edgeCount());
    _childVertexTag.resize(_child->vertCount());
}

void
VtrRefinement::populateParentTags()
{
    //
    //  Not doing anything here yet -- the "transitional" tag should be populated by now:
    //
    assert(hasParentEdgeTags());
}

void
VtrRefinement::populateChildTags()
{
    //
    //  Populate the "incomplete" tag for vertices -- this is really best determined as
    //  the topology is subdivided.  To do so now is awkward given we haven't set up the
    //  child-to-parent mapping yet, so we iterate through all parent components and
    //  identify and tag their child vertices:
    //
    //  Note:  the current metrics for detecting completeness are not fully general, e.g.
    //  do not work for a non-quad-split or non-manifold cases with bare edges.
    //
    assert(_child->_hasTopology);
    assert(hasChildVertexTags());

    for (VtrIndex pFace = 0; pFace < _parent->faceCount(); ++pFace) {
        VtrIndex cVert = this->_faceChildVertIndex[pFace];
        if (!VtrIndexIsValid(cVert)) continue;

        _childVertexTag[cVert]._incomplete = (_child->mVertFaceCountsAndOffsets[2*cVert] !=
                                              _parent->mFaceVertCountsAndOffsets[2*pFace]);
    }
    for (VtrIndex pEdge = 0; pEdge < _parent->edgeCount(); ++pEdge) {
        VtrIndex cVert = this->_edgeChildVertIndex[pEdge];
        if (!VtrIndexIsValid(cVert)) continue;

        _childVertexTag[cVert]._incomplete = (_child->mVertFaceCountsAndOffsets[2*cVert] !=
                                            2*_parent->mEdgeFaceCountsAndOffsets[2*pEdge]);
    }
    for (VtrIndex pVert = 0; pVert < _parent->vertCount(); ++pVert) {
        VtrIndex cVert = this->_vertChildVertIndex[pVert];
        if (!VtrIndexIsValid(cVert)) continue;

        _childVertexTag[cVert]._incomplete = (_child->mVertFaceCountsAndOffsets[2*cVert] !=
                                              _parent->mVertFaceCountsAndOffsets[2*pVert]);
    }
}

void
VtrRefinement::computeMaskWeights()
{
    const VtrLevel& parent = *this->_parent;
          VtrLevel& child  = *this->_child;

    assert(child.vertCount() != 0);

    _faceVertWeights.resize(parent.faceVertCount());
    _edgeVertWeights.resize(parent.edgeVertCount());
    _edgeFaceWeights.resize(parent.edgeFaceCount());
    _vertVertWeights.resize(parent.vertCount());
    _vertEdgeWeights.resize(parent.vertEdgeCount());
    _vertFaceWeights.resize(parent.vertEdgeCount());

    //
    //  Hard-coding this for Catmark temporarily for testing...
    //
    assert(_schemeType == TYPE_CATMARK);

    SdcScheme<TYPE_CATMARK> scheme(_schemeOptions);

    if (_childVertFromFaceCount) {
        for (int pFace = 0; pFace < parent.faceCount(); ++pFace) {
            VtrIndex cVert = _faceChildVertIndex[pFace];
            if (!VtrIndexIsValid(cVert)) continue;

            int    fVertCount   = parent.mFaceVertCountsAndOffsets[2*pFace];
            int    fVertOffset  = parent.mFaceVertCountsAndOffsets[2*pFace + 1];
            float* fVertWeights = &_faceVertWeights[fVertOffset];

            VtrMaskInterface fMask(fVertWeights, 0, 0);
            VtrFaceInterface fHood(fVertCount);

            scheme.ComputeFaceVertexMask(fHood, fMask);
        }
    }
    if (_childVertFromEdgeCount) {
        VtrEdgeInterface eHood(parent);

        for (int pEdge = 0; pEdge < parent.edgeCount(); ++pEdge) {
            VtrIndex cVert = _edgeChildVertIndex[pEdge];
            if (!VtrIndexIsValid(cVert)) continue;

            //
            //  Update the locations for the mask weights:
            //
            int    eFaceCount   = parent.mEdgeFaceCountsAndOffsets[2*pEdge];
            int    eFaceOffset  = parent.mEdgeFaceCountsAndOffsets[2*pEdge + 1];
            float* eFaceWeights = &_edgeFaceWeights[eFaceOffset];
            float* eVertWeights = &_edgeVertWeights[2 * pEdge];

            VtrMaskInterface eMask(eVertWeights, 0, eFaceWeights);

            //
            //  Identify the parent and child and compute weights -- note that the face
            //  weights may not be populated, so set them to zero if not:
            //
            eHood.SetIndex(pEdge);

            SdcRule pRule = (parent.edgeSharpness(pEdge) > 0.0) ? SdcCrease::RULE_CREASE : SdcCrease::RULE_SMOOTH;
            SdcRule cRule = child.vertRule(cVert);

            scheme.ComputeEdgeVertexMask(eHood, eMask, pRule, cRule);

            if (eMask.GetFaceWeightCount() == 0) {
                std::fill(eFaceWeights, eFaceWeights + eFaceCount, 0.0);
            }
        }
    }
    if (_childVertFromVertCount) {
        VtrVertexInterface vHood(parent, child);

        for (int pVert = 0; pVert < parent.vertCount(); ++pVert) {
            VtrIndex cVert = _vertChildVertIndex[pVert];
            if (!VtrIndexIsValid(cVert)) continue;

            //
            //  Update the locations for the mask weights:
            //
            float* vVertWeights = &_vertVertWeights[pVert];

            int    vEdgeCount   = parent.mVertEdgeCountsAndOffsets[2*pVert];
            int    vEdgeOffset  = parent.mVertEdgeCountsAndOffsets[2*pVert + 1];
            float* vEdgeWeights = &_vertEdgeWeights[vEdgeOffset];

            int    vFaceCount   = parent.mVertFaceCountsAndOffsets[2*pVert];
            int    vFaceOffset  = parent.mVertFaceCountsAndOffsets[2*pVert + 1];
            float* vFaceWeights = &_vertFaceWeights[vFaceOffset];

            VtrMaskInterface vMask(vVertWeights, vEdgeWeights, vFaceWeights);

            //
            //  Initialize the neighborhood and gather the pre-determined Rules:
            //
            vHood.SetIndex(pVert, cVert);

            SdcRule pRule = parent.vertRule(pVert);
            SdcRule cRule = child.vertRule(cVert);

            scheme.ComputeVertexVertexMask(vHood, vMask, pRule, cRule);

            if (vMask.GetEdgeWeightCount() == 0) {
                std::fill(vEdgeWeights, vEdgeWeights + vEdgeCount, 0.0);
            }
            if (vMask.GetFaceWeightCount() == 0) {
                std::fill(vFaceWeights, vFaceWeights + vFaceCount, 0.0);
            }
        }
    }
}


} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
