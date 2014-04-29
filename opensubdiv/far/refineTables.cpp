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
#include "../far/refineTables.h"

#include <cassert>


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Relatively trivial construction/destruction -- the base level (level[0]) needs
//  to be explicitly initialized after construction and refinement then applied
//
FarRefineTables::FarRefineTables() :
    _subdivType(TYPE_CATMARK),
    _subdivOptions(),
    _isUniform(true),
    _maxLevel(0),
    _levels(1)
{
}

FarRefineTables::~FarRefineTables()
{
}

void
FarRefineTables::Clear()
{
    _levels.clear();
    _refinements.clear();
}


//
//  Accessor to the refinement options:
//
bool
FarRefineTables::IsUniform() const
{
    return _isUniform;
}

int
FarRefineTables::GetMaxLevel() const
{
    return _maxLevel;
}


//
//  Miscellaneous methods -- may not persist in the long run:
//
VtrLevel&
FarRefineTables::GetBaseLevel()
{
    assert(_levels.size() > 0);  //  Make sure base level is always present

    return _levels.front();
}

VtrLevel&
FarRefineTables::GetLastLevel()
{
    assert(_levels.size() > 0);  //  Make sure base level is always present

    return _levels.back();
}


//
//  Accessors to the topology information:
//
int
FarRefineTables::GetVertCount(int level) const
{
    return _levels[level].vertCount();
}
int
FarRefineTables::GetEdgeCount(int level) const
{
    return _levels[level].edgeCount();
}
int
FarRefineTables::GetFaceCount(int level) const
{
    return _levels[level].faceCount();
}

int
FarRefineTables::GetVertCount() const
{
    int sum = 0;
    for (int i = 0; i < (int)_levels.size(); ++i) {
        sum += _levels[i].vertCount();
    }
    return sum;
}
int
FarRefineTables::GetEdgeCount() const
{
    int sum = 0;
    for (int i = 0; i < (int)_levels.size(); ++i) {
        sum += _levels[i].edgeCount();
    }
    return sum;
}
int
FarRefineTables::GetFaceCount() const
{
    int sum = 0;
    for (int i = 0; i < (int)_levels.size(); ++i) {
        sum += _levels[i].faceCount();
    }
    return sum;
}

//
//  Main refinement method -- allocating and initializing levels and refinements:
//
void
FarRefineTables::RefineUniform(int maxLevel, bool fullTopology, bool computeMasks)
{
    assert(_levels[0].vertCount() > 0);  //  Make sure the base level has been initialized
    assert(_subdivType == TYPE_CATMARK);

    //
    //  Allocate the stack of levels and the refinements between them:
    //
    _isUniform = true;
    _maxLevel = maxLevel;

    _levels.resize(maxLevel + 1);
    _refinements.resize(maxLevel);

    //
    //  Initialize refinement options for Vtr -- adjusting full-topology for the last level:
    //
    VtrRefinement::Options refineOptions;
    refineOptions._sparse       = false;
    refineOptions._computeMasks = computeMasks;

    for (int i = 1; i <= maxLevel; ++i) {
        refineOptions._faceTopologyOnly = fullTopology ? false : (i == maxLevel);

        _refinements[i-1].initialize(_levels[i-1], _levels[i]);
        _refinements[i-1].refine(refineOptions);
    }
}


void
FarRefineTables::RefineAdaptive(int subdivLevel, bool fullTopology, bool computeMasks)
{
    assert(_levels[0].vertCount() > 0);  //  Make sure the base level has been initialized
    assert(_subdivType == TYPE_CATMARK);

    //
    //  Allocate the stack of levels and the refinements between them:
    //
    _isUniform = false;
    _maxLevel = subdivLevel;

    //  Should we presize all or grow one at a time as needed?
    _levels.resize(subdivLevel + 1);
    _refinements.resize(subdivLevel);

    //
    //  Initialize refinement options for Vtr:
    //
    //  Enabling both parent and child tagging for now
    bool parentTagging = true;
    bool childTagging  = true;

    VtrRefinement::Options refineOptions;
    refineOptions._sparse           = true;
    refineOptions._faceTopologyOnly = !fullTopology;
    refineOptions._computeMasks     = computeMasks;
    refineOptions._parentTagging    = parentTagging;
    refineOptions._childTagging     = childTagging;

    for (int i = 1; i <= subdivLevel; ++i) {
        //  Keeping full topology on for debugging -- may need to go back a level and "prune"
        //  its topology if we don't use the full depth
        refineOptions._faceTopologyOnly = false;

        VtrLevel& parentLevel     = _levels[i-1];
        VtrLevel& childLevel      = _levels[i];
        VtrRefinement& refinement = _refinements[i-1];

        refinement.initialize(parentLevel, childLevel);

        //
        //  Initialize a Selector to mark a sparse set of components for refinement.  The
        //  previous refinement may include tags on its child components that are relevant,
        //  which is why the Selector identifies it.
        //
        //  It's ebatable whether our begin/end should be moved into the feature adaptive code
        //  that uses the Selector -- or the use of the Selector entirely for that matter...
        //
        VtrSparseSelector selector(refinement);
        selector.setPreviousRefinement((i-1) ? &_refinements[i-2] : 0);

        selector.beginSelection(parentTagging);

        catmarkFeatureAdaptiveSelector(selector);

        selector.endSelection();

        //
        //  Continue refining if something selected, otherwise terminate refinement and trim
        //  the Level and Refinement vectors to remove the curent refinement and child that
        //  were in progress:
        //
        if (!selector.isSelectionEmpty()) {
            refinement.refine(refineOptions);

            //childLevel.print(&refinement);
            //assert(childLevel.validateTopology());
        } else {
            //  Note that if we support the "full topology at last level" option properly,
            //  we should prune the previous level generated, as it is now the last...
            int maxLevel = i - 1;

            _maxLevel = maxLevel;
            _levels.resize(maxLevel + 1);
            _refinements.resize(maxLevel);
            break;
        }
    }
}


