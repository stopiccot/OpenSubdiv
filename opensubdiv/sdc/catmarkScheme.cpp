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
//  Specializations for SdcScheme<CATMARK>:
//

//
//  Computing the stencil for a face-vertex:
//      This is fairly trivial.
//
template <>
void
SdcScheme<CATMARK>::ComputeFaceVertexStencil(SdcFaceNeighborhood const& neighborhood,
                                             SdcStencil<float>&         stencil) const
{
    int   valence = neighborhood._vertCount;
    float vWeight = 1.0 / valence;

    for (int i = 0; i < valence; ++i) {
        stencil._vertWeights[i] = vWeight;
    }
    stencil._vertCount = valence;
    stencil._edgeCount = 0;
    stencil._faceCount = 0;
}

//
//  Computing the stencil for an edge-vertex:
//      This is relatively straight-forward, but is complicated by two issues:
//          - the presence of the "Triangle Subdivision" option
//              - only useful at level 0 and rarely used
//          - the need for child-edge sharpness values to determine the child rule
//              - only present when edge is not smooth or sharp
//              - only required for non-simple creasing methods like Chaikin
//  The parent "rule" in this case is either Smooth or a Crease.  The child rule can
//  additionally have the case of Dart when the parent is a Crease, which we can
//  treat the same as Smooth.
//
namespace {
    template <typename REAL>
    inline void
    assignEdgeVertexStencilAsCrease(SdcEdgeNeighborhood const&, SdcStencil<REAL>& stencil)
    {
        stencil._vertWeights[0] = 0.5;
        stencil._vertWeights[1] = 0.5;

        stencil._vertCount = 2;
        stencil._edgeCount = 0;
        stencil._faceCount = 0;
    }
    template <typename REAL>
    inline void
    assignEdgeVertexStencilAsSmooth(SdcEdgeNeighborhood const& neighborhood, SdcStencil<REAL>& stencil)
    {
        stencil._vertWeights[0] = 0.25;
        stencil._vertWeights[1] = 0.25;

        stencil._vertCount = 2;
        stencil._edgeCount = 0;
        stencil._faceCount = neighborhood.GetFaceCount();

        //  Face-count will be >= 2:
        if (stencil._faceCount == 2) {
            stencil._faceWeights[0] = 0.25;
            stencil._faceWeights[1] = 0.25;
        } else {
            REAL fWeight = 0.5 / stencil._faceCount;
            for (int i = 0; i < stencil._faceCount; ++i) {
                stencil._faceWeights[i] = fWeight;
            }
        }
    }
    template <typename REAL>
    inline void
    assignEdgeVertexStencilAsSmoothWithTris(SdcEdgeNeighborhood const&, SdcStencil<REAL>& stencil,
                                            bool face0IsTriangle, bool face1IsTriangle)
    {
        const REAL CATMARK_SMOOTH_TRI_EDGE_WEIGHT = 0.470;

        REAL f0Weight = face0IsTriangle ? CATMARK_SMOOTH_TRI_EDGE_WEIGHT : 0.25;
        REAL f1Weight = face1IsTriangle ? CATMARK_SMOOTH_TRI_EDGE_WEIGHT : 0.25;

        REAL fWeight = 0.5 * (f0Weight + f1Weight);
        REAL vWeight = 0.5 * (1.0 - 2.0 * fWeight);

        stencil._vertWeights[0] = vWeight;
        stencil._vertWeights[1] = vWeight;

        stencil._faceWeights[0] = fWeight;
        stencil._faceWeights[1] = fWeight;

        stencil._vertCount = 2;
        stencil._edgeCount = 0;
        stencil._faceCount = 2;
    }
}

