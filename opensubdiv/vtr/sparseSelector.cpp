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
#include "../vtr/sparseSelector.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"

#include <cassert>


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Some of these inline utilities may be worth making static methods...
//
namespace {
    //
    //  Currenly use 3 bits in the marking process:
    //      - the main "selected" bit is used to mark components that are actually designated,
    //  i.e. explicitly selected, by the caller
    //      - the "neighboring" bit is used to mark components that neighbor those selected but
    //  not themselves selected.  Typically we "or" this with components around those selected.
    //      - the "full" bit is a recent addition and became necessary when allowing a subset of
    //  a parent's child components to be selected.  This bit is used to indicate that a parent
    //  component has been fully selected, and so to avoid visiting it again (either selecting
    //  it more than once or when detecting the neighboring components).  Note it is redundant
    //  if not accounting for the explicit selection of child components.
    //
    VtrIndex const IndexSparseMaskNeighboring = (1 << 0);
    VtrIndex const IndexSparseMaskSelected    = (1 << 1);
    VtrIndex const IndexSparseMaskFull        = (1 << 2);

    VtrIndex const IndexSparseMaskFullySelected = IndexSparseMaskSelected | IndexSparseMaskFull;

    void markSparseIndexNeighbor(VtrIndex& index)      { index |= IndexSparseMaskNeighboring; }
    void markSparseIndexSelected(VtrIndex& index)      { index |= IndexSparseMaskSelected; }
    void markSparseIndexFullySelected(VtrIndex& index) { index = IndexSparseMaskFullySelected; }
    void markSparseIndex(VtrIndex& index, int mask)    { index |= mask; }

    bool isSparseIndexFullySelected(VtrIndex index) { return index & IndexSparseMaskFull; }
    bool isSparseIndexSelected(VtrIndex index)      { return index & IndexSparseMaskSelected; }
    bool isSparseIndexMarked(VtrIndex index)        { return index != 0; }
}


//
//  Methods involved in selection and marking for sparse refinement:
//      Expected usage is as follows:
//
//          VtrSparseSelector selector(refinement);
//          selector.beginSelection();
//              selector.selectFace(i);
//              selector.selectFace(j);
//              ...
//          selector.endSelection();
//
//          //  To be later followed by:
//          refinement.Refine(asSelectedNotUniform);
//
//  where the "refinement" associated with the Selector can be changed, and potentially
//  will be when creating multiple refinements between multiple levels.
//
//  Regarding begin/end:
//      Its unclear to what degree we want/need the begin/end pair to clearly bracket
//  the selection process.  We do require allocation/initialization of the child mapping,
//  which is specifically NOT done on construction as initialization is dependent on
//  uniform vs sparse refinement.  We could do it lazily in the component selection
//  methods, at an added cost per component.
//      The call to Refine() can potentially serve as the "end".
//
//  Regarding component marking:
//      We have (at least) two possibilities:  mark the surrounding neighborhoods of each
//  component at the time selected, or mark only the descending components and make a
//  separate pass to "guarantee neighbors".  The former will be much more involved per
//  component and the expectation is that it will ultimately be slower.  The latter is
//  also conceptually easier and simpler to code, so we will pursue it first.
//      Once working, we should really compare the two approaches.  The former may end
//  up much better in cases of extreme sparsity and much worse at the opposite end of the
//  spectrum.
//
void
VtrSparseSelector::beginSelection(bool parentTagging)
{
    //  Unclear yet if this is ultimately warranted -- see notes above...

    _refine->initializeParentToChildMappings(0);

    if (parentTagging) {
        _refine->initializeParentTags();
    }
    _selected = false;
}
void
VtrSparseSelector::endSelection()
{
    //  Unclear yet if this is ultimately warranted -- see notes above...

    //  We need to ensure this is not called again in Refine()...
    markSparseNeighboringComponents();
}

//
//  Inline methods for quickly testing if components have already been fully selected in
//  the selection methods that follow:
//    - we mark an edge's child vertex
//    - we mark the first (or all) face's child faces (since Loop has no child vertex)
//  There is no need to test and mark a vertex -- marking it on selection is trivial, but
//  we do for the case when we select all incident faces of a vertex.  So in this case,
//  to avoid visiting all incident faces redundantly, we do mark a vertex fully selected:
//    - we mark the vert's child vertex
//  and it is only tested and set within SelectVertexFaces()
//
inline bool
VtrSparseSelector::isVertexFullySelected(VtrIndex parentVertex) const
{
    return isSparseIndexFullySelected(_refine->vertexChildVertexIndex(parentVertex));
}
inline bool
VtrSparseSelector::isEdgeFullySelected(VtrIndex parentEdge) const
{
    return isSparseIndexFullySelected(_refine->edgeChildVertexIndex(parentEdge));
}
inline bool
VtrSparseSelector::isFaceFullySelected(VtrIndex parentFace) const
{
    //
    //  Remember, using the child-vertex can't be used for a tri-split face -- in which
    //  case mark the child faces...
    //
    return isSparseIndexFullySelected(_refine->faceChildFaces(parentFace)[0]);
}

