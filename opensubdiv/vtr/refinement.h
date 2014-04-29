//
//   Copyright 2014 DreamWorks Animation LLC.
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//
#ifndef VTR_REFINEMENT_H
#define VTR_REFINEMENT_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../vtr/types.h"
#include "../vtr/level.h"

#include <vector>

//
//  Declaration for the main refinement class (VtrRefinement) and its pre-requisites:
//
namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class VtrSparseSelector;

//
//  VtrRefinement:
//      A refinement is a mapping between two levels -- relating the components in the original
//  (parent) level to the one refined (child).  The refinement may be complete (uniform) or sparse
//  (adaptive or otherwise selective), so not all components in the parent level will spawn
//  components in the child level.
//
//  At a high level, all that is necessary in terms of interface is to construct, initialize
//  (linking the two levels), optionally select components for sparse refinement (via use of the
//  VtrSparseSelector) and call the refine() method.  This is the usage expected in FarRefineTables.
//
//  Since we really want this class to be restricted from public access eventually, all methods
//  begin with lower case (as is the convention for protected methods) and the list of friends
//  will be maintained more strictly.
//
class VtrRefinement
{
public:
    VtrRefinement();
    ~VtrRefinement();

    void initialize(VtrLevel& parent, VtrLevel& child);

    VtrLevel const& parent() const { return *_parent; }

    VtrLevel const& child() const  { return *_child; }
    VtrLevel&       child()        { return *_child; }

    //
    //  Options associated with the actual refinement operation, which are going to get
    //  quite involved to ensure that the refinement of data that is not of interest can
    //  be suppressed.  For now we have:
    //
    //      "sparse": the alternative to uniform refinement, which requires that
    //          components be previously selected/marked to be included.
    //
    //      "face topology only": this is one that may get broken down into a finer
    //          set of options.  It suppresses "full topology" in the child level
    //          and only generates what is necessary to define the list of faces.
    //          This is only one of the six possible topological relations that
    //          can be generated -- we may eventually want a flag for each.
    //
    //      "compute masks": this is intended to be temporary, along with the data
    //          members associated with it -- it will trigger the computation and
    //          storage of mask weights for all child vertices.  This is naively
    //          stored at this point and exists only for reference.
    //
    //  Where "tags" reside -- either in the Levels or in the Refinement class -- is
    //  under consideration.  Some tags specifically involve relationships between
    //  levels, so they arguably belong here.
    //
    struct Options {
        Options() : _sparse(0),
                    _parentTagging(0),
                    _childTagging(0),
                    _faceTopologyOnly(0),
                    _computeMasks(0) { }

        unsigned int _sparse           : 1;
        unsigned int _parentTagging    : 1;
        unsigned int _childTagging     : 1;
        unsigned int _faceTopologyOnly : 1;
        unsigned int _computeMasks     : 1;
    };

    void refine(Options options = Options());

public:
    //
    //  Tags on parent and child components related to the refinement -- note that these
    //  are optional in the refinement and should not be requested if not enabled:
    //
    struct FaceTag {
        FaceTag() : _transitional(0) { }

        unsigned int _transitional : 4;  // bit-per edge if <= 4, otherwise non-zero
    };
    struct EdgeTag {
        EdgeTag() : _transitional(0) { }

        unsigned int _transitional : 1;  // adjacent faces not refined consistently
    };
    struct VertexTag {
        VertexTag() : _incomplete(0) { }

        unsigned int _incomplete : 1;  // does not support have the full neighborhood for its limit
    };

    //  Use these more for assertions than branching:
    bool hasParentFaceTags() const   { return _parentFaceTag.size() > 0; }
    bool hasParentEdgeTags() const   { return _parentEdgeTag.size() > 0; }
    bool hasParentVertexTags() const { return _parentVertexTag.size() > 0; }

    bool hasChildFaceTags() const   { return _childFaceTag.size() > 0; }
    bool hasChildEdgeTags() const   { return _childEdgeTag.size() > 0; }
    bool hasChildVertexTags() const { return _childVertexTag.size() > 0; }

