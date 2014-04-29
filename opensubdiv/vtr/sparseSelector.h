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
#ifndef VTR_SPARSE_SELECTOR_H
#define VTR_SPARSE_SELECTOR_H

#include "../version.h"

#include "../vtr/types.h"
#include "../vtr/refinement.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class VtrRefinement;

//
//  VtrSparseSelector:
//      This is experimental at present -- just keeing all of the functionality related to sparse
//  refinment out of VtrRefinement for now until it matures.
//
//  Since it is expected this will be protected or integrated elsewhere into another Vtr class --
//  which will be similarly protected -- all methods intentionally begin with lower case.
//
class VtrSparseSelector
{
public:
    VtrSparseSelector(VtrRefinement& refine) : _refine(&refine), _prevRefine(0), _selected(false) { }
    ~VtrSparseSelector() { }

    //
    //  A previous refinement may be used to indicate whether components are fully defined or
    //  not -- note since optional is is specified/returned by pointer rather than reference
    //  (could make these both ptr for consistency...)
    //
    void setRefinement(VtrRefinement& refine) { _refine = &refine; }
    void setPreviousRefinement(VtrRefinement const* refine) { _prevRefine = refine; }

    VtrRefinement&       getRefinement() const         { return *_refine; }
    VtrRefinement const* getPreviousRefinement() const { return _prevRefine; }

    //
    //  Methods for bracketing the selection process -- unclear if these will be needed or
    //  if they will be more implicit:
    //
    void beginSelection(bool tagParentComponents);
    void endSelection();

    bool isSelectionEmpty() const { return !_selected; }

    bool isVertexIncomplete(VtrIndex parentVertex) const {
        //  A parent of this refinement was child of the previous refinement:
        return _prevRefine && _prevRefine->isChildVertexIncomplete(parentVertex);
    }

    //
    //  Methods for selecting (and marking) components for refinement.  All component indices
    //  refer to components in the parent:
    //
    void selectVertex(VtrIndex parentVertex);
    void selectEdge(  VtrIndex parentEdge);
    void selectFace(  VtrIndex parentFace);

    //  Mark all incident faces of a vertex -- common in feature-adaptive to warrant inclusion
    void selectVertexFaces(VtrIndex parentVertex);

    //
    //  Selection of specific child components may prove useful, but doing so significanly
    //  complicates the marking of neighboring components.  An implementation was completed
    //  and tested, but has been disabled until it is proved to be needed.
    //
    void selectChildEdge(VtrIndex parentEdge, int childEdge);
    void selectChildFace(VtrIndex parentFace, int childFace);

private:
    VtrSparseSelector() { }
    bool isVertexFullySelected(VtrIndex parentEdge) const;
    bool isEdgeFullySelected(VtrIndex parentEdge) const;
    bool isFaceFullySelected(VtrIndex parentEdge) const;
    void markChildFace(VtrIndex parentFace, int childFace, int markingMask);
    void markSparseNeighboringComponents();

    //  Two different approaches being explored -- traversing faces and edges is simple and
    //  effective but only if child components are not specified, while traversing vertices
    //  is relatively simple but can result in a lot of overlapping effort.
    void markSparseNeighboringComponentsByVertex();
    void markSparseNeighboringComponentsByEdgeAndFace();

private:
    VtrRefinement*       _refine;
    VtrRefinement const* _prevRefine;
    bool _selected;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* VTR_SPARSE_SELECTOR_H */