//
//  Component selection methods:
//      How components are marked when selected is intimately related to how neighboring
//  components will be determined.  If marking neighboring components as part of each
//  component selection, everything must be marked.  But if deferring the identication
//  and marking of neighboring components to a process -- what is marked on selection
//  can be carefully chosenn and minimized, given that the post-process will need to
//  inspect the neighborhoods of all components later.
//
//  So for now, we mark selected components by marking only the vertices that comprise
//  them and their direct child components -- the latter is not necessary when the
//  neighboring post-process uses vertex neighborhoods, but we do want to make the
//  distinction between faces and edges that were selected and those that were included
//  for neighboring support.
//
//  Remember:
//      Whether the refinement is a quad- or tri-split needs to be considered here.
//
void
VtrSparseSelector::selectVertex(VtrIndex parentVertex)
{
    //  No need to test if it was already selected, just mark it again:
    markSparseIndexSelected(_refine->vertexChildVertexIndex(parentVertex));

    _selected = true;
}

void
VtrSparseSelector::selectEdge(VtrIndex parentEdge)
{
    if (isEdgeFullySelected(parentEdge)) return;

    //
    //  Mark the two child edges, the two end vertices and finally the edge vertex:
    //
    VtrIndexArray eChildEdges = _refine->edgeChildEdges(parentEdge);
    markSparseIndexSelected(eChildEdges[0]);
    markSparseIndexSelected(eChildEdges[1]);

    VtrIndexArray const eVerts = _refine->parent().accessEdgeVerts(parentEdge);
    markSparseIndexSelected(_refine->vertexChildVertexIndex(eVerts[0]));
    markSparseIndexSelected(_refine->vertexChildVertexIndex(eVerts[1]));

    //  Remember -- mark child vertex fully selected to avoid revisiting this edge
    markSparseIndexFullySelected(_refine->edgeChildVertexIndex(parentEdge));

    _selected = true;
}

void
VtrSparseSelector::selectFace(VtrIndex parentFace)
{
    assert(_refine->_quadSplit);

    if (isFaceFullySelected(parentFace)) return;

    //
    //  Mark the child faces and all child vertices directly -- calling the public selection
    //  methods for the incident components will result in many components tested and/or
    //  marked multiple times (e.g. note how much simpler this is than markChildFace() being
    //  called four times).
    //
    //VtrIndexArray const fEdges = _refine->parent().accessFaceEdges(parentFace);
    VtrIndexArray const fVerts = _refine->parent().accessFaceVerts(parentFace);

    VtrIndexArray fChildFaces = _refine->faceChildFaces(parentFace);
    VtrIndexArray fChildEdges = _refine->faceChildEdges(parentFace);

    for (int i = 0; i < fVerts.size(); ++i) {
        //markSparseIndexSelected(_refine->edgeChildVertexIndex(fEdges[i]));
        markSparseIndexSelected(_refine->vertexChildVertexIndex(fVerts[i]));
        markSparseIndexSelected(fChildEdges[i]);

        //  Remember -- mark child faces fully selected to avoid revisiting this face
        markSparseIndexFullySelected(fChildFaces[i]);
    }
    markSparseIndexSelected(_refine->faceChildVertexIndex(parentFace));

    _selected = true;
}

void
VtrSparseSelector::selectVertexFaces(VtrIndex parentVertex)
{
    if (isVertexFullySelected(parentVertex)) return;

    VtrIndexArray const vertFaces = _refine->parent().accessVertFaces(parentVertex);
    for (int i = 0; i < vertFaces.size(); ++i) {
        selectFace(vertFaces[i]);
    }

    //  Remember -- mark child vertex fully selected to avoid revisiting this vertex
    markSparseIndexFullySelected(_refine->vertexChildVertexIndex(parentVertex));
}

