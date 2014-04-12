// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_TYPE_H
#define SDC_TYPE_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Enumerated type for all subdivisions schemes supported by OpenSubdiv:
//
//  Questions:
//      In general, scoping of enumeration names is an issue given the lack of nested
//  namespaces.  I gave most other types a qualifying prefix to avoid conflicts, but
//  these names didn't seem to warrant it.  Should we apply a prefix to these or might
//  we end up with an "sdc" namespace, in which case we might avoid prefixes entirely?
//
enum SdcType {
    BILINEAR,
    CATMARK,
    LOOP
};


//
//  Traits associated with all types -- these are specialized and instantiated for
//  each of the supported types.
//
//  Traits do not vary with the topology or any options applied to the scheme.  They
//  are intended to help construct more general queries about a subdivision scheme
//  in a context where its details may be less well understood.  They serve little
//  purpose in code specialized to the particular scheme, i.e. in code already
//  specialized for Catmark, the values for these traits for the Catmark scheme are
//  typically known and their usage well understood.
//
//  Question:
//      Do we really need/want these TypeTraits, or will static methods on another
//  class specialized for the type suffice, i.e. SdcScheme<SCHEME_TYPE>?
//      If yes, there will be little in here other than SdcType, which we may want
//  to merge into <sdc/options.h>.
//
enum SdcSplit {
    SPLIT_TO_QUADS,  // used by Catmark and Bilinear
    SPLIT_TO_TRIS,   // used by Loop
    SPLIT_HYBRID     // not currently used (potential future extension)
};

template <SdcType SCHEME_TYPE>
struct SdcTypeTraits
{
    static SdcType Type() { return SCHEME_TYPE; }

    static SdcSplit    TopologicalSplitType();
    static int         LocalNeighborhoodSize();
    static int         RegularVertexValence();
    static int         RegularFaceValence();
    static char const* Label();
};


} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_TYPE_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
