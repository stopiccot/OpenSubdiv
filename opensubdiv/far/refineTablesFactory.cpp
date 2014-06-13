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
FarRefineTablesFactoryBase::validateComponentTopologySizing(FarRefineTables& refTables) {

    VtrLevel& baseLevel = refTables.getBaseLevel();

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
FarRefineTablesFactoryBase::validateComponentTopologyAssignment(FarRefineTables& refTables) {

    VtrLevel& baseLevel = refTables.getBaseLevel();

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
FarRefineTablesFactoryBase::applyComponentTagsAndBoundarySharpness(FarRefineTables& refTables) {

    VtrLevel&  baseLevel = refTables.getBaseLevel();

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
        //  Take inventory of properties of incident edges that affect this vertex:
        //
        int infSharpEdgeCount    = 0;
        int semiSharpEdgeCount   = 0;
        int nonManifoldEdgeCount = 0;
        for (int i = 0; i < vEdges.size(); ++i) {
            VtrLevel::ETag const& eTag = baseLevel.mEdgeTags[vEdges[i]];

            infSharpEdgeCount    += eTag._infSharp;
            semiSharpEdgeCount   += eTag._semiSharp;
            nonManifoldEdgeCount += eTag._nonManifold;
        }
        int sharpEdgeCount = infSharpEdgeCount + semiSharpEdgeCount;

        //
        //  Sharpen the vertex before using it in conjunction with incident edge
        //  properties to determine the semi-sharp tag and rule:
        //
        bool isCorner = (vFaces.size() == 1) && (vEdges.size() == 2);
        if (isCorner && sharpenCornerVerts) {
            vSharpness = SdcCrease::INFINITE;
        } else if (vTag._nonManifold && sharpenNonManFeatures) {
            //  Don't sharpen the vertex if a non-manifold crease:
            if (nonManifoldEdgeCount != 2) {
                vSharpness = SdcCrease::INFINITE;
            }
        }

        vTag._infSharp = SdcCrease::IsInfinite(vSharpness);

        vTag._semiSharp = SdcCrease::IsSemiSharp(vSharpness) || (semiSharpEdgeCount > 0);

        vTag._rule = creasing.DetermineVertexVertexRule(vSharpness, sharpEdgeCount);

        //
        //  Assign topological tags -- note that the "xordinary" (or conversely a "regular")
        //  tag is still being considered, but regardless, it depends on the SdcScheme...
        //
        assert(refTables.GetSchemeType()==TYPE_CATMARK);

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

//
// Instantiates FarRefineTables from indexing arrays.
//
FarRefineTables *
FarRefineTablesFactoryBase::Create( SdcType type,
                                    SdcOptions options,
                                    unsigned int numVertices,
                                    unsigned int numFaces,
                                    unsigned int const * vertsPerFace,
                                    unsigned int const * vertIndices) {

    FarRefineTables *refTables = new FarRefineTables(type, options);

    { // resize components
        refTables->setNumBaseVertices(numVertices);
        refTables->setNumBaseFaces(numFaces);

        for (unsigned int face=0; face<numFaces; ++face) {
            refTables->setNumBaseFaceVertices(face, vertsPerFace[face]);
        }
    }

    validateComponentTopologySizing(*refTables);

    { // assign face vertex indices

        for (unsigned int face=0, idx=0; face<numFaces; ++face) {

            FarIndexArray dstFaceVerts = refTables->setBaseFaceVertices(face);

            for (int vert=0; vert<dstFaceVerts.size(); ++vert) {

                dstFaceVerts[vert] = vertIndices[idx++];
            }
        }
    }

    validateComponentTopologyAssignment(*refTables);

    return refTables;
}

//
// Adds edge creases.
//
unsigned int
FarRefineTablesFactoryBase::AddCreases( FarRefineTables & refTables,
                                        unsigned int numEdges,
                                        unsigned int const * vertIndexPairs,
                                        float const * creaseWeights ) {
    int result=0;

    if (refTables.GetMaxLevel()>0) {
        // This function should never be called on topology that has
        // been refined
        return result;
    }

    for (unsigned int edge=0; edge<numEdges; ++edge, vertIndexPairs+=2) {

        FarIndex idx = refTables.FindEdge(0, vertIndexPairs[0], vertIndexPairs[1]);
        
        if (idx!=VTR_INDEX_INVALID) {
           refTables.baseEdgeSharpness(idx) = creaseWeights[edge];
           ++result;
        }
    }
    return result;
}

//
// Adds vertex creases.
//
unsigned int
FarRefineTablesFactoryBase::AddCorners( FarRefineTables & refTables,
                                        unsigned int numVertices,
                                        unsigned int const * vertIndices,
                                        float const * cornerWeights ) {
    int result=0;

    if (refTables.GetMaxLevel()>0) {
        // This function should never be called on topology that has
        // been refined
        return result;
    }

    for (unsigned int vert=0; vert<numVertices; ++vert) {

        int idx = vertIndices[vert];

        if (idx < refTables.GetNumVertices(0)) {
           refTables.baseVertexSharpness(idx) = cornerWeights[vert];
           ++result;
        }
    }
    return result;
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