//
//  Marking of components neighboring those selected:
//      For schemes requiring neighboring support, this is the equivalent of the "guarantee
//  neighbors" in Hbr -- it ensures that all components required to define the limit of
//  those "selected" are also generated in the refinement.
//
//  The difference with Hbr is that we do this in a single pass for all components once
//  "selection" of components of interest has been completed.
//
//  Considering two approaches:
//      1) By Vertex neighborhoods:
//          - for each base vertex
//              - for each incident face
//                  - test and mark components for its child face
//  or
//      2) By Edge and Face contents:
//          - for each base edge
//              - test and mark local components
//          - for each base face
//              - test and mark local components
//
//  Given a typical quad mesh with N verts, N faces and 2*N edges, determine which is more
//  efficient...
//
//  Going with (2) initially for simplicity -- certain aspects of (1) are awkward, i.e. the
//  identification of child-edges to be marked (trivial in (2).  We are also guaranteed with
//  (2) that we only visit each component once, i.e. each edge and each face.
//
//  Revising the above assessment... (2) has gotten WAY more complicated once the ability to
//  select child faces is provided.  Given that feature is important to Manuel for support
//  of the FarStencilTables we have to assume it will be needed.  So we'll try (1) out as it
//  will be simpler to get it correct -- we can work on improving performance later.
//
//  Complexity added by child component selection:
//      - the child vertex of the component can now be selected as part of a child face or
//  edge, and so the parent face or edge is not fully selected.  So we've had to add another
//  bit to the marking masks to indicate when a parent component is "fully selected".
//      - selecting a child face creates the situation where child edges of parent edges do
//  not have any selected vertex at their ends -- both can be neighboring.  This complicated
//  the marking of neighboring child edges, which was otherwise trivial -- if any end vertex
//  of a child edge (of a parent edge) was selected, the child edge was at least neighboring.
//
void
VtrSparseSelector::markSparseNeighboringComponentsByEdgeAndFace()
{
    //
    //  For each parent edge:
    //      Test each end vertex to see if marked and mark both the child edge and the
    //  middle child vertex if so.
    //
    //  Note that no edges have been marked "fully selected" -- only their vertices have
    //  been marked and marking of their child edges deferred to visiting each edge only
    //  once here.
    //
    for (VtrIndex pEdge = 0; pEdge < _refine->parent().edgeCount(); ++pEdge) {
        //  We really don't need this unless majority of edges are explicitly selected:
        //if (isEdgeFullySelected(parentEdge)) continue;

        VtrIndexArray       eChildEdges = _refine->edgeChildEdges(pEdge);
        VtrIndexArray const eVerts      = _refine->parent().accessEdgeVerts(pEdge);

        if (isSparseIndexSelected(_refine->vertexChildVertexIndex(eVerts[0]))) {
            markSparseIndexNeighbor(eChildEdges[0]);
            markSparseIndexNeighbor(_refine->edgeChildVertexIndex(pEdge));
        }
        if (isSparseIndexSelected(_refine->vertexChildVertexIndex(eVerts[1]))) {
            markSparseIndexNeighbor(eChildEdges[1]);
            markSparseIndexNeighbor(_refine->edgeChildVertexIndex(pEdge));
        }

        //
        //  TAG the parent edges as "transitional" here if only one was selected (or in
        //  the more general non-manifold case, they are not all selected the same way).
        //  We use the transitional tags on the edges to TAG the parent face below.
        //
        //  Note -- this is best done now rather than as a post-process as we have more
        //  explicit information about the selected components.  Unless we also tag the
        //  parent faces as selected, we can't easily tell from the child-faces of the
        //  edge's incident faces which were generated by selection or neighboring...
        //
        if (_refine->hasParentEdgeTags()) {
            VtrIndexArray const eFaces = _refine->parent().accessEdgeFaces(pEdge);
            if (eFaces.size() == 2) {
                _refine->_parentEdgeTag[pEdge]._transitional = (isFaceFullySelected(eFaces[0]) !=
                                                                isFaceFullySelected(eFaces[1]));
            } else if (eFaces.size() < 2) {
                _refine->_parentEdgeTag[pEdge]._transitional = false;
            } else {
                bool isFace0Selected = isFaceFullySelected(eFaces[0]);

                _refine->_parentEdgeTag[pEdge]._transitional = false;
                for (int i = 1; i < eFaces.size(); ++i) {
                    if (isFaceFullySelected(eFaces[i]) != isFace0Selected) {
                        _refine->_parentEdgeTag[pEdge]._transitional = true;
                        break;
                    }
                }
            }
        }
    }

    //
    //  For each parent face:
    //      All boundary edges will be adequately marked as a result of the pass over the
    //  edges above and boundary vertices marked by selection.  So all that remains is to
    //  identify the child faces and interior child edges for a face requiring neighboring
    //  child faces.
    //      For each corner vertex selected, we need to mark the corresponding child face,
    //  the two interior child edges and shared child vertex in the middle.
    //
    assert(_refine->_quadSplit);

    for (VtrIndex pFace = 0; pFace < _refine->parent().faceCount(); ++pFace) {
        //
        //  A fully selected face cannot be transitional, and unless there is any other
        //  tagging of faces to be done here, we are finished with this face:
        //
        if (isFaceFullySelected(pFace)) continue;

        VtrIndexArray fChildFaces = _refine->faceChildFaces(pFace);
        VtrIndexArray fChildEdges = _refine->faceChildEdges(pFace);

        int marked = false;

        VtrIndexArray const fVerts = _refine->parent().accessFaceVerts(pFace);
        for (int i = 0; i < fVerts.size(); ++i) {
            //  NOTE - the mod 4 here will not work for N-gons (and want to avoid % anyway)
            int iPrev = (i+3) % 4;

            if (isSparseIndexSelected(_refine->vertexChildVertexIndex(fVerts[i]))) {
                  markSparseIndexNeighbor(fChildFaces[i]);

                  markSparseIndexNeighbor(fChildEdges[i]);
                  markSparseIndexNeighbor(fChildEdges[iPrev]);

                marked = true;
            }
        }
        if (marked) {
            markSparseIndexNeighbor(_refine->faceChildVertexIndex(pFace));
        }

        //
        //  TAG the parent face as "transitional" using the tags on its boundary edges.
        //  We have to inspect all edges (potentially) so retain the state of each in a
        //  4-bit mask that reflects the full transitional topology on later inspection:
        //
        if (_refine->hasParentFaceTags()) {
            //
            //  Remember we have skipped fully-selected faces -- they are not transitional
            //  and so are justly skipped for that tagging purpose, but if anything else
            //  is added here, consider whether it affects selected faces or not...
            //
            VtrIndexArray const fEdges = _refine->parent().accessFaceEdges(pFace);
            if (fEdges.size() == 4) {
                _refine->_parentFaceTag[pFace]._transitional =
                        (_refine->_parentEdgeTag[fEdges[0]]._transitional << 0) |
                        (_refine->_parentEdgeTag[fEdges[1]]._transitional << 1) |
                        (_refine->_parentEdgeTag[fEdges[2]]._transitional << 2) |
                        (_refine->_parentEdgeTag[fEdges[3]]._transitional << 3);
            } else if (fEdges.size() == 3) {
                _refine->_parentFaceTag[pFace]._transitional =
                        (_refine->_parentEdgeTag[fEdges[0]]._transitional << 0) |
                        (_refine->_parentEdgeTag[fEdges[1]]._transitional << 1) |
                        (_refine->_parentEdgeTag[fEdges[2]]._transitional << 2);
            } else {
                _refine->_parentFaceTag[pFace]._transitional = false;
                for (int i = 0; i < fEdges.size(); ++i) {
                    _refine->_parentFaceTag[pFace]._transitional |=
                            _refine->_parentEdgeTag[fEdges[i]]._transitional;
                }
            }
        }
    }
}