template <>
void
SdcScheme<CATMARK>::ComputeEdgeVertexStencil(SdcEdgeNeighborhood const& neighborhood,
                                             SdcStencil<float>&         stencil,
                                             SdcCrease::Rule            parentRule,
                                             SdcCrease::Rule            childRule) const
{
    bool useTriangleOption = (_options.GetTriangleSubdivision() != SdcOptions::TRI_SUB_NORMAL);

    //
    //  Determine if parent rule is smooth and quickly deal with the common case (i.e. no
    //  use of the triangle smoothing option).  Note that for the parent, the Rule cases
    //  of Dart or Corner are not possible and so should not be specified.
    //
    bool pIsSmooth = (parentRule == SdcCrease::RULE_SMOOTH);
    if (!pIsSmooth) {
        if (parentRule == SdcCrease::RULE_CREASE) {
            pIsSmooth = false;
        } else {
            pIsSmooth = (neighborhood.GetSharpness() <= 0.0);
        }
    }

    //  Quick compute and exit for the most common Smooth case:
    if (pIsSmooth && !useTriangleOption) {
        assignEdgeVertexStencilAsSmooth(neighborhood, stencil);
        return;
    }

    //
    //  Determine if child rule is a crease and quickly deal with that common case.  Unlike
    //  the parent, a Rule value of Dart is possible for the child but will be effectively
    //  be considered Smooth.
    //
    //  Note also that the parent being Smooth overrides any specification or determination
    //  of the child as a Crease.  If specified as a Smooth-to-Crease transition, there is
    //  no fractional weight from the Smooth parent, so we effectively treat it as Smooth.
    //  Similarly, if the parent is Smooth and the child unspecified, there is no need to
    //  inspect further as the result will be Smooth.
    //
    bool cIsCrease;
    if (pIsSmooth) {
        cIsCrease = false;
    } else if (childRule == SdcCrease::RULE_CREASE) {
        cIsCrease = true;
    } else if (childRule != SdcCrease::RULE_UNKNOWN) {
        cIsCrease = false;
    } else {
        //
        // THIS IS WHERE WE NEED CHILD-EDGE SHARPNESS!
        //     - assuming simple creasing for now...
        //
        cIsCrease = (neighborhood.GetSharpness() > 1.0);
    }

    //  Before dealing with the simple crease case, revert a transitional case to Crease:
    if (!pIsSmooth && !cIsCrease) {
        //
        //  If the child is not a Crease now, we have a Crease-to-Smooth transition.  In
        //  this case the fractional weight should be the edge sharpness, which vanishes to
        //  zero for the child edges (one or both).  But note that it could be > 1.0 with
        //  Chaikin creasing.  In such a case, we clamp the fractional weight to 1, which
        //  effectively makes it a Crease, so override the Smooth determination here:
        //
        cIsCrease = (neighborhood.GetSharpness() > 1.0);
    }

    //  Quick compute and exit for the common simple Crease case:
    if (cIsCrease) {
        assignEdgeVertexStencilAsCrease(neighborhood, stencil);
        return;
    }

    //
    //  We now have the Crease-to-Smooth case and Smooth cases requiring inspection of
    //  incident faces to the the triangular subdivision option.  Compute the appropriate
    //  Smooth stencil and augment it with the transition to Crease as required:
    //
    bool face0IsTri = false;
    bool face1IsTri = false;
    if (useTriangleOption) {
        if (neighborhood.GetFaceCount() != 2) {
            useTriangleOption = false;
        } else {
            //
            //  Need to inspect/gather valence of incident faces here...
            //
            useTriangleOption = face0IsTri || face1IsTri;
        }
    }

    if (useTriangleOption) {
        assignEdgeVertexStencilAsSmoothWithTris(neighborhood, stencil, face0IsTri, face1IsTri);
    } else {
        assignEdgeVertexStencilAsSmooth(neighborhood, stencil);
    }
    if (!pIsSmooth) {
        float alpha = neighborhood.GetSharpness();
        float beta  = 1.0 - alpha;

        float vWeight = beta * stencil._vertWeights[0] + alpha * 0.5;
        float fWeight = beta * stencil._faceWeights[0];

        stencil._vertWeights[0] = vWeight;
        stencil._vertWeights[1] = vWeight;
        for (int i = 0; i < stencil._faceCount; ++i) {
            stencil._faceWeights[i] = fWeight;
        }
    }
}


//
//  Helper functions for the vertex-vertex stencils -- these are the basic formulae
//  for each Rule and are combined in the main stencil query that follows (as it needs
//  to support the fractional transition and blending between Rules):
//
namespace {
    template <typename REAL>
    inline void
    assignVertexVertexStencilAsCorner(SdcVertexNeighborhood const&, SdcStencil<REAL>& stencil)
    {
        stencil._vertWeights[0] = 1.0;

        stencil._vertCount = 1;
        stencil._edgeCount = 0;
        stencil._faceCount = 0;
    }

    template <typename REAL>
    inline void
    assignVertexVertexStencilAsCrease(SdcVertexNeighborhood const& neighborhood, SdcStencil<REAL>& stencil,
                                      float const* edgeSharpness)
    {
        int valence = neighborhood.GetEdgeCount();

        REAL vWeight = 0.75;
        REAL eWeight = 0.125;

        stencil._vertWeights[0] = vWeight;
        if (edgeSharpness != 0) {
            //  Use the sharpness values to identify the crease edges:
            for (int i = 0; i < valence; ++i) {
                stencil._edgeWeights[i] = (edgeSharpness[i] > 0.0) ? eWeight : 0.0;
            }
        } else {
            //  Use the boundary edges (first and last) as the crease edges:
            stencil._edgeWeights[0] = eWeight;
            for (int i = 1; i < (valence - 1); ++i) {
                stencil._edgeWeights[i] = 0.0;
            }
            stencil._edgeWeights[valence-1] = eWeight;
        }

        stencil._vertCount = 1;
        stencil._edgeCount = valence;
        stencil._faceCount = 0;
    }

