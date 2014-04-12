// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#ifndef SDC_EDGE_NEIGHBORHOOD_H
#define SDC_EDGE_NEIGHBORHOOD_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  The "edge neighborhood" class:
//
//  For most cases the required information and the computation of the stencil is relatively
//  trivial -- the presence of creasing (and so the Rule) is trivially determined and subdivisions
//  of sharpness to determine if we have a crease transition is usually simple.  Difficulty occurs
//  when a non-trivial creasing method is applied, e.g. the Chaikin method.  In this case we need
//  to know the sharpness values of the two subdivided child edges, which requires the sharpness
//  values around both end-vertices of the parent edge.  Its best to provide the subdivided edge
//  sharpness values when present.
//
//  The full set of requirements of the EdgeNieghborhood includes:
//      - purely topological information:
//          - number of incident faces
//          - number of verts per incident face (needed for Catmark tri-subdivision)
//      - sharpness information:
//          - sharpness of parent edge (to determine the parent edge Rule)
//          - sharpness of child edges (to determine the child vertex Rule)
//      - possible additional tags that may prove helpful:
//          - manifold
//          - regular
//
//  The EdgeNeighborhood is a base class that will suffice for most purposes.  In a context where
//  subdivision dependencies are well understood and relevant data is available, it should be
//  straight forward to specify the EdgeNeighborhood adequately for an appropriate stencil query.
//
//  Virtual gathering:
//      For use cases where it is desired to gather only what is needed on demand, a subset of the
//  requirements can be specified and the rest left to "virtual gathering", i.e. a set of virtual
//  methods that can be provided by a subclass to identify information only when needed.  Such
//  information includes:
//      - number of vertices per incident face
//      - sharpness of the two subdivided child edges
//  Note that the gathering does not update the Neighborhood instance -- to do so would violate
//  its "const" usage in the stencil queries.  Invokation of these methods provides a reference
//  to local storage for the results.
//
//  Anticipated usage within OpenSubdiv is not expected to require any subclassing.  Existing
//  needs in Hbr and Far, as well as new usage in Vtr, are all specific to each of the subdivision
//  schemes and so are able to identify and provide requirements accordingly.
//

//
//  Neighborhood required for an edge-vertex:
//
class SdcEdgeNeighborhood {
public:
    SdcEdgeNeighborhood() : _faceCount(0), _edgeSharpness(0.0),
                            _childSharpness(0), _faceValence(0)
                            { }
    SdcEdgeNeighborhood(int fCount) : _faceCount(fCount), _edgeSharpness(0.0),
                                      _childSharpness(0), _faceValence(0)
                                      { }
    virtual ~SdcEdgeNeighborhood() { }

    //
    //  Initializers for potential use after default construction:
    //
    void SetFaceCount(int fCount)                     { _faceCount = fCount; }
    void SetSharpness(float eSharp)                   { _edgeSharpness = eSharp; }
    void SetChildEdgeSharpness(float* childSharpness) { _childSharpness = childSharpness; }
    void SetFaceValence(int* valencePerFace)          { _faceValence = valencePerFace; }

    //
    //  Simple accessors:
    //
    int GetFaceCount() const { return _faceCount; }

    float GetSharpness() const { return _edgeSharpness; }

    float const* GetChildEdgeSharpness() const { return _childSharpness; }
    float*       GetChildEdgeSharpness()       { return _childSharpness; }

    int const* GetFaceValence() const { return _faceValence; }
    int*       GetFaceValence()       { return _faceValence; }

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
    bool HasValencePerFace() const     { return _faceValence != 0; }
    bool HasChildEdgeSharpness() const { return _childSharpness != 0; }

    virtual int GatherValencePerFace(int*)       { return 0; }
    virtual int GatherChildEdgeSharpness(float*) { return 0; }

public:
    //  Is it worth making SdcScheme<TYPE> a friend, or are contents accessible enough...

    //  Possible additions:
    //  bool _isRegular;
    //  bool _isManifold;

    int   _faceCount;
    float _edgeSharpness;

    //  Members to be specified or externally gathered on demand:
    float* _childSharpness;
    int*   _faceValence;
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* SDC_EDGE_NEIGHBORHOOD_H */

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