void
FarRefineTables::catmarkFeatureAdaptiveSelector(VtrSparseSelector& selector)
{
    VtrLevel const& level = selector.getRefinement().parent();

    //
    //  For faces, we only need to select irregular faces from level 0 -- which will
    //  generate an extra-ordinary vertex in its interior:
    //
    if (level.depth() == 0) {
        for (VtrIndex face = 0; face < level.faceCount(); ++face) {
            VtrIndexArray const faceVerts = level.accessFaceVerts(face);
            if (faceVerts.size() != 4) {
                selector.selectFace(face);
            }
        }
    }

    //
    //  For vertices, we want to immediatly skip neighboring vertices generated from the
    //  previous level (the percentage will typically be high enough to warrant immediate
    //  culling, as the will include all perimeter vertices).
    //
    //  Sharp vertices are complicated by the corner case -- an infinitely sharp corner is
    //  considered a regular feature and not sharp, but a corner with any other sharpness
    //  will eventually become extraordinary once its sharpness has decayed -- so it is
    //  both sharp and irregular.
    //
    //  For the remaining topological cases, non-manifold vertices should be considered
    //  along with extra-ordinary, and we should be testing a vertex tag for thats (and
    //  maybe the extra-ordinary too)
    //
    for (VtrIndex vert = 0; vert < level.vertCount(); ++vert) {
        if (selector.isVertexIncomplete(vert)) continue;

        bool selectVertex = false;

        float vertSharpness = level.vertSharpness(vert);
        if (vertSharpness > 0.0) {
            selectVertex = (level.accessVertFaces(vert).size() != 1) || (vertSharpness < SdcCrease::INFINITE);
        } else {
            VtrIndexArray const vertFaces = level.accessVertFaces(vert);
            VtrIndexArray const vertEdges = level.accessVertEdges(vert);

            //  Should be non-manifold test -- remaining cases assume manifold...
            if (vertFaces.size() == vertEdges.size()) {
                selectVertex = (vertFaces.size() != 4);
            } else {
                selectVertex = (vertFaces.size() != 2);
            }
        }
        if (selectVertex) {
            selector.selectVertexFaces(vert);
        }
    }

    //
    //  For edges, we only care about sharp edges, so we can immediately skip all smooth.
    //
    //  That leaves us dealing with sharp edges that may in the interior or on a boundary.
    //  A boundary edge is always a (regular) B-spline boundary, unless something at an end
    //  vertex makes it otherwise.  But any end vertex that would make the edge irregular
    //  should already have been detected above.  So I'm pretty sure we can just skip all
    //  boundary edges.
    //
    //  So reject boundaries, but in a way that includes non-manifold edges for selection.
    //
    //  And as for vertices, skip incomplete neighboring vertices from the previous level.
    //
    for (VtrIndex edge = 0; edge < level.edgeCount(); ++edge) {
        if ((level.edgeSharpness(edge) <= 0.0) || (level.accessEdgeFaces(edge).size() < 2)) continue;

        VtrIndexArray const edgeVerts = level.accessEdgeVerts(edge);
        if (!selector.isVertexIncomplete(edgeVerts[0])) {
            selector.selectVertexFaces(edgeVerts[0]);
        }
        if (!selector.isVertexIncomplete(edgeVerts[1])) {
            selector.selectVertexFaces(edgeVerts[1]);
        }
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
