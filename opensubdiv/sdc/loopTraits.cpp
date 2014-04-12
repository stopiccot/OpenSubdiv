// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/type.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Specializations for SdcTypeTraits<LOOP>:
//
template <>
SdcSplit
SdcTypeTraits<LOOP>::TopologicalSplitType()
{
    return SPLIT_TO_TRIS;
}

template <>
int
SdcTypeTraits<LOOP>::LocalNeighborhoodSize()
{
    return 1;
}

template <>
int
SdcTypeTraits<LOOP>::RegularVertexValence()
{
    return 6;
}

template <>
int
SdcTypeTraits<LOOP>::RegularFaceValence()
{
    return 3;
}

template <>
char const*
SdcTypeTraits<LOOP>::Label()
{
    //  Might need to declare static here to keep all compilers happy...
    return "loop";
};

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
