// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/type.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Specializations for SdcTypeTraits<CATMARK>:
//
template <>
SdcSplit
SdcTypeTraits<CATMARK>::TopologicalSplitType()
{
    return SPLIT_TO_QUADS;
}

template <>
int
SdcTypeTraits<CATMARK>::LocalNeighborhoodSize()
{
    return 1;
}

template <>
int
SdcTypeTraits<CATMARK>::RegularVertexValence()
{
    return 4;
}

template <>
int
SdcTypeTraits<CATMARK>::RegularFaceValence()
{
    return 4;
}

template <>
char const*
SdcTypeTraits<CATMARK>::Label()
{
    //  Might need to declare static here to keep all compilers happy...
    return "catmark";
};

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