    //  Specific queries related to the tags:
    bool isParentEdgeTransitional(VtrIndex e) const      { return _parentEdgeTag[e]._transitional; }
    bool isParentFaceTransitional(VtrIndex f) const      { return _parentFaceTag[f]._transitional != 0; }
    int  getParentFaceTransitionalMask(VtrIndex f) const { return _parentFaceTag[f]._transitional; }

    bool isChildVertexIncomplete(VtrIndex v) const { return _childVertexTag[v]._incomplete; }

public:
    friend class VtrSparseSelector;

    //
    //  Access to members -- VtrSparseSelector currently modifies these to mark components for
    //  sparse refinement:
    //
    //  Const:
    int childVerticesFromFacesCount() const    { return _childVertFromFaceCount; }
    int childVerticesFromEdgesCount() const    { return _childVertFromEdgeCount; }
    int childVerticesFromVerticesCount() const { return _childVertFromVertCount; }

    VtrIndex const& faceChildVertexIndex(VtrIndex f) const   { return _faceChildVertIndex[f]; }
    VtrIndex const& edgeChildVertexIndex(VtrIndex e) const   { return _edgeChildVertIndex[e]; }
    VtrIndex const& vertexChildVertexIndex(VtrIndex v) const { return _vertChildVertIndex[v]; }

    VtrIndexArray const faceChildFaces(VtrIndex parentFace) const;
    VtrIndexArray const faceChildEdges(VtrIndex parentFace) const;
    VtrIndexArray const edgeChildEdges(VtrIndex parentEdge) const;

    //  Non-const:
    VtrIndex& faceChildVertexIndex(VtrIndex f)   { return _faceChildVertIndex[f]; }
    VtrIndex& edgeChildVertexIndex(VtrIndex e)   { return _edgeChildVertIndex[e]; }
    VtrIndex& vertexChildVertexIndex(VtrIndex v) { return _vertChildVertIndex[v]; }

    VtrIndexArray faceChildFaces(VtrIndex parentFace);
    VtrIndexArray faceChildEdges(VtrIndex parentFace);
    VtrIndexArray edgeChildEdges(VtrIndex parentEdge);

//
//  Remaining methods really should remain private...
//
private:
    //  The following methods need to be combined more effectively...
    void identifySparseChildComponents();
    void identifyUniformChildComponents();
    void assignChildComponentInventory();

    //  Main functional tasks in the refinement:
    void subdivideTopology();
    void subdivideVertexSharpness();
    void subdivideEdgeSharpness();
    void subdivideVertexRule();

    //  All this does is size the vectors and assign a constant initial value:
    void initializeParentToChildMappings(VtrIndex initValue);
    void printParentToChildMappings() const;

    void initializeParentTags();
    void initializeChildTags();

    //  Parent and child tags will likely be populated during refinement, but these
    //  methods will serve as a post-refinement process to populate then until it is
    //  clear how best to assign them:
    void populateParentTags();
    void populateChildTags();

    //  Methods for sizing the child topology vectors (note we only need four of the
    //  expected six here as face-edge shares face-vert and edge-vert is trivial):
    void initializeChildFaceVertexCountsAndOffsets();
    void initializeChildEdgeFaceCountsAndOffsets();
    void initializeChildVertexFaceCountsAndOffsets();
    void initializeChildVertexEdgeCountsAndOffsets();

    //  Methods for populating sections of child topology relations based on
    //  their origin in the parent -- 12 in all:
    void populateFaceVerticesFromParentFaces();
    void populateFaceEdgesFromParentFaces();
    void populateEdgeVerticesFromParentFaces();
    void populateEdgeVerticesFromParentEdges();
    void populateEdgeFacesFromParentFaces();
    void populateEdgeFacesFromParentEdges();
    void populateVertexFacesFromParentFaces();
    void populateVertexFacesFromParentEdges();
    void populateVertexFacesFromParentVertices();
    void populateVertexEdgesFromParentFaces();
    void populateVertexEdgesFromParentEdges();
    void populateVertexEdgesFromParentVertices();

