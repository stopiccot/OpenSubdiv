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

    assert((vCount > 0) && (fCount > 0));

    //
    //  This still needs a little work -- currently we are assuming all counts and offsets
    //  have been assigned, but eventually only the counts will be assigined (in arbitrary
    //  order) and we will need to accumulate the offsets to get the total sizes.  That
    //  will require new methods on VtrLevel -- we do not want direct member access here.
    //
    int fVertCount = 0;
    for (int i = 0; i < fCount; ++i) {
        fVertCount += baseLevel.faceVertCount(i);
    }
    baseLevel.resizeFaceVerts(fVertCount);
    assert(baseLevel.faceVertCount() > 0);

    if (eCount > 0) {
        baseLevel.resizeFaceEdges(baseLevel.faceVertCount());
        baseLevel.resizeEdgeVerts();
        baseLevel.resizeEdgeFaces(baseLevel.edgeFaceCount(eCount-1) + baseLevel.edgeFaceOffset(eCount-1));
        baseLevel.resizeVertFaces(baseLevel.vertFaceCount(vCount-1) + baseLevel.vertFaceOffset(vCount-1));
        baseLevel.resizeVertEdges(baseLevel.vertEdgeCount(vCount-1) + baseLevel.vertEdgeOffset(vCount-1));

        assert(baseLevel.faceEdgeCount() > 0);
        assert(baseLevel.edgeVertCount() > 0);
        assert(baseLevel.edgeFaceCount() > 0);
        assert(baseLevel.vertFaceCount() > 0);
        assert(baseLevel.vertEdgeCount() > 0);
    }
}

void
FarRefineTablesFactoryBase::validateComponentTopologyAssignment(FarRefineTables& refTables)
{
    VtrLevel& baseLevel = refTables.GetBaseLevel();

    //
    //  In future we may want the ability to complete aspects of the topology that are incovenient
    //  for clients to specify, e.g. the local indices associated with some relations, orienting
    //  the vertex relations, etc.  For the near term we'll be assuming only face-vertices have
    //  been specified and the absence of edges will trigger the construction of everything else:
    //
    bool completeMissingTopology = (baseLevel.edgeCount() == 0);
    if (completeMissingTopology) {
        //  Need to invoke some VtrLevel method to "fill in" the missing topology...
        baseLevel.completeTopologyFromFaceVertices();
    }

    bool applyValidation = false;
    if (applyValidation) {
        if (!baseLevel.validateTopology()) {
            printf("Invalid topology detected in FarRefineTablesFactory (%s)\n",
                completeMissingTopology ? "partially specified and completed" : "fully specified");
            //baseLevel.print();
            assert(false);
        }
    }
}

//
//  This method combines the initialization of component tags with the sharpening of edges and
//  vertices according to the given boundary interpolation rule in the Options.  Since both
//  involve traversing the edge and vertex lists and noting the presence of boundaries -- best
//  to do both at once...
//
void
FarRefineTablesFactoryBase::applyComponentTagsAndBoundarySharpness(FarRefineTables& refTables)
{
    VtrLevel&  baseLevel = refTables.GetBaseLevel();

    assert((int)baseLevel.mEdgeTags.size() == baseLevel.edgeCount());
    assert((int)baseLevel.mVertTags.size() == baseLevel.vertCount());
    assert((int)baseLevel.mFaceTags.size() == baseLevel.faceCount());

    SdcOptions options = refTables.GetSchemeOptions();
    SdcCrease  creasing(options);

    bool sharpenCornerVerts    = (options.GetVVarBoundaryInterpolation() == SdcOptions::VVAR_BOUNDARY_EDGE_AND_CORNER);
    bool sharpenNonManFeatures = (options.GetNonManifoldInterpolation() == SdcOptions::NON_MANIFOLD_SHARP);

    //
    //  Process the Edge tags first, as Vertex tags (notably the Rule) are dependent on
    //  properties of their incident edges.
    //
    for (VtrIndex eIndex = 0; eIndex < baseLevel.edgeCount(); ++eIndex) {
        VtrLevel::ETag& eTag       = baseLevel.mEdgeTags[eIndex];
        float&          eSharpness = baseLevel.mEdgeSharpness[eIndex];

        eTag._boundary = (baseLevel.mEdgeFaceCountsAndOffsets[eIndex*2 + 0] < 2);
        if (eTag._boundary || (eTag._nonManifold && sharpenNonManFeatures)) {
            eSharpness = SdcCrease::INFINITE;
        }
        eTag._infSharp  = SdcCrease::IsInfinite(eSharpness);
        eTag._semiSharp = SdcCrease::IsSharp(eSharpness) && !eTag._infSharp;
    }

    //
    //  Process the Vertex tags now -- for some tags (semi-sharp and its rule) we need
    //  to inspect all incident edges:
    //
    for (VtrIndex vIndex = 0; vIndex < baseLevel.vertCount(); ++vIndex) {
        VtrLevel::VTag& vTag       = baseLevel.mVertTags[vIndex];
        float&          vSharpness = baseLevel.mVertSharpness[vIndex];

        VtrIndexArray const vEdges = baseLevel.accessVertEdges(vIndex);
        VtrIndexArray const vFaces = baseLevel.accessVertFaces(vIndex);

        //
        //  Sharpen the vertex before determining any tags that depend on it:
        //
        bool isCorner = (vFaces.size() == 1) && (vEdges.size() == 2);
        if ((isCorner && sharpenCornerVerts) || (vTag._nonManifold && sharpenNonManFeatures)) {
            vSharpness = SdcCrease::INFINITE;
        }
        vTag._infSharp = SdcCrease::IsInfinite(vSharpness);

        //
        //  The semi-sharp tag and the rule both depend on incident edge sharpness, but we
        //  don't need the actual edge sharpness values here, just whether sharp or not:
        //
        vTag._semiSharp = 0;
        int sharpEdgeCount = 0;
        for (int i = 0; i < vEdges.size(); ++i) {
            VtrLevel::ETag const& eTag = baseLevel.mEdgeTags[vEdges[i]];

            vTag._semiSharp |= eTag._semiSharp;
            sharpEdgeCount += eTag._semiSharp | eTag._infSharp;
        }
        vTag._semiSharp |= SdcCrease::IsSemiSharp(vSharpness);

        vTag._rule = creasing.DetermineVertexVertexRule(vSharpness, sharpEdgeCount);

        //
        //  Assign topological tags -- note that the "xordinary" (or conversely a "regular")
        //  tag is still being considered, but regardless, it depends on the SdcScheme...
        //
        assert(_schemeType == TYPE_CATMARK);

        vTag._boundary = (vFaces.size() < vEdges.size());
        if (isCorner) {
            vTag._xordinary = !sharpenCornerVerts;
        } else if (vTag._boundary) {
            vTag._xordinary = (vFaces.size() != 2);
        } else {
            vTag._xordinary = (vFaces.size() != 4);
        }
    }

    //
    //  Anything more to be done with Face tags? (eventually when processing edits perhaps)
    //
    //  for (VtrIndex fIndex = 0; fIndex < baseLevel.faceCount(); ++fIndex) {
    //  }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
