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
#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../sdc/crease.h"
#include "../vtr/level.h"
#include "../far/refineTables.h"
#include "../far/refineTablesFactory.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Methods for the Factory base class -- general enough to warrant including in
//  the base class rather than the subclass template (and so replicated for each
//  usage)
//
//
void
FarRefineTablesFactoryBase::validateComponentTopologySizing(FarRefineTables& refTables)
{
    VtrLevel& baseLevel = refTables.GetBaseLevel();

    int vCount = baseLevel.vertCount();
    int eCount = baseLevel.edgeCount();
    int fCount = baseLevel.faceCount();

    assert((vCount > 0) && (eCount > 0) && (fCount > 0));

    //
    //  This still needs a little work -- currently we are assuming all counts and offsets
    //  have been assigned, but eventually only the counts will be assigined (in arbitrary
    //  order) and we will need to accumulate the offsets to get the total sizes.  That
    //  will require new methods on VtrLevel -- we do not want direct member access here.
    //
    baseLevel.resizeFaceVerts(baseLevel.faceVertCount(fCount-1) + baseLevel.faceVertOffset(fCount-1));
    baseLevel.resizeFaceEdges(baseLevel.faceVertCount());
    baseLevel.resizeEdgeVerts();
    baseLevel.resizeEdgeFaces(baseLevel.edgeFaceCount(eCount-1) + baseLevel.edgeFaceOffset(eCount-1));
    baseLevel.resizeVertFaces(baseLevel.vertFaceCount(vCount-1) + baseLevel.vertFaceOffset(vCount-1));
    baseLevel.resizeVertEdges(baseLevel.vertEdgeCount(vCount-1) + baseLevel.vertEdgeOffset(vCount-1));

    assert(baseLevel.faceVertCount() > 0);
    assert(baseLevel.faceEdgeCount() > 0);
    assert(baseLevel.edgeVertCount() > 0);
    assert(baseLevel.edgeFaceCount() > 0);
    assert(baseLevel.vertFaceCount() > 0);
    assert(baseLevel.vertEdgeCount() > 0);
}

void
FarRefineTablesFactoryBase::applyBoundariesToSharpness(FarRefineTables& refTables)
{
    VtrLevel&  baseLevel = refTables.GetBaseLevel();
    SdcOptions options   = refTables.GetSchemeOptions();

    //
    //  Apply (infinite) sharpness based on assigned boundary interpolation options
    //
    //  Note/question:
    //      - it is assumed internally that boundary edges are *always* creased to simplify
    //  the logic of dealing with them (i.e. avoiding having to test both the sharpness and
    //  topology to determine a crease/boundary rule -- just test the sharpness)
    //      ? when is a vertex on a boundary NOT treated like a crease, i.e. a B-spline?
    //          - is the EDGE_ONLY option really redundant?
    //      - so this differs from Hbr in its implementation, but yields the same limit
    //      ? does it matter that edges are sharpened as a side effect?
    //      - if so, we need to crease boundary edges prior to calling SdcCrease methods
    //
    //bool sharpenEdges = (options.GetVVarBoundaryInterpolation() != SdcOptions::VVAR_BOUNDARY_NONE);
    bool sharpenEdges = true;
    if (sharpenEdges) {
        for (int i = 0; i < baseLevel.edgeCount(); ++i) {
            if (baseLevel.mEdgeFaceCountsAndOffsets[i*2 + 0] != 2) {
                baseLevel.mEdgeSharpness[i] = SdcCrease::INFINITE;
            }
        }
    }

    bool sharpenVerts = (options.GetVVarBoundaryInterpolation() == SdcOptions::VVAR_BOUNDARY_EDGE_AND_CORNER);
    if (sharpenVerts) {
        for (int i = 0; i < baseLevel.vertCount(); ++i) {
            if (baseLevel.mVertFaceCountsAndOffsets[i*2 + 0] == 1) {
                baseLevel.mVertSharpness[i] = SdcCrease::INFINITE;
            }
        }
    }
}

void
FarRefineTablesFactoryBase::computeBaseVertexRules(FarRefineTables& refTables)
{
    SdcCrease creasing(_schemeOptions);

    VtrLevel& baseLevel = refTables.GetBaseLevel();

    float const* levelEdgeSharpness = &baseLevel.mEdgeSharpness[0];
    float const* levelVertSharpness = &baseLevel.mVertSharpness[0];

    baseLevel.mVertRule.resize(baseLevel.vertCount());
    for (VtrIndex vIndex = 0; vIndex < baseLevel.vertCount(); ++vIndex) {
        //
        //  Gather incident edge sharpness for the Rule query:
        //
        VtrIndexArray const vEdges = baseLevel.accessVertEdges(vIndex);

        float incEdgeSharpness[vEdges.size()];
        for (int i = 0; i < vEdges.size(); ++i) {
            incEdgeSharpness[i] = levelEdgeSharpness[vEdges[i]];
        }
        baseLevel.mVertRule[vIndex] = creasing.DetermineVertexVertexRule(
                        levelVertSharpness[vIndex], vEdges.size(), incEdgeSharpness);

    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
