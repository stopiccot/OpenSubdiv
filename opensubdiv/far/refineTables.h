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
#ifndef FAR_REFINE_TABLES_H
#define FAR_REFINE_TABLES_H

#include "../version.h"

#include "../sdc/type.h"
#include "../sdc/options.h"
#include "../sdc/catmarkScheme.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"
#include "../vtr/maskInterfaces.h"

#include <vector>
#include <cassert>
#include <cstdio>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

template <class MESH> class FarRefineTablesFactory;
class VtrSparseSelector;

//
//  Class to store topology data for a specified set of refinement options.
//
class FarRefineTables
{
public:
    typedef VtrIndex       Index;
    typedef VtrLocalIndex  LocalIndex;

    typedef VtrIndexArray      IndexArray;
    typedef VtrLocalIndexArray LocalIndexArray;

public:
    FarRefineTables(SdcType type, SdcOptions options = SdcOptions());
    ~FarRefineTables();

    //  Accessors:
    SdcType    GetSchemeType() const    { return _subdivType; }
    SdcOptions GetSchemeOptions() const { return _subdivOptions; }

    bool IsUniform() const   { return _isUniform; }
    int  GetMaxLevel() const { return _maxLevel; }

    //
    //  The "counts" return the number of components at a specific level or for the sum
    //  of all levels:
    //
    //  Should cache these results as members for trivial return...
    int GetVertCount() const;
    int GetEdgeCount() const;
    int GetFaceCount() const;

    int GetVertCount(int level) const { return _levels[level].vertCount(); }
    int GetEdgeCount(int level) const { return _levels[level].edgeCount(); }
    int GetFaceCount(int level) const { return _levels[level].faceCount(); }

    //
    //  Main refinement method(s) -- we will need some variants here to support
    //  different refinement options, i.e. eventually struct RefineOptions:
    //
    void RefineUniform(int maxLevel, bool fullTopologyInLastLevel = true);
    void RefineAdaptive(int maxLevel, bool fullTopologyInLastLevel = true);

    //
    //  A few public utilities for consideration:
    //      - clear all refinements, leaving the base level
    //      - clear all refinements and all levels
    //      - computing the mask weights
    //
    void Unrefine();
    void Clear();
#ifdef _VTR_COMPUTE_MASK_WEIGHTS_ENABLED
    void ComputeMaskWeights();
#endif

    //  Level access for converting to/from base/last levels (ultimately these
    //  need to be protected and the required functionality provided through an
    //  extended public interface)
    VtrLevel& GetLevel(int l) { return _levels[l]; }

    VtrLevel& GetBaseLevel() { return _levels.front(); }

    VtrLevel& GetLastLevel() { return _levels.back(); }

    VtrRefinement& GetRefinement(int l) { return _refinements[l]; }

    template <class T> void Interpolate(T const * src, T * dst) const;

    template <class T> void InterpolateLevel(int level, T const * src, T * dst) const;

protected:

    //  For use by the Factory base and subclasses to construct the base level:
    template <class MESH> friend class FarRefineTablesFactory;

    //  Sizing specifications required before allocation:
    void setBaseFaceCount(  int count) { _levels[0].resizeFaces(count); }
    void setBaseEdgeCount(  int count) { _levels[0].resizeEdges(count); }
    void setBaseVertexCount(int count) { _levels[0].resizeVerts(count); }

    void setBaseFaceVertexCount(Index f, int count) { _levels[0].resizeFaceVerts(f, count); }
    void setBaseEdgeFaceCount(  Index e, int count) { _levels[0].resizeEdgeFaces(e, count); }
    void setBaseVertexFaceCount(Index v, int count) { _levels[0].resizeVertFaces(v, count); }
    void setBaseVertexEdgeCount(Index v, int count) { _levels[0].resizeVertEdges(v, count); }

    //  Access to populate the base level topology after allocation:
    int        getBaseFaceCount() const  { return GetFaceCount(0); }
    IndexArray baseFaceVertices(Index f) { return _levels[0].modifyFaceVerts(f); }
    IndexArray baseFaceEdges(   Index f) { return _levels[0].modifyFaceEdges(f); }

