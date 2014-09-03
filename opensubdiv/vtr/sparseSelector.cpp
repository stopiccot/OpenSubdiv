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
//  Component selection methods:
//      Marking of selection is retained in the SparseTags of the Refinement.  The
//  selection simply marks the parent components -- not any child components that may
//  be derived from them.  That is done later when we need to additionally identify
//  all of the "neighboring" child components that must exist at the next subdivision
//  level in order to fully define supported further refinement of selected components.
//
inline void
VtrSparseSelector::markSelection() {

    if (!_selected) {
        _refine->initializeSparseSelectionTags();
    }
    _selected = true;
}

void
VtrSparseSelector::selectVertex(VtrIndex parentVertex) {

    markSelection();

    //  Don't bother to test-and-set here, just set
    markVertexSelected(parentVertex);
}

void
VtrSparseSelector::selectEdge(VtrIndex parentEdge) {

    markSelection();

    if (!wasEdgeSelected(parentEdge)) {
        //  Mark the two end vertices:
        VtrIndexArray const eVerts = _refine->parent().getEdgeVertices(parentEdge);
        markVertexSelected(eVerts[0]);
        markVertexSelected(eVerts[1]);

        markEdgeSelected(parentEdge);
    }
}

void
VtrSparseSelector::selectFace(VtrIndex parentFace) {

    markSelection();

    if (!wasFaceSelected(parentFace)) {
        //  Mark the face's incident verts and edges as selected:
        VtrIndexArray const fEdges = _refine->parent().getFaceEdges(parentFace);
        VtrIndexArray const fVerts = _refine->parent().getFaceVertices(parentFace);

        for (int i = 0; i < fVerts.size(); ++i) {
            markEdgeSelected(fEdges[i]);
            markVertexSelected(fVerts[i]);
        }
        markFaceSelected(parentFace);
    }
}

void
VtrSparseSelector::selectVertexFaces(VtrIndex parentVertex) {

    markSelection();

    //
    //  Unclear if this will still be useful -- regardless, we can no longer tag the
    //  vertex to distinguish it being selected by incidence of a selected face or
    //  selected explicitly here.
    //
    VtrIndexArray const vertFaces = _refine->parent().getVertexFaces(parentVertex);
    for (int i = 0; i < vertFaces.size(); ++i) {
        if (wasFaceSelected(vertFaces[i])) continue;

        selectFace(vertFaces[i]);
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
