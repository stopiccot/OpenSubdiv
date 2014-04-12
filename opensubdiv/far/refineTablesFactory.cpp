//  Insert DWA-approved OpenSource copyright notice here...

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../sdc/crease.h"
#include "../sdc/arrayInterface.h"
#include "../vtr/level.h"
#include "../far/refineTables.h"
#include "../far/refineTablesFactory.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

namespace {
    int
    countAndOffsetVectorSize(const std::vector<int>& vector)
    {
        const int* lastCountOffsetPair = &vector[vector.size() - 2];
        return lastCountOffsetPair[0] + lastCountOffsetPair[1];
    }
}

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

    assert(baseLevel.vertCount() > 0);
    assert(baseLevel.edgeCount() > 0);
    assert(baseLevel.faceCount() > 0);

    //
    //  This still needs a little work -- currently we are assuming all counts and offsets
    //  have been assigned, but eventually only the counts will be assigined (in arbitrary
    //  order) and we will need to accumulate the offsets to get the total sizes.  That
    //  will require new methods on VtrLevel -- we do not want direct member access here.
    //
    baseLevel.resizeFaceVerts(countAndOffsetVectorSize(baseLevel.mFaceVertCountsAndOffsets));
    baseLevel.resizeFaceEdges(baseLevel.faceVertCount());
    baseLevel.resizeEdgeVerts();
    baseLevel.resizeEdgeFaces(countAndOffsetVectorSize(baseLevel.mEdgeFaceCountsAndOffsets));
    baseLevel.resizeVertFaces(countAndOffsetVectorSize(baseLevel.mVertFaceCountsAndOffsets));
    baseLevel.resizeVertEdges(countAndOffsetVectorSize(baseLevel.mVertEdgeCountsAndOffsets));

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
    VtrLevel& baseLevel = refTables.GetBaseLevel();

    //
    //  Need to inspect the subd options from the factory member:
    //
    bool markBoundariesBySharpness = true;
    bool markCornersBySharpness = true;

    if (markBoundariesBySharpness) {
        for (int i = 0; i < baseLevel.edgeCount(); ++i) {
            if (baseLevel.mEdgeFaceCountsAndOffsets[i*2 + 0] != 2) {
                baseLevel.mEdgeSharpness[i] = SdcCrease::INFINITE;
            }
        }
    }
    if (markCornersBySharpness) {
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
    SdcCrease creasing;  //  Should be passing _options member here...

    VtrLevel& baseLevel = refTables.GetBaseLevel();

    float const* levelEdgeSharpness = &baseLevel.mEdgeSharpness[0];
    float const* levelVertSharpness = &baseLevel.mVertSharpness[0];

    baseLevel.mVertRule.resize(baseLevel.vertCount());
    for (VtrIndex vIndex = 0; vIndex < baseLevel.vertCount(); ++vIndex) {
        //
        //  Gather incident edge sharpness for the Rule query:
        //
        VtrIndexAccessor vEdges = baseLevel.accessVertEdges(vIndex);

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