    template <typename REAL>
    inline void
    assignVertexVertexStencilAsSmooth(SdcVertexNeighborhood const& neighborhood, SdcStencil<REAL>& stencil)
    {
        int valence = neighborhood.GetFaceCount();

        REAL vWeight = (REAL)(valence - 2) / (REAL)valence;
        REAL fWeight = 1.0 / (REAL)(valence * valence);
        REAL eWeight = fWeight;

        stencil._vertWeights[0] = vWeight;
        for (int i = 0; i < valence; ++i) {
            stencil._edgeWeights[i] = eWeight;
            stencil._faceWeights[i] = fWeight;
        }

        stencil._vertCount = 1;
        stencil._edgeCount = valence;
        stencil._faceCount = valence;
    }

    template <typename REAL>
    inline void
    combineVertexVertexStencils(SdcStencil<REAL>& a, REAL alpha, SdcStencil <REAL>const& b, REAL beta)
    {
        a._vertWeights[0] = alpha * a._vertWeights[0] + beta * b._vertWeights[0];

        if (b._edgeCount > 0) {
            if (a._edgeCount > 0) {
                for (int i = 0; i < a._edgeCount; ++i) {
                    a._edgeWeights[i] = alpha * a._edgeWeights[0] + beta * b._edgeWeights[0];
                }
            } else {
                for (int i = 0; i < a._edgeCount; ++i) {
                    a._edgeWeights[i] = beta * b._edgeWeights[0];
                }
                a._edgeCount = b._edgeCount;
            }
        }
        if (b._faceCount > 0) {
            if (a._faceCount > 0) {
                for (int i = 0; i < a._faceCount; ++i) {
                    a._faceWeights[i] = alpha * a._faceWeights[0] + beta * b._faceWeights[0];
                }
            } else {
                for (int i = 0; i < a._faceCount; ++i) {
                    a._faceWeights[i] = beta * b._faceWeights[0];
                }
                a._faceCount = b._faceCount;
            }
        }
    }
}

