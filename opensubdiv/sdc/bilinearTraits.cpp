// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/type.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Specializations for SdcTypeTraits<BILINEAR>:
//
template <>
SdcSplit
SdcTypeTraits<BILINEAR>::TopologicalSplitType()
{
    return SPLIT_TO_QUADS;
}

template <>
int
SdcTypeTraits<BILINEAR>::LocalNeighborhoodSize()
{
    return 0;
}

template <>
int
SdcTypeTraits<BILINEAR>::RegularVertexValence()
{
    return 0;
}

template <>
int
SdcTypeTraits<BILINEAR>::RegularFaceValence()
{
    return 0;
}

template <>
char const*
SdcTypeTraits<BILINEAR>::Label()
{
    //  Might need to declare static here to keep all compilers happy...
    return "bilinear";
};

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