    //  Methods for populating a child relation for a single parent component:
    //
    //  Note to self:  these methods and those above (for all components) were
    //  created in the midst of threading experimentation that was never finished.
    //  The intent was for each of the above 12 methods to be split out into one
    //  that could be invoked for a single component to allow different kinds of
    //  iteration -- with 12 corresponding methods for all components that would
    //  manage all and iterate/distribute accordingly.
    //
    //  So there really should be 12 methods below.  It turns out though that
    //  breaking them up this way may not be ideal for efficiency.  For some a
    //  common set of information is retrieved for the parent component which
    //  all derived child components can use.
    //
    //  When sparse refinement is driven more by iteration through the child
    //  components that are generated (rather than iterating through all parent
    //  components and skipping those children not generated) we should revist
    //  and refactor this.
    //
    void populateChildFaceVerticesFromParentFace(VtrIndex parentFace);
    void populateChildFaceEdgesFromParentFace(VtrIndex parentFace);
    void populateChildEdgeVerticesFromParentFace(VtrIndex parentFace);
    void populateChildEdgeVerticesFromParentEdge(VtrIndex parentEdge);
    void populateChildEdgeFacesFromParentFace(VtrIndex parentFace);
    void populateChildEdgeFacesFromParentEdge(VtrIndex parentEdge);

    void classifyChildVerticesFromParentFaces();
    void classifyChildVerticesFromParentEdges();
    void classifyChildVerticesFromParentVertices();

private:
    friend class VtrLevel;  //  Access for some debugging information

    VtrLevel* _parent;
    VtrLevel* _child;

    SdcType    _schemeType;
    SdcOptions _schemeOptions;

    bool _quadSplit;  // generalize this to SdcSplit later

    //
    //  Hierarchy vectors:
    //      - unclear yet if this should be bi-directional
    //
    //  References to components in a child level:
    //      Note that faces and edges will not spawn all children in cases of "selective"
    //  refinement (e.g. adaptive, including holes or otherwise select), so some children
    //  may be marked as invalid/unused.  These vectors are sized to match the topology
    //  relations and so the same counts and offsets apply.
    //
    //  The sizes of these vectors will dictate/correspond to the sizes of vectors in the
    //  child level.
    //
    //  Note that in most cases, but not all -- we can use the same offsets for the
    //  parent components children as are used for its incident components, e.g. we can
    //  use the face-verts offsets to find the child faces corresponding to each face-
    //  vert.  But in the case of Loop subdivision, the norm will be for each triagular
    //  face to have four child faces, so these child vectors may need their own set of
    //  offsets in some cases (in the Loop case, provided faces other than triangles are
    //  not supported -- which is currently the case -- integer multiples can be used
    //  insteadof offsets).
    //
    VtrIndexVector _faceChildFaceIndices;  // *cannot* always use face-vert counts/offsets
    VtrIndexVector _faceChildEdgeIndices;  // can use face-vert counts/offsets
    VtrIndexVector _faceChildVertIndex;

    VtrIndexVector _edgeChildEdgeIndices;  // trivial/corresponding pair for each
    VtrIndexVector _edgeChildVertIndex;

    VtrIndexVector _vertChildVertIndex;

    //
    //  Members to indicate the origin of child components.  These are determined as we
    //  size the vectors of child components and its useful to retain them for later use:
    //
    int _childFaceFromFaceCount;  // arguably redundant (all faces originate from faces)
    int _childEdgeFromFaceCount;
    int _childEdgeFromEdgeCount;
    int _childVertFromFaceCount;
    int _childVertFromEdgeCount;
    int _childVertFromVertCount;

    //
    //  References to components in a parent level relative to the child:
    //      Referring to parent components already specified by the child of a parent may
    //  be redundant/unnecessary, but the parent of vertices is often desired (why?).
    //  Note that the parent of a vertex may be a vertex, edge or face.  Given the way we
    //  arrange these, we should be able to tell from the index of a vertex at this level
    //  and the knowledge of children in its parent level, whether the vertex comes from
    //  a face, edge or vertex (child vertices are defined first from faces, then edges,
    //  then vertices).  We are considering maintaining the counts of these when children
    //  are generated for use in the parent level.  Otherwise a separate tag/indicator
    //  will be required.
    //
    //  TBD...

