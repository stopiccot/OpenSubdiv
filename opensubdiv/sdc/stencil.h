// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_STENCIL_NEIGHBORHOOD_H
#define SDC_STENCIL_NEIGHBORHOOD_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Simple struct to hold the stencil weights for the three kinds of vertex stencils distinguished
//  by their origin -- face-vertex, edge-vertex and vertex-vertex.
//
//  The user is expected to provide pre-allocated buffers to be populated by stencil queries.  The
//  populated sizes of these buffers is returned by the queries and so any assignment to those
//  members on construction or explicitly prior to a query is irrelevant.
//
//  Important:
//      Note that "face weights" currently refer to the average/center of the incident face, and
//  not a particular (e.g. opposite) vertex associated with it.  If we want face-weights to refer
//  to opposite vertices of the face, we will need to accomodate (valence_i - 3) vertices for each
//  face for the "face weights", where valence_i is the number of vertices in the i'th incident face.
//      Given the preference within OSD for the face-centers, it would be simplest to provide this
//  in a Stencil method to translate a given face-centered stencil into the other form.  Such a
//  translation method would require the number of vertices per face for the most general case (only
//  required for the cage) and be overloaded for the regular cases.
//
//  Questions:
//      Is it worth having three specialized stencil types instead of this general one?  Originally
//  there were three -- with each stencil class local to the Nieghborhood class to which it was
//  associated -- but this seemed unnecessary.  In some cases it also became detrimental as having
//  multiple types to hold the same information made writing some general code (like the application
//  of the weights) more tedious.
//

template <typename REAL = float>
struct SdcStencil {
    SdcStencil() { }
    SdcStencil(REAL* vWeights, REAL* eWeights = 0, REAL* fWeights = 0) :
                           _vertWeights(vWeights), _edgeWeights(eWeights), _faceWeights(fWeights) { }

    void  SetVertexWeights(REAL* vWeights) { _vertWeights = vWeights; }
    REAL* GetVertexWeights()               { return _vertWeights; }
    int   GetVertexWeightCount()           { return _vertCount; }

    void  SetEdgeWeights(REAL* eWeights) { _edgeWeights = eWeights; }
    REAL* GetEdgeWeights()               { return _edgeWeights; }
    int   GetEdgeWeightCount()           { return _edgeCount; }

    void  SetFaceWeights(REAL* fWeights) { _faceWeights = fWeights; }
    REAL* GetFaceWeights()               { return _faceWeights; }
    int   GetFaceWeightCount()           { return _faceCount; }

public:
    REAL* _vertWeights;
    REAL* _edgeWeights;
    REAL* _faceWeights;

    int _vertCount;
    int _edgeCount;
    int _faceCount;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_STENCIL_NEIGHBORHOOD_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