    int        getBaseEdgeCount() const  { return GetEdgeCount(0); }
    IndexArray baseEdgeVertices(Index e) { return _levels[0].modifyEdgeVerts(e); }
    IndexArray baseEdgeFaces(   Index e) { return _levels[0].modifyEdgeFaces(e); }

    int        getBaseVertexCount() const { return GetVertCount(0); }
    IndexArray baseVertexFaces(Index v)   { return _levels[0].modifyVertFaces(v); }
    IndexArray baseVertexEdges(Index v)   { return _levels[0].modifyVertEdges(v); }

    //  Not sure yet if we will determine these internally...
    LocalIndexArray baseVertexFaceLocalIndices(Index v) { return _levels[0].modifyVertFaceLocalIndices(v); }
    LocalIndexArray baseVertexEdgeLocalIndices(Index v) { return _levels[0].modifyVertEdgeLocalIndices(v); }

    //  Optionally available to get/set sharpness values:
    float& baseEdgeSharpness(Index e)   { return _levels[0].edgeSharpness(e); }
    float& baseVertexSharpness(Index v) { return _levels[0].vertSharpness(v); }

private:
    //  Prototype -- mainly for illustrative purposes right now...
    void catmarkFeatureAdaptiveSelector(VtrSparseSelector& selector);
    void catmarkFeatureAdaptiveSelectorByFace(VtrSparseSelector& selector);

    template <class T> void interpolateChildVertsFromFaces(VtrRefinement const &, T const * src, T * dst) const;

    template <class T> void interpolateChildVertsFromEdges(VtrRefinement const &, T const * src, T * dst) const;

    template <class T> void interpolateChildVertsFromVerts(VtrRefinement const &, T const * src, T * dst) const;

//  The following should be private but leaving it open while still early...
public:
    SdcType    _subdivType;
    SdcOptions _subdivOptions;

    bool _isUniform;
    int  _maxLevel;

    std::vector<VtrLevel>      _levels;
    std::vector<VtrRefinement> _refinements;
};

template <class T>
inline void
FarRefineTables::Interpolate(T const * src, T * dst) const {

    assert(_subdivType == TYPE_CATMARK);

    for (int level=1; level<=GetMaxLevel(); ++level) {

        InterpolateLevel(level, src, dst);
        
        src = dst;
        dst += GetVertCount(level);
    }
}

template <class T>
inline void
FarRefineTables::InterpolateLevel(int level, T const * src, T * dst) const {

    assert(level>0 and level<=(int)_refinements.size());

    VtrRefinement const & refinement = _refinements[level-1];

    interpolateChildVertsFromFaces(refinement, src, dst);
    interpolateChildVertsFromEdges(refinement, src, dst);
    interpolateChildVertsFromVerts(refinement, src, dst);
}

template <class T>
inline void
FarRefineTables::interpolateChildVertsFromFaces(VtrRefinement const & refinement, T const * src, T * dst) const {

    SdcScheme<TYPE_CATMARK> scheme(_subdivOptions);

    const VtrLevel& parent = refinement.parent();

    for (int face = 0; face < parent.faceCount(); ++face) {

        VtrIndex cVert = refinement.faceChildVertexIndex(face);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent face:
        VtrIndexArray const fVerts = parent.accessFaceVerts(face);

        float fVertWeights[fVerts.size()];

        VtrMaskInterface fMask(fVertWeights, 0, 0);
        VtrFaceInterface fHood(fVerts.size());

        scheme.ComputeFaceVertexMask(fHood, fMask);

        //  Apply the weights to the parent face's vertices:
        T & vdst = dst[cVert];

        vdst.Clear();

        for (int i = 0; i < fVerts.size(); ++i) {
            vdst.AddWithWeight(src[fVerts[i]], fVertWeights[i]);
            //vdst.AddVaryingWithWeight(src[fVerts[i]], fVertWeights[i]);
        }
    }
}

