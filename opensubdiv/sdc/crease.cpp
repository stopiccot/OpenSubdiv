// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/crease.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Declarations of creasing constants and non-inline methods:
//
float const SdcCrease::SMOOTH   =  0.0f;
float const SdcCrease::INFINITE = 10.0f;


//
//  Creasing queries dependent on sharpness values:
//
SdcCrease::Rule
SdcCrease::DetermineVertexVertexRule(float        vertexSharpness,
                                     int          incidentEdgeCount,
                                     float const* incidentEdgeSharpness) const
{
    if (isSharp(vertexSharpness)) return SdcCrease::RULE_CORNER;

    int sharpCount = 0;
    for (int i = 0; i < incidentEdgeCount; ++i) {
        sharpCount += isSharp(incidentEdgeSharpness[i]);
    }
    return (sharpCount < 3) ? (SdcCrease::Rule) sharpCount : SdcCrease::RULE_CORNER;
}

float
SdcCrease::ComputeFractionalWeightAtVertex(float        vertexSharpness,
                                           int          incidentEdgeCount,
                                           float const* parentSharpness,
                                           float const* childSharpness) const
{
    int   transitionCount = ((0.0 < vertexSharpness) && (vertexSharpness <= 1.0));
    float transitionSum   = transitionCount ? vertexSharpness : 0.0;

    //
    //  We need the child-edge sharpness values for non-simple methods to ensure
    //  that the sharpness went from a non-zero value (potentially greater than
    //  1.0) to zero...
    //
    if (IsSimple() || (childSharpness == 0)) {
        for (int i = 0; i < incidentEdgeCount; ++i) {
            if ((parentSharpness[i] > 0.0) && (parentSharpness[i] <= 1.0)) {
                transitionSum   += parentSharpness[i];
                transitionCount ++;
            }
        }
    } else {
        for (int i = 0; i < incidentEdgeCount; ++i) {
            if ((parentSharpness[i] > 0.0) && (childSharpness[i] <= 0.0)) {
                transitionSum   += parentSharpness[i];
                transitionCount ++;
            }
        }
    }
    if (transitionCount == 0) return 0.0;
    float fractionalWeight = transitionSum / transitionCount;
    return (fractionalWeight > 1.0) ? 1.0 : fractionalWeight;
}

//
//  Subdividing edge sharpness values (vertex sharpness is inline):
//
float
SdcCrease::SubdivideEdgeSharpnessAtVertex(float         edgeSharpness,
                                          int           incEdgeCountAtVertex,
                                          float const * incEdgeSharpness) const
{
    if (IsSimple() || (incEdgeCountAtVertex < 2)) {
        return decrementSharpness(edgeSharpness);
    }

    if (isSmooth(edgeSharpness)) return SdcCrease::SMOOTH;
    if (isInfinite(edgeSharpness)) return SdcCrease::INFINITE;

    float sharpSum   = 0.0;
    int   sharpCount = 0;
    for (int i = 0; i < incEdgeCountAtVertex; ++i) {
        sharpCount += (incEdgeSharpness[i] > 0.0);
        sharpSum   += incEdgeSharpness[i];
    }
    if (sharpCount > 1) {
        //  Chaikin rule is 3/4 original sharpness + 1/4 average of the others

        float avgSharpnessAtVertex = (sharpSum - edgeSharpness) / (sharpCount - 1);

        edgeSharpness = (0.75 * edgeSharpness) + (0.25 * avgSharpnessAtVertex);
    }
    edgeSharpness -= 1.0;
    return (edgeSharpness > 0.0) ? edgeSharpness : 0.0;
}

void
SdcCrease::SubdivideEdgeSharpnessAroundVertex(int          edgeCount,
                                              float const* parentSharpness,
                                              float *      childSharpness) const
{
    if (IsSimple() || (edgeCount < 2)) {
        for (int i = 0; i < edgeCount; ++i) {
            childSharpness[i] = decrementSharpness(parentSharpness[i]);
        }
        return;
    }
    
    //
    //  Chaikin creasing is most efficiently computed for all edges around a vertex at
    //  once as the subdivided value for each creased edge depends on the average of
    //  the other edges around the vertex.  So we can sum up the sharpness around the
    //  vertex once and use that for each edge, rather than iterating around the vertex
    //  for each incident edge.
    //
    if (_options.GetCreasingMethod() == SdcOptions::CREASE_CHAIKIN) {
        float sharpSum   = 0.0;
        int   sharpCount = 0;
        for (int i = 0; i < edgeCount; ++i) {
            sharpCount += (parentSharpness[i] > 0.0);
            sharpSum   += parentSharpness[i];
        }

        //
        //  The smooth case is most common -- specialize for it first:
        //
        if (sharpCount == 0) {
            for (int i = 0; i < edgeCount; ++i) {
                childSharpness[i] = 0.0;
            }
        } else {
            for (int i = 0; i < edgeCount; ++i) {
                float const& pSharp = parentSharpness[i];
                float&       cSharp = childSharpness[i];

                if (isSmooth(pSharp)) {
                    cSharp = SdcCrease::SMOOTH;
                } else if (isInfinite(pSharp)) {
                    cSharp = SdcCrease::INFINITE;
                } else if (sharpCount == 1) {
                    //  Need special case here anyway to avoid divide by zero below...
                    cSharp = decrementSharpness(pSharp);
                } else {
                    float pOtherAverage = (sharpSum - pSharp) / (sharpCount - 1);

                    //  Chaikin rule is 3/4 original sharpness + 1/4 average of the others
                    cSharp = ((0.75 * pSharp) + (0.25 * pOtherAverage)) - 1.0;
                    if (cSharp < 0.0) cSharp = 0.0;
                }
            }
        }
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