void
VtrSparseSelector::markSparseNeighboringComponents()
{
    //
    //  Note we use the edge-and-face traversal for simple parent component selection and the
    //  vertex traversal when child component selection is supported:
    //
    //markSparseNeighboringComponentsByVertex();
    markSparseNeighboringComponentsByEdgeAndFace();
}


//
//  Public and private methods for supporting the selection and marking of child components.
//
//  This greatly complicates the marking of neighboring components and so a separate marking
//  strategy is applied when selection of child components is enabled.
//
void
VtrSparseSelector::selectChildEdge(VtrIndex parentEdge, int childIndex)
{
    if (isEdgeFullySelected(parentEdge)) return;

    //
    //  Mark the child edge, the corresponding end vertex and finally the edge vertex:
    //
    markSparseIndexSelected(_refine->edgeChildEdges(parentEdge)[childIndex]);

    VtrIndexArray const edgeVerts = _refine->parent().accessEdgeVerts(parentEdge);
    markSparseIndexSelected(_refine->vertexChildVertexIndex(edgeVerts[childIndex]));

    markSparseIndexSelected(_refine->edgeChildVertexIndex(parentEdge));

    _selected = true;
}
void
VtrSparseSelector::selectChildFace(VtrIndex parentFace, int childIndex)
{
    assert(_refine->_quadSplit);

    if (isFaceFullySelected(parentFace)) return;

    markChildFace(parentFace, childIndex, IndexSparseMaskSelected);

    _selected = true;
}