template <class T>
inline void
FarRefineTables::interpolateChildVertsFromEdges(VtrRefinement const & refinement, T const * src, T * dst) const {

    assert(_subdivType == TYPE_CATMARK);
    SdcScheme<TYPE_CATMARK> scheme(_subdivOptions);

    const VtrLevel& parent = refinement.parent();
    const VtrLevel& child  = refinement.child();

    VtrEdgeInterface eHood(parent);

    for (int edge = 0; edge < parent.edgeCount(); ++edge) {

        VtrIndex cVert = refinement.edgeChildVertexIndex(edge);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent edge:
        VtrIndexArray const eVerts = parent.accessEdgeVerts(edge);
        VtrIndexArray const eFaces = parent.accessEdgeFaces(edge);

        float eVertWeights[2];
        float eFaceWeights[eFaces.size()];

        VtrMaskInterface eMask(eVertWeights, 0, eFaceWeights);

        eHood.SetIndex(edge);

        SdcRule pRule = (parent.edgeSharpness(edge) > 0.0) ? SdcCrease::RULE_CREASE : SdcCrease::RULE_SMOOTH;
        SdcRule cRule = child.vertRule(cVert);

        scheme.ComputeEdgeVertexMask(eHood, eMask, pRule, cRule);

        //  Apply the weights to the parent edges's vertices and (if applicable) to
        //  the child vertices of its incident faces:
        T & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[eVerts[0]], eVertWeights[0]);
        vdst.AddWithWeight(src[eVerts[1]], eVertWeights[1]);

        if (eMask.GetFaceWeightCount() > 0) {

            for (int i = 0; i < eFaces.size(); ++i) {

                VtrIndex cVertOfFace = refinement.faceChildVertexIndex(eFaces[i]);
                assert(VtrIndexIsValid(cVertOfFace));
                vdst.AddWithWeight(dst[cVertOfFace], eFaceWeights[i]);
            }
        }
    }
}

template <class T>
inline void
FarRefineTables::interpolateChildVertsFromVerts(VtrRefinement const & refinement, T const * src, T * dst) const {

    assert(_subdivType == TYPE_CATMARK);
    SdcScheme<TYPE_CATMARK> scheme(_subdivOptions);

    const VtrLevel& parent = refinement.parent();
    const VtrLevel& child  = refinement.child();

    VtrVertexInterface vHood(parent, child);

    for (int vert = 0; vert < parent.vertCount(); ++vert) {

        VtrIndex cVert = refinement.vertexChildVertexIndex(vert);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent edge:
        VtrIndexArray const vEdges = parent.accessVertEdges(vert);
        VtrIndexArray const vFaces = parent.accessVertFaces(vert);

        float  vVertWeight;
        float  vEdgeWeights[2 * vEdges.size()];
        float* vFaceWeights = vEdgeWeights + vEdges.size();

        VtrMaskInterface vMask(&vVertWeight, vEdgeWeights, vFaceWeights);

        vHood.SetIndex(vert, cVert);

        SdcRule pRule = parent.vertRule(vert);
        SdcRule cRule = child.vertRule(cVert);

        scheme.ComputeVertexVertexMask(vHood, vMask, pRule, cRule);

        //  Apply the weights to the parent vertex, the vertices opposite its incident
        //  edges, and the child vertices of its incident faces:
        T & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[vert], vVertWeight);

        if (vMask.GetEdgeWeightCount() > 0) {

            for (int i = 0; i < vEdges.size(); ++i) {

                VtrIndexArray const eVerts = parent.accessEdgeVerts(vEdges[i]);
                VtrIndex pVertOppositeEdge = (eVerts[0] == vert) ? eVerts[1] : eVerts[0];

                vdst.AddWithWeight(src[pVertOppositeEdge], vEdgeWeights[i]);
            }
        }
        if (vMask.GetFaceWeightCount() > 0) {

            for (int i = 0; i < vFaces.size(); ++i) {

                VtrIndex cVertOfFace = refinement.faceChildVertexIndex(vFaces[i]);
                assert(VtrIndexIsValid(cVertOfFace));
                vdst.AddWithWeight(dst[cVertOfFace], vFaceWeights[i]);
            }
        }
    }
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* FAR_REFINE_TABLES_H */
