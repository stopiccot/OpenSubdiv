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
#ifndef FAR_REFINE_TABLES_H
#define FAR_REFINE_TABLES_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../vtr/level.h"
#include "../vtr/sparseSelector.h"
#include "../vtr/refinement.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

template <class MESH> class FarRefineTablesFactory;

//
//  Class to store topology data for a specified set of refinement options.
//
class FarRefineTables
{
public:
    typedef VtrIndex       Index;
    typedef VtrLocalIndex  LocalIndex;

    typedef VtrIndexArray      IndexArray;
    typedef VtrLocalIndexArray LocalIndexArray;

public:
    FarRefineTables(SdcType type, SdcOptions options = SdcOptions());
    ~FarRefineTables();

    //  Accessors:
    SdcType    GetSchemeType() const    { return _subdivType; }
    SdcOptions GetSchemeOptions() const { return _subdivOptions; }

    bool IsUniform() const   { return _isUniform; }
    int  GetMaxLevel() const { return _maxLevel; }

    //
    //  The "counts" return the number of components at a specific level or for the sum
    //  of all levels:
    //
    //  Should cache these results as members for trivial return...
    int GetVertCount() const;
    int GetEdgeCount() const;
    int GetFaceCount() const;

    int GetVertCount(int level) const { return _levels[level].vertCount(); }
    int GetEdgeCount(int level) const { return _levels[level].edgeCount(); }
    int GetFaceCount(int level) const { return _levels[level].faceCount(); }

    //
    //  Main refinement method(s) -- we will need some variants here to support
    //  different refinement options, i.e. eventually struct RefineOptions:
    //
    void RefineUniform(int maxLevel, bool fullTopologyInLastLevel = true,
                                     bool computeMasksPerLevel = false);
    void RefineAdaptive(int maxLevel, bool fullTopologyInLastLevel = true,
                                      bool computeMasksPerLevel = false);

    void Unrefine();  // Clear all but base level
    void Clear();

    //  Level access for converting to/from base/last levels (ultimately these
    //  need to be protected and the required functionality provided through an
    //  extended public interface)
    VtrLevel& GetBaseLevel() { return _levels.front(); }
    VtrLevel& GetLastLevel() { return _levels.back(); }

protected:
    //
    //  For use by the Factory base and subclasses to construct the base level:
    //
    template <class MESH> friend class FarRefineTablesFactory;

    //  Sizing specifications required before allocation:
    void setBaseFaceCount(  int count) { _levels[0].resizeFaces(count); }
    void setBaseEdgeCount(  int count) { _levels[0].resizeEdges(count); }
    void setBaseVertexCount(int count) { _levels[0].resizeVerts(count); }

    void setBaseFaceVertexCount(Index f, int count) { _levels[0].resizeFaceVerts(f, count); }
    void setBaseEdgeFaceCount(  Index e, int count) { _levels[0].resizeEdgeFaces(e, count); }
    void setBaseVertexFaceCount(Index v, int count) { _levels[0].resizeVertFaces(v, count); }
    void setBaseVertexEdgeCount(Index v, int count) { _levels[0].resizeVertEdges(v, count); }

    //  Access to populate the base level topology after allocation:
    int        getBaseFaceCount() const  { return GetFaceCount(0); }
    IndexArray baseFaceVertices(Index f) { return _levels[0].modifyFaceVerts(f); }
    IndexArray baseFaceEdges(   Index f) { return _levels[0].modifyFaceEdges(f); }

    int        getBaseEdgeCount() const  { return GetEdgeCount(0); }
    IndexArray baseEdgeVertices(Index e) { return _levels[0].modifyEdgeVerts(e); }
    IndexArray baseEdgeFaces(   Index e) { return _levels[0].modifyEdgeFaces(e); }

    int        getBaseVertexCount() const { return GetVertCount(0); }
    IndexArray baseVertexFaces(Index v)   { return _levels[0].modifyVertFaces(v); }
    IndexArray baseVertexEdges(Index v)   { return _levels[0].modifyVertEdges(v); }

    //  Not sure yet if we will determine these internally...
    LocalIndexArray baseVertexFaceLocalIndices(Index v) { return _levels[0].modifyVertFaceLocalIndices(v); }
    LocalIndexArray baseVertexEdgeLocalIndices(Index v) { return _levels[0].modifyVertEdgeLocalIndices(v); }

    //  Optionally available to get/set sharpness values:
    float& baseEdgeSharpness(Index e)   { return _levels[0].edgeSharpness(e); }
    float& baseVertexSharpness(Index v) { return _levels[0].vertSharpness(v); }


private:
    //  Prototype -- mainly for illustrative purposes right now...
    void catmarkFeatureAdaptiveSelector(VtrSparseSelector& selector);

//  The following should be private but leaving it open while still early...
public:
    SdcType    _subdivType;
    SdcOptions _subdivOptions;

    bool _isUniform;
    int  _maxLevel;

    std::vector<VtrLevel>      _levels;
    std::vector<VtrRefinement> _refinements;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* FAR_REFINE_TABLES_H */