    //
    //  Tags for parent and child components -- optionally populated by the refinement:
    //
    //  Like other groups of vectors, consider aggregating these into one and using the
    //  Array classes to partition a single larger vector into arrays per component type.
    //
    std::vector<FaceTag>   _parentFaceTag;
    std::vector<EdgeTag>   _parentEdgeTag;
    std::vector<VertexTag> _parentVertexTag;

    std::vector<FaceTag>   _childFaceTag;
    std::vector<EdgeTag>   _childEdgeTag;
    std::vector<VertexTag> _childVertexTag;

    //  References to components in the base level (top-most ancestor) may be useful
    //  to copy non-interpolatible properties to all descendant components.


public:
    //  TEMPORARY -- FOR ILLUSTRATIVE PURPOSES ONLY...
    //
    //  Mask for the child vertices stored relative to parent topology, i.e. weights
    //  for a child face-vertex are stored relative to the parent face -- a weight for
    //  each of the parent face's vertices.
    //
    //  Currently the full complement of weights is used and expected to be applied, e.g.
    //  for a crease in the interior, there may be no face-vert weights in the stencil
    //  and so no need to apply them, but we require all and so set these to zero for now.
    //  We will need some kind of stencil type associated with each child vertex if we
    //  are to avoid doing so in order to detect the difference.
    //
    //  Note this is potentially extremely wasteful in terms of memory when the set of
    //  refined components in the child is small relative to the parent.  All possible
    //  stencil weights (i.e. for uniform refinement) will be allocated here if the
    //  corresonding counts/offset of the parent are to be used.
    //
    void computeMaskWeights();

    std::vector<float> _faceVertWeights;  // matches parent face vert counts and offsets
    std::vector<float> _edgeVertWeights;  // trivially 2 per parent edge
    std::vector<float> _edgeFaceWeights;  // matches parent edge face counts and offsets
    std::vector<float> _vertVertWeights;  // trivially 1 per parent vert
    std::vector<float> _vertEdgeWeights;  // matches parent vert edge counts and offsets
    std::vector<float> _vertFaceWeights;  // matches parent vert face counts and offsets
};

inline VtrIndexArray const
VtrRefinement::faceChildFaces(VtrIndex parentFace) const
{
    //
    //  Note this will need to vary based on the topological split applied...
    //
    const int* countOffsetPair = &_parent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexArray(&_faceChildFaceIndices[countOffsetPair[1]], countOffsetPair[0]);
}
inline VtrIndexArray
VtrRefinement::faceChildFaces(VtrIndex parentFace)
{
    const int* countOffsetPair = &_parent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexArray(&_faceChildFaceIndices[countOffsetPair[1]], countOffsetPair[0]);
}

inline VtrIndexArray const
VtrRefinement::faceChildEdges(VtrIndex parentFace) const
{
    //
    //  Note this *may* need to vary based on the topological split applied...
    //
    const int* countOffsetPair = &_parent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexArray(&_faceChildEdgeIndices[countOffsetPair[1]], countOffsetPair[0]);
}
inline VtrIndexArray
VtrRefinement::faceChildEdges(VtrIndex parentFace)
{
    const int* countOffsetPair = &_parent->mFaceVertCountsAndOffsets[parentFace*2];
    return VtrIndexArray(&_faceChildEdgeIndices[countOffsetPair[1]], countOffsetPair[0]);
}

inline VtrIndexArray const
VtrRefinement::edgeChildEdges(VtrIndex parentEdge) const
{
    return VtrIndexArray(&_edgeChildEdgeIndices[parentEdge*2], 2);
}
inline VtrIndexArray
VtrRefinement::edgeChildEdges(VtrIndex parentEdge)
{
    return VtrIndexArray(&_edgeChildEdgeIndices[parentEdge*2], 2);
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* VTR_REFINEMENT_H */