//
//  Computing the stencil for a vertex-vertex:
//      This is the most involved of the three given the greater variation that can occur
//  at a vertex that is not derived from the middle of a face or edge.
//
//  Remember -- if the parent rule is specified but the child is not, the assumption is that
//  the child rule is the same.
//
//  NOTE:  With the separate functions above to compute stencils for each rule, this method
//  potentially serves as a template for any Scheme to determine, compute and combine
//  stencils to support creasing in its full generality.  At this point I see no reason why
//  the Loop implementation shouldn't be identical.
//
template <>
void
SdcScheme<CATMARK>::ComputeVertexVertexStencil(SdcVertexNeighborhood const& neighborhood,
                                               SdcStencil<float>&           stencil,
                                               SdcCrease::Rule              parentRule,
                                               SdcCrease::Rule              childRule) const
{
    //  Quick compute and exit for the most common case:
    if (parentRule == SdcCrease::RULE_SMOOTH) {
        assignVertexVertexStencilAsSmooth(neighborhood, stencil);
        return;
    }

    //
    //  To distinguish Rules that are locally determined from what are passed in -- the
    //  "p" and "c" prefixes refering to "parent" and "child" are used throughout...
    //
    SdcCrease::Rule pRule = parentRule;
    SdcCrease::Rule cRule = childRule;

    if ((pRule != SdcCrease::RULE_UNKNOWN) && (cRule == SdcCrease::RULE_UNKNOWN)) {
        cRule = pRule;
    }

    //
    //  Assess the need for creasing/sharpness information to determine and combine
    //  interpolation rules:
    //
    int edgeCount = neighborhood.GetEdgeCount();
    int faceCount = neighborhood.GetFaceCount();

    SdcCrease creasing(_options);
    float     creasingSharpnessBuffer[2*edgeCount];
    bool      ignoreCreasing = false;

    //
    //  Determine if we need the parent edge sharpness values -- identify/gather if so
    //  and use it to compute the parent rule if unspecified:
    //
    float*       pSharpnessBuffer = creasingSharpnessBuffer;
    float const* pSharpness       = neighborhood.GetEdgeSharpness();
    float        pVertexSharpness = neighborhood.GetVertexSharpness();

    bool requireParentSharpness = (pRule == SdcCrease::RULE_UNKNOWN) ||
                                  (pRule == SdcCrease::RULE_CREASE);

    if (requireParentSharpness && (pSharpness == 0)) {
        if (neighborhood.GatherSharpness(pVertexSharpness, pSharpnessBuffer) > 0) {
            pSharpness = pSharpnessBuffer;
        }
        ignoreCreasing = (pSharpness == 0);
    }
    if (pRule == SdcCrease::RULE_UNKNOWN) {
        if (!ignoreCreasing) {
            pRule = creasing.DetermineVertexVertexRule(pVertexSharpness, edgeCount, pSharpness);
        } else {
            //  May want more inspection of Options and topology when sharpness values are missing...
            pRule = (edgeCount == faceCount) ? SdcCrease::RULE_SMOOTH : SdcCrease::RULE_CREASE;
        }
    }

    //
    //  With the parent Rule now known, we can make some quick inferences about the child rule
    //  to avoid the effort of fully determining it from the sharpness values:
    //
    //  Remember -- parent rule SMOOTH means it is the only rule relevant, i.e.
    //  child rule may differ (only due to applied edits) but should have no fractional
    //  component as SMOOTH should dominate (need to verify this).  So potentially no
    //  need to compute child rule in this case.
    //
    //
    if (cRule == SdcCrease::RULE_UNKNOWN) {
        if (pRule == SdcCrease::RULE_SMOOTH) {
            cRule = SdcCrease::RULE_SMOOTH;
        } else if (ignoreCreasing) {
            cRule = pRule;
        }
    }

    //
    //  Determine if we need the child edge sharpness values -- identify/gather if so
    //  and use it to compute the child rule if unspecified:
    //
    float*       cSharpnessBuffer = creasingSharpnessBuffer + edgeCount;
    float const* cSharpness       = neighborhood.GetChildEdgeSharpness();
    float        cVertexSharpness = neighborhood.GetChildVertexSharpness();

    bool requireChildSharpness = (cRule == SdcCrease::RULE_UNKNOWN) || (cRule != pRule);

    if (requireChildSharpness && (cSharpness == 0)) {
        if (neighborhood.GatherChildSharpness(cVertexSharpness, cSharpnessBuffer) > 0) {
            cSharpness = cSharpnessBuffer;
        }
    }
    if (cRule == SdcCrease::RULE_UNKNOWN) {
        if (cSharpness) {
            cRule = creasing.DetermineVertexVertexRule(cVertexSharpness, edgeCount, cSharpness);
        } else {
            //  Should we allow this?  Or assert() if creasing not ignored...
            cRule = pRule;
        }
    }

    //  Add special cases here to deal with SMOOTH and DART, which are essentially equivalent
    //  (so we should be able to avoid blending between the two).  We dealt with the parent
    //  SMOOTH case previously, so only the DART-to-SMOOTH transition remains... (?)

    //
    //  Assemble the parent stencil first.  If the child rule differs, compute a local child
    //  stencil and combine.  Note that in order to compute the fractional weight, both parent
    //  and child sharpness values must be made available:
    //
    if ((pRule == SdcCrease::RULE_SMOOTH) || (pRule == SdcCrease::RULE_DART)) {
        assignVertexVertexStencilAsSmooth(neighborhood, stencil);
    } if (pRule == SdcCrease::RULE_CORNER) {
        assignVertexVertexStencilAsCorner(neighborhood, stencil);
    } if (pRule == SdcCrease::RULE_CREASE) {
        assignVertexVertexStencilAsCrease(neighborhood, stencil, pSharpness);
    }
    if (pRule != cRule) {
        float             cStencilBuffer[1 + 2 * edgeCount];
        SdcStencil<float> cStencil(cStencilBuffer, cStencilBuffer + 1, cStencilBuffer + 1 + edgeCount);

        if ((cRule == SdcCrease::RULE_SMOOTH) || (cRule == SdcCrease::RULE_DART)) {
            assignVertexVertexStencilAsSmooth(neighborhood, cStencil);
        } if (cRule == SdcCrease::RULE_CORNER) {
            assignVertexVertexStencilAsCorner(neighborhood, cStencil);
        } if (cRule == SdcCrease::RULE_CREASE) {
            assignVertexVertexStencilAsCrease(neighborhood, cStencil, cSharpness);
        }

        float pWeight = creasing.ComputeFractionalWeightAtVertex(pVertexSharpness,
                                        edgeCount, pSharpness, cSharpness);
        float cWeight = 1.0 - pWeight;

        combineVertexVertexStencils(stencil, pWeight, cStencil, cWeight);
    }
}

//  Is this needed for some compilers?
template class SdcScheme<CATMARK>;

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