void
VtrSparseSelector::markChildFace(VtrIndex parentFace, int childIndex, int mask)
{
    assert("marking child faces disabled!" == 0);

    //
    //  This method is used by the neighboring pass and so includes the marking of all 
    //  incident components, rathar than a reduced set for selection.
    //
    VtrIndexArray const faceVerts = _refine->parent().accessFaceVerts(parentFace);
    VtrIndexArray const faceEdges = _refine->parent().accessFaceEdges(parentFace);

    int childPrev = childIndex ? (childIndex - 1) : (faceVerts.size() - 1);

    VtrIndex edgePrev   = faceEdges[childPrev];
    VtrIndex vertCorner = faceVerts[childIndex];
    VtrIndex edgeNext   = faceEdges[childIndex];

    //  Always mark the four corner vertices:
    markSparseIndex(_refine->faceChildVertexIndex(parentFace), mask);
    markSparseIndex(_refine->vertexChildVertexIndex(vertCorner), mask);
    markSparseIndex(_refine->edgeChildVertexIndex(edgePrev), mask);
    markSparseIndex(_refine->edgeChildVertexIndex(edgeNext), mask);

    //  Always mark the specified child face of the face:
    markSparseIndex(_refine->faceChildFaces(parentFace)[childIndex], mask);

    //  Only mark child edges when marking neighbors:
    if (mask & IndexSparseMaskNeighboring) {
        //  Mark the two child edges of the parent edges (be sure to mark the correct child
        //  for each based on edge orientation)
        int edgePrevChild = (_refine->parent().accessEdgeVerts(edgePrev)[1] == vertCorner);
        int edgeNextChild = (_refine->parent().accessEdgeVerts(edgeNext)[1] == vertCorner);

        markSparseIndex(_refine->edgeChildEdges(edgePrev)[edgePrevChild], mask);
        markSparseIndex(_refine->edgeChildEdges(edgeNext)[edgeNextChild], mask);

        //  Mark the two child edges of the parent face, and the child face itself:
        VtrIndexArray fChildEdges = _refine->faceChildEdges(parentFace);

        markSparseIndex(fChildEdges[childPrev], mask);
        markSparseIndex(fChildEdges[childIndex], mask);
    }
}


void
VtrSparseSelector::markSparseNeighboringComponentsByVertex()
{
    assert("marking neighboring components by vertex disabled!" == 0);

    for (VtrIndex pVert = 0; pVert < _refine->parent().vertCount(); ++pVert) {
        VtrIndexArray const      vertFaces       = _refine->parent().accessVertFaces(pVert);
        VtrLocalIndexArray const vertFaceIndices = _refine->parent().accessVertFaceLocalIndices(pVert);

        //
        //  If this vertex was selected, could quickly mark all incident child faces separately,
        //  but the savings seems minimal.  If not selected, test the interior child vertex of the
        //  face next, then the child faces' corners from edges:
        //
        bool vertWasSelected = isSparseIndexSelected(_refine->vertexChildVertexIndex(pVert));

        for (int i = 0; i < vertFaces.size(); ++i) {
            VtrIndex pFace      = vertFaces[i];
            int      vertInFace = vertFaceIndices[i];

            if (vertWasSelected || isSparseIndexSelected(_refine->faceChildVertexIndex(pFace))) {
                markChildFace(pFace, vertInFace, IndexSparseMaskNeighboring);
            } else {
                VtrIndexArray const faceEdges = _refine->parent().accessFaceEdges(pFace);

                VtrIndex pEdgePrev = faceEdges[vertInFace ? (vertInFace - 1) : (faceEdges.size() - 1)];
                VtrIndex pEdgeNext = faceEdges[vertInFace];

                if (isSparseIndexSelected(_refine->edgeChildVertexIndex(pEdgePrev)) ||
                    isSparseIndexSelected(_refine->edgeChildVertexIndex(pEdgeNext))) {
                    markChildFace(pFace, vertInFace, IndexSparseMaskNeighboring);
                }
            }
        }

        //
        //  Note that if newly marked in any way, we must assure the child-verts of all incident
        //  faces are present due to the dependency of vertex-vertices on incident face-centers.
        //  So some of the resulting child vertices may have no incident edges or faces and are
        //  only present to support child vertices on the corners or edges of the parent face:
        //
        bool vertIsNowMarked = isSparseIndexMarked(_refine->vertexChildVertexIndex(pVert));
        if (!vertWasSelected && vertIsNowMarked) {
            for (int i = 0; i < vertFaces.size(); ++i) {
                markSparseIndexNeighbor(_refine->faceChildVertexIndex(vertFaces[i]));
            }
        }
    }
}


} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
