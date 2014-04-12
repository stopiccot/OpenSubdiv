//  Insert DWA-approved OpenSource copyright notice here...

#ifndef FAR_REFINE_TABLES_H
#define FAR_REFINE_TABLES_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../vtr/level.h"
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
    //  Main refinement method -- we will need some variants here to support
    //  different refinement options:
    //
    void Refine(int subdivLevel, bool fullTopologyInLastLevel = true,
                                 bool computeStencilsPerLevel = false);

    //  Level access for converting to/from base/last levels (ultimately these
    //  need to be protected and the required functionality provided through an
    //  extended public interface)
    VtrLevel& GetBaseLevel();
    VtrLevel& GetLastLevel();

    void Clear();

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
