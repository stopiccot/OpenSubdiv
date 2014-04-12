//  Insert DWA-approved OpenSource copyright notice here...

#include "../far/refineTables.h"

#include <cassert>


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Relatively trivial construction/destruction -- the base level (level[0]) needs
//  to be explicitly initialized after construction and refinement then applied
//
FarRefineTables::FarRefineTables() :
    _subdivType(CATMARK),
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
FarRefineTables::Refine(int maxLevel, bool fullTopology, bool computeStencils)
{
    assert(_levels[0].vertCount() > 0);  //  Make sure the base level has been initialized
    assert(_subdivType == CATMARK);

    _isUniform = true;
    _maxLevel = maxLevel;

    //
    //  Allocate the stack of levels and the refinements between them:
    //
    _levels.resize(maxLevel + 1);
    _refinements.resize(maxLevel);

    for (int i = 1; i <= maxLevel; ++i) {
        _refinements[i-1].initialize(_levels[i-1], _levels[i]);
        _refinements[i-1].refine((i < maxLevel) ? true : fullTopology, computeStencils);
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv
