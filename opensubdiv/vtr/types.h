// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef VTR_TYPES_H
#define VTR_TYPES_H

#include "../version.h"

#include "../sdc/arrayInterface.h"

#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  A few types (and constants) are declared here while Vtr is being
//  developed.  These tend to be used by more than one Vtr class, i.e.
//  both VtrLevel and VtrRefinement and are often present in their
//  interfaces.
//
//  Is the sharpness overkill -- perhaps Sdc should define this...
//
typedef float VtrSharpness;

//
//  Indices -- note we can't use sized integer types like uint32_t, etc. as use of
//  stdint is not portable.
//
//  The convention throughout the OpenSubdiv code is to use "int" in most places,
//  with "unsigned int" being limited to a few cases (why?).  So we continue that
//  trend here and use "int" for topological indices (with -1 indicating "invalid")
//  despite the fact that we lose half the range compared to using "uint" (with ~0
//  as invalid).
//
typedef int            VtrIndex;      //  Used to index the vectors of components
typedef unsigned char  VtrLocalIndex; //  Used to index one component within another

static const VtrIndex  VTR_INDEX_INVALID = -1;

inline bool VtrIndexIsValid(VtrIndex index) { return (index != VTR_INDEX_INVALID); }

typedef std::vector<VtrIndex>  VtrIndexVector;

//
//  Unclear what we should eventually call these, but there will be need
//  for both const and non-const versions of the arrays of indices...
//
typedef SdcConstArrayInterface<VtrIndex>  VtrIndexAccessor;
typedef SdcArrayInterface<VtrIndex>       VtrIndexModifier;

typedef SdcConstArrayInterface<VtrLocalIndex>  VtrLocalIndexAccessor;
typedef SdcArrayInterface<VtrLocalIndex>       VtrLocalIndexModifier;


} // end namespace OPENSUBDIV_VERSION

using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* VTR_TYPES_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
