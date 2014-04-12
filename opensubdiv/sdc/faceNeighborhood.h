// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_FACE_NEIGHBORHOOD_H
#define SDC_FACE_NEIGHBORHOOD_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  The "face neighborhood" class:
//
//  Currently there is no need for any information other than the number of incident vertices
//  (the number of incident edges will be the same).  No sharpess values for its vertices or
//  edges are currently required for any purpose.  Its unlikely that will change, but having
//  it in the interface to the stencil query for a face-vertex will simplify any required
//  future extension.
//
//  Note:
//      This is so trivial as to almost be useless -- it completes a pattern used for other
//  neighborhoods and may have future uses.  It does make me want to combine all three of the
//  neighborhood classes into one header file.  Is there a precedence for that in Far, or are
//  classes always separated?
//

//
//  Neighborhood required for an face-vertex:
//
class SdcFaceNeighborhood {
public:
    SdcFaceNeighborhood() : _vertCount(0) { }
    SdcFaceNeighborhood(int vCount) : _vertCount(vCount) { }
    virtual ~SdcFaceNeighborhood() { }

    //
    //  Initializers for potential use after default construction and accessor:
    //
    void SetVertexCount(int vCount) { _vertCount = vCount; }
    int  GetVertexCount() const     { return _vertCount; }

public:
    //  Potential additions:
    //  bool _isRegular;
    //  bool _isManifold;

    int _vertCount;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_FACE_NEIGHBORHOOD_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
