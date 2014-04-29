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

//
//  Class to store topology data for a specified set of refinement options.
//
class FarRefineTables
{
public:
    FarRefineTables();
    ~FarRefineTables();

    //  Accessors:
    bool IsUniform() const;
    int  GetMaxLevel() const;

    //
    //  The "counts" return the number of elements at a specific level
    //  or the sum of all levels:
    //
    int GetVertCount() const;
    int GetVertCount(int level) const;

    int GetFaceCount() const;
    int GetFaceCount(int level) const;

    int GetEdgeCount() const;
    int GetEdgeCount(int level) const;

    //
    //  Main refinement method(s) -- we will need some variants here to support
    //  different refinement options:
    //
    void RefineUniform(int maxLevel, bool fullTopologyInLastLevel = true,
                                     bool computeStencilsPerLevel = false);
    void RefineAdaptive(int maxLevel, bool fullTopologyInLastLevel = true,
                                      bool computeStencilsPerLevel = false);

    //  Level access for converting to/from base/last levels (ultimately these
    //  need to be protected and the required functionality provided through an
    //  extended public interface)
    VtrLevel& GetBaseLevel();
    VtrLevel& GetLastLevel();

    void Clear();

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
