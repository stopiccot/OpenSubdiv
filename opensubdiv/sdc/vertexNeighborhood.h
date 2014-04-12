// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_VERTEX_NEIGHBORHOOD_H
#define SDC_VERTEX_NEIGHBORHOOD_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  The "vertex neighborhood" class:
//
//  In the most general case of a stencil query though, no "Rule" analysis or subdivision of
//  sharpness values has been performed, so the general query may potentially access all that
//  might be required.  This is somewhat impractical (no current or anticipated usage within
//  OpenSubdiv requires it) but the full set of requirements includes:
//
//      - purely topological information:
//          - number of incident edges
//          - number of incident faces
//      - sharpness information:
//          - sharpness of parent vertex and all of its incident parent edges
//          - sharpness of child vertex and all of its incidentn child edges
//      - possible additional tags that may prove helpful:
//          - manifold
//          - regular
//
//  The VertexNeighborhood is a base class that will suffice for most purposes.  In a context where
//  subdivision dependencies are well understood and relevant data is available, it should be
//  straight forward to specify the VertexNeighborhood adequately for an appropriate stencil query.
// 
//  Virtual gathering:
//      For use cases where it is desired to gather only what is needed on demand, a subset of the
//  requirements can be specified and the rest left to "virtual gathering", i.e. a set of virtual
//  methods that can be provided by a subclass to identify information only when needed.  Such
//  information includes:
//      - sharpness values for the parent vertex and its incident edges
//      - sharpness values for the child vertex and its incident edges
//  Note that the gathering does not update the Neighborhood instance -- to do so would violate
//  its "const" usage in the stencil queries.  Invokation of these methods provides a reference
//  to local storage for the results.
//
//  Anticipated usage within OpenSubdiv is not expected to require any subclassing.  Existing
//  needs in Hbr and Far, as well as new usage in Vtr, are all specific to each of the subdivision
//  schemes and so are able to identify and provide requirements accordingly.
//

//
//  Neighborhood required for a vertex-vertex:
//
class SdcVertexNeighborhood {
public:
    SdcVertexNeighborhood() : _edgeCount(0), _faceCount(0),
                              _vertSharpness(0.0), _vertSharpnessChild(0.0),
                              _edgeSharpness(0), _edgeSharpnessChild(0) { }
    SdcVertexNeighborhood(int fCount, int eCount) : _edgeCount(eCount), _faceCount(fCount),
                                                    _vertSharpness(0.0), _vertSharpnessChild(0.0),
                                                    _edgeSharpness(0), _edgeSharpnessChild(0) { }
    virtual ~SdcVertexNeighborhood() { }

    //
    //  Initializers for potential use after default construction:
    //
    void SetEdgeCount(int eCount) { _edgeCount = eCount; }
    void SetFaceCount(int fCount) { _faceCount = fCount; }

    void SetSharpness(     float vSharp, float* eSharp) { _vertSharpness      = vSharp, _edgeSharpness      = eSharp; }
    void SetChildSharpness(float vSharp, float* eSharp) { _vertSharpnessChild = vSharp, _edgeSharpnessChild = eSharp; }

    //
    //  Simple accessors (NOTE:  consider using SdcArrays vs ptrs -- they have iterators)
    //
    int  GetEdgeCount() const { return _edgeCount; }
    int  GetFaceCount() const { return _faceCount; }

    float        GetVertexSharpness() const { return _vertSharpness; }
    float const* GetEdgeSharpness() const   { return _edgeSharpness; }
    float*       GetEdgeSharpness()         { return _edgeSharpness; }

    float        GetChildVertexSharpness() const { return _vertSharpnessChild; }
    float const* GetChildEdgeSharpness() const   { return _edgeSharpnessChild; }
    float*       GetChildEdgeSharpness()         { return _edgeSharpnessChild; }

    //
    //  Virtual methods for lazily gathering edge-sharpness when required but left unspecified:
    //
    //  The gathering methods return the number of entries gathered, which should match the
    //  number of components assigned (number of edges in this case) or 0, which is used as a
    //  shortcut for common/default cases.  For sharpness gathering, returning 0 for parent
    //  sharpness indicates no sharpness present, i.e. all smooth, while returning 0 for the
    //  child when the parent has sharpness is an indication that it should be computed as
    //  needed from the parent instead of provided.
    //
    bool HasSharpness() const       { return _edgeSharpness != 0; }
    bool HasChildSharpness() const  { return _edgeSharpnessChild != 0; }

    virtual int GatherSharpness(     float&, float*) const { return 0; }
    virtual int GatherChildSharpness(float&, float*) const { return 0; }

public:
    //  Is it worth making SdcScheme<TYPE> a friend, or are contents accessible enough...

    //  Possible additions:
    //  bool _isRegular;
    //  bool _isManifold;

    int _edgeCount;
    int _faceCount;

    //  Members to be specified or externally gathered on demand:
    float  _vertSharpness;
    float  _vertSharpnessChild;
    float* _edgeSharpness;
    float* _edgeSharpnessChild;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_VERTEX_NEIGHBORHOOD_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
