// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_CREASE_H
#define SDC_CREASE_H

#include "../version.h"

#include "../sdc/options.h"
#include "../sdc/arrayInterface.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  Types, constants and utilities related to semi-sharp creasing -- whose implementation is
//  independent of the subdivision scheme.
//
//  SdcCrease is intended to be a light-weight, trivially constructed class that computes
//  crease-related properties.  An instance of an SdcCrease is defined with a set of options
//  that include current and future variations that will impact computations involving
//  sharpness values.
//
//  We do not to use Neighborhoods here as input.  Since their sharpness values are potentially
//  not specified (and gathered on demand), and the methods here rely more on the sharpness
//  values and less on the topology, we choose to work directly with the sharpness values for
//  more flexibility.  We also follow the trend of using primitive arrays in the interface.
//
//  Note on the need for and use of sharpness values:
//      In general, stencil queries rely on the sharpness values.  The common case of a smooth
//  vertex, when known, avoids the need to inspect them, but unless the rules are well understood,
//  users will be expected to provided them -- particularly when they expect the stencil queries
//  to do all of the work (just determining if a vertex is smooth will require inspection of
//  incident edge sharpness). 
//      Stencil queries will occassionally require the subdivided sharpness values around the
//  child vertex.  So users will be expected to either provide them up front when known, or to be
//  gathered on demand.  Any implementation of subdivision with creasing cannot avoid subdividing
//  the sharpness values first, so keeping them available for re-use is a worthwhile consideration.
//

class SdcCrease {
public:
    //
    //  There is little reason to make sharpness anything other than a float -- maybe
    //  half-float, but double is unnecessary.  So is a typedef for Sharpness worthwhile?
    //
    //typedef float Sharpness;

    static float const SMOOTH;    // =  0.0f, do we really need this?
    static float const INFINITE;  // = 10.0f;

    //
    //  Enum for the types of subdivision rules applied based on sharpness values (note these
    //  correspond to Hbr's vertex "mask").  Note that the values explicitly assigned the
    //  enumerations correspond to the number of sharp edges encountered (with Corner >= 3).
    //
    enum Rule {
        RULE_SMOOTH  = 0,
        RULE_DART    = 1,
        RULE_CREASE  = 2,
        RULE_CORNER  = 3,
        RULE_UNKNOWN = 255
    };

public:
    SdcCrease() : _options() { }
    SdcCrease(SdcOptions const& options) : _options(options) { }
    ~SdcCrease() { }

    //
    //  Considering labeling the current/default/normal creasing method as "simple" in contrast
    //  to all others that are "complex".  The idea is that code can make certain assumptions
    //  and take some "simple" action in some cases to avoid the higher costs of dealing with
    //  more complex implementations.
    //
    bool IsSimple() const { return _options.GetCreasingMethod() == SdcOptions::CREASE_NORMAL; }

    //
    //  Sharpness subdivision:
    //      The simple case for computing a subdivided sharpness value is as follows:
    //        - Smooth edges or verts stay Smooth
    //        - Sharp edges or verts stay Sharp
    //        - semi-sharp edges or verts are decremented by 1.0
    //  but for Chaikin (and potentially future creasing schemes that improve upon it) the
    //  computation is more involved.  In the case of edges in particular, the sharpness of a
    //  child edge is determined by the sharpness in the neighborhood of the end vertex
    //  corresponding to the child.  For this reason, an alternative to subdividing sharpness
    //  that computes all child edges around a vertex is given.
    //
    float SubdivideVertexSharpness(float vertexSharpness) const;

    float SubdivideEdgeSharpnessAtVertex(float        edgeSharpness,
                                         int          incidentEdgeCountAtEndVertex,
                                         float const* edgeSharpnessAroundEndVertex) const;

    void SubdivideEdgeSharpnessAroundVertex(int          incidentEdgeCountAtVertex,
                                            float const* incidentEdgeSharpnessAroundVertex,
                                            float*       childEdgesSharpnessAroundVertex) const;

    //
    //  Rule determination:
    //      Stencil queries do not require the Rule to be known, it can be determined from
    //  the information provided, but it is generally more efficient when the Rule is known
    //  and provided.  In particular, the Smooth case dominates and is known to be applicable
    //  based on the origin of the vertex without inspection of sharpness.
    //
    Rule DetermineVertexVertexRule(float        vertexSharpness,
                                   int          incidentEdgeCount,
                                   float const* incidentEdgeSharpness) const;

    //
    //  Transitional weighting:
    //      When the rules applicable to a parent vertex and its child differ, one or more
    //  sharpness values has "decayed" to zero.  Both rules are then applicable and blended 
    //  by a weight between 0 and 1 that reflects the transition.  Most often this will be
    //  a single sharpness value that decays from within the interval [0,1] to zero -- and
    //  the weight to apply is exactly that sharpness value -- but more than one may decay,
    //  and values > 1 may also decay to 0 in a single step while others within [0,1] may
    //  remain > 0.
    //      So to properly determine a transitional weight, sharpness values for both the
    //  parent and child must be inspected, combined and clamped accordingly.
    //
    //  Open questions:
    //      - does this method need to be public, or can it reside within the stencil
    //        query classes? (though it would be the same for anything non-linear, so
    //        may be worth making a protected method somewhere)
    //      - does this need further consideration at an edge-vertex?
    //          - no, the edge-vertex case is far more trivial:  one non-zero sharpness
    //            for the edge that decays to zero for one or both child edges -- the
    //            transitional weight is simply the edge sharpness (clamped to 1)
    //
    float ComputeFractionalWeightAtVertex(float        vertexSharpness,
                                          int          incidentEdgeCount,
                                          float const* incidentEdgeSharpness,
                                          float const* childEdgesSharpness) const;

    //  Would these really help?  Maybe only need Rules for the vertex-vertex case...
    //
    //  Rule DetermineEdgeVertexRule(float parentEdgeSharpness) const;
    //  Rule DetermineEdgeVertexRule(float childEdge1Sharpness, float childEdge2Sharpness) const;

protected:
    //
    //  Unclear if its worth making these public -- may encourage decision making on the users
    //  part that may be flawed and better left to public methods here (and Hbr has similar
    //  methods that behave differently, e.g. IsSharp(), which may lead to confusion)...
    //
    //  Also consider making these static.
    //
    bool isSmooth(float sharpness) const   { return sharpness <= SMOOTH; }
    bool isInfinite(float sharpness) const { return sharpness >= INFINITE; }
    bool isSharp(float sharpness) const    { return !isSmooth(sharpness); }

    float decrementSharpness(float sharpness) const;

private:
    SdcOptions _options;
};


//
//  Non-trivial inline declarations:
//
inline float
SdcCrease::decrementSharpness(float sharpness) const
{
    if (isSmooth(sharpness)) return SdcCrease::SMOOTH;  // redundant but most common
    if (isInfinite(sharpness)) return SdcCrease::INFINITE;
    if (sharpness > 1.0) return (sharpness - 1.0);
    return SdcCrease::SMOOTH;
}

inline float
SdcCrease::SubdivideVertexSharpness(float vertexSharpness) const
{
    return decrementSharpness(vertexSharpness);
}


//
//  For temporary backward compatibility in the existing body of code that already uses
//  them -- unless we decide they are worth keeping:
//
typedef SdcCrease::Rule  SdcRule;

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_CREASE_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
