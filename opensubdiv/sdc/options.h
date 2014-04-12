// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_OPTIONS_H
#define SDC_OPTIONS_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  This header contains all supported options that can be applied to a subdivision
//  scheme to affect the shape of the limit surface.  These differ from approximations
//  that may be applied at a higher level, i.e. options to limit the level of feature
//  adaptive subdivision, options to ignore fractional creasing, or creasing entirely,
//  etc.  These options define a particular limit surface.
//
//  The intent is that these sets of options be defined at a high-level and propagated
//  into the lowest-level computation in support of each subdivision scheme.  Ideally
//  it remains a set of bit-fields (essentially an int) and so remains light weight and
//  easily passed down by value.
//
//  Questions:
//      Should the individual enum's be nested within the class or independent?
//
//  Note:
//      A case can be made that the CreaseMethod enum is better defined as part of the
//  Crease class, but the goal is to try and put them all in one place.  We could define
//  it there and aggregate it into Options here, but we need to be careful about the
//  possibility of circular dependencies (nesting types in classes inhibits forward
//  declaration).
//
class SdcOptions {
public:
    enum BoundaryInterpolation {
        BOUNDARY_NONE,
        BOUNDARY_EDGE_ONLY,
        BOUNDARY_EDGE_AND_CORNER,
        BOUNDARY_ALWAYS_SHARP
    };
    enum CreasingMethod {
        CREASE_NORMAL,
        CREASE_CHAIKIN
    };
    enum TriangleSubdivision {
        TRI_SUB_NORMAL,
        TRI_SUB_OLD,
        TRI_SUB_NEW
    };
    enum NonManifoldInterpolation {
        NON_MANIFOLD_NONE,
        NON_MANIFOLD_SMOOTH,
        NON_MANIFOLD_SHARP
    };

public:
    //  Trivial constructor and destructor:
    SdcOptions() : _boundaryInterp(BOUNDARY_NONE),
                   _nonManInterp(NON_MANIFOLD_NONE),
                   _creasingMethod(CREASE_NORMAL),
                   _triangleSub(TRI_SUB_NORMAL),
                   _hbrCompatible(false) { }
    ~SdcOptions() { }

    //
    //  Trivial get/set methods:
    //
    BoundaryInterpolation GetBoundaryInterpolation() const { return (BoundaryInterpolation)_boundaryInterp; }
    void                  SetBoundaryInterpolation(BoundaryInterpolation b) { _boundaryInterp = b; }

    CreasingMethod GetCreasingMethod() const { return (CreasingMethod)_creasingMethod; }
    void           SetCreasingMethod(CreasingMethod c) { _creasingMethod = c; }

    NonManifoldInterpolation GetNonManifoldInterpolation() const { return (NonManifoldInterpolation)_nonManInterp; }
    void                     SetNonManifoldInterpolation(NonManifoldInterpolation n) { _nonManInterp = n; }

    TriangleSubdivision GetTriangleSubdivision() const { return (TriangleSubdivision)_triangleSub; }
    void                SetTriangleSubdivision(TriangleSubdivision t) { _triangleSub = t; }

    //
    //  This may be premature, but it is useful to have some kind of flag so that users can be assured
    //  the options and meshes they specify are compliant with Hbr, RenderMan, etc.  How to measure that
    //  is still ill-defined given versions of Hbr, prMan will evolve...
    //
    bool GetHbrCompatibility() const { return _hbrCompatible; }
    void SetHbrCompatibility(bool onOrOff) { _hbrCompatible = onOrOff; }

private:
    //  Bitfield members:
    unsigned int _boundaryInterp : 2;
    unsigned int _nonManInterp   : 2;
    unsigned int _creasingMethod : 2;
    unsigned int _triangleSub    : 2;
    unsigned int _hbrCompatible  : 1;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_OPTIONS_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
