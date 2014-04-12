// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/scheme.h"
#include "../sdc/faceNeighborhood.h"
#include "../sdc/edgeNeighborhood.h"
#include "../sdc/vertexNeighborhood.h"
#include "../sdc/stencil.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Unclear yet how we will be specializing these wrt SdcStencil<REAL>...
//
namespace {
    template <typename REAL>
    void
    assignFaceVertexStencil(SdcStencil<REAL>& stencil, int valence)
    {
        REAL vWeight = 1.0 / valence;

        for (int i = 0; i < valence; ++i) {
            stencil._vertWeights[i] = vWeight;
        }
        stencil._vertCount = valence;
        stencil._edgeCount = 0;
        stencil._faceCount = 0;
    }
    template <typename REAL>
    void
    assignEdgeVertexStencil(SdcStencil<REAL>& stencil)
    {
        stencil._vertWeights[0] = 0.5;
        stencil._vertWeights[1] = 0.5;

        stencil._vertCount = 2;
        stencil._edgeCount = 0;
        stencil._faceCount = 0;
    }
    template <typename REAL>
    void
    assignVertexVertexStencil(SdcStencil<REAL>& stencil)
    {
        stencil._vertWeights[0] = 1.0;

        stencil._vertCount = 1;
        stencil._edgeCount = 0;
        stencil._faceCount = 0;
    }

} // namespace anon


//
//  Current specializations:
//
template <>
void
SdcScheme<BILINEAR>::ComputeFaceVertexStencil(SdcFaceNeighborhood const& neighborhood,
                                              SdcStencil<float>& stencil) const
{
    assignFaceVertexStencil(stencil, neighborhood._vertCount);
}

template <>
void
SdcScheme<BILINEAR>::ComputeEdgeVertexStencil(SdcEdgeNeighborhood const&,
                                              SdcStencil<float>& stencil,
                                              SdcCrease::Rule, SdcCrease::Rule) const
{
    assignEdgeVertexStencil(stencil);
}

template <>
void
SdcScheme<BILINEAR>::ComputeVertexVertexStencil(SdcVertexNeighborhood const&,
                                                SdcStencil<float>& stencil,
                                                SdcCrease::Rule, SdcCrease::Rule) const
{
    assignVertexVertexStencil(stencil);
}

//  Is this needed for some compilers?
template class SdcScheme<BILINEAR>;

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
