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
//  Typedef's for indices we want at the Far level -- need to go elsewhere eventually...
//
typedef VtrIndex       FarIndex;
typedef VtrLocalIndex  FarLocalIndex;

typedef VtrIndexArray      FarIndexArray;
typedef VtrLocalIndexArray FarLocalIndexArray;

//
//  Class to store topology data for a specified set of refinement options.
//
class FarRefineTables
{
public:
    //  Local typedef's for local notational convenience:
    typedef FarIndex       Index;
    typedef FarLocalIndex  LocalIndex;

    typedef FarIndexArray      IndexArray;
    typedef FarLocalIndexArray LocalIndexArray;

public:
    FarRefineTables(SdcType type, SdcOptions options = SdcOptions());
    ~FarRefineTables();

    //
    //  Queries for the instance as a whole:
    //
    SdcType    GetSchemeType() const    { return _subdivType; }
    SdcOptions GetSchemeOptions() const { return _subdivOptions; }

    bool IsUniform() const   { return _isUniform; }
    int  GetMaxLevel() const { return _maxLevel; }

    //  (Note to self -- should cache these internally for trivial return)
    int GetNumVerticesTotal() const;
    int GetNumEdgesTotal() const;
    int GetNumFacesTotal() const;

    //
    //  High level refinement and related methods:
    //      - need some variants here for different refinement options, i.e.
    //        single refine method plus struct RefineOptions
    //
    void RefineUniform(int maxLevel, bool fullTopologyInLastLevel = true);
    void RefineAdaptive(int maxLevel, bool fullTopologyInLastLevel = true);
    void Unrefine();
    void Clear();
#ifdef _VTR_COMPUTE_MASK_WEIGHTS_ENABLED
    void ComputeMaskWeights();
#endif
    template <class T> void Interpolate(                T const * src, T * dst) const;
    template <class T> void InterpolateLevel(int level, T const * src, T * dst) const;

    //
    //  Inspection of components per level:
    //
    //  Component inventories:
    int GetNumVertices(int level) const { return _levels[level].getNumVertices(); }
    int GetNumEdges(   int level) const { return _levels[level].getNumEdges(); }
    int GetNumFaces(   int level) const { return _levels[level].getNumFaces(); }

    //  Component properties:
    float   GetEdgeSharpness(  int level, Index edge) const { return _levels[level].getEdgeSharpness(edge); }
    float   GetVertexSharpness(int level, Index vert) const { return _levels[level].getVertexSharpness(vert); }
    SdcRule GetVertexRule(     int level, Index vert) const { return _levels[level].getVertexRule(vert); }

    //  Topological relations -- incident/adjacent components:
    IndexArray const GetFaceVertices(int level, Index face) const { return _levels[level].getFaceVertices(face); }
    IndexArray const GetFaceEdges(   int level, Index face) const { return _levels[level].getFaceEdges(face); }
    IndexArray const GetEdgeVertices(int level, Index edge) const { return _levels[level].getEdgeVertices(edge); }
    IndexArray const GetEdgeFaces(   int level, Index edge) const { return _levels[level].getEdgeFaces(edge); }
    IndexArray const GetVertexFaces( int level, Index vert) const { return _levels[level].getVertexFaces(vert); }
    IndexArray const GetVertexEdges( int level, Index vert) const { return _levels[level].getVertexEdges(vert); }

    //      ... and do we want to include these with the above?
    //  LocalIndexArray const VertexFaceLocalIndices(int level, Index vert) const;
    //  LocalIndexArray const VertexEdgeLocalIndices(int level, Index vert) const;

    //  Other topological queries:
    Index FindEdge(int level, Index v0, Index v1) const { return _levels[level].findEdge(v0, v1); }

    //  Parent-to-child relationships, i.e. relationships between components in one level
    //  and the next (entries may be invalid if sparse):
    IndexArray const GetFaceChildFaces(int level, Index f) const { return _refinements[level].getFaceChildFaces(f); }
    IndexArray const GetFaceChildEdges(int level, Index f) const { return _refinements[level].getFaceChildEdges(f); }
    IndexArray const GetEdgeChildEdges(int level, Index e) const { return _refinements[level].getEdgeChildEdges(e); }

    Index GetFaceChildVertex(  int level, Index f) const { return _refinements[level].getFaceChildVertex(f); }
    Index GetEdgeChildVertex(  int level, Index e) const { return _refinements[level].getEdgeChildVertex(e); }
    Index GetVertexChildVertex(int level, Index v) const { return _refinements[level].getVertexChildVertex(v); }

    //  Debugging aides:
    bool ValidateTopology(int level) const { return _levels[level].validateTopology(); }
    void PrintTopology(int level, bool children = true) const { _levels[level].print(children ? &_refinements[level] : 0); }


public:
    //  Want to be protect or remove these entirely:
    VtrLevel&       GetLevel(int l)       { return _levels[l]; }
    VtrLevel const& GetLevel(int l) const { return _levels[l]; }

    VtrRefinement& GetRefinement(int l) { return _refinements[l]; }

protected:
    //
    //  For use by the Factory base and subclasses to construct the base level:
    //
    template <class MESH>
    friend class FarRefineTablesFactory;
    friend class FarRefineTablesFactoryBase;

    //  This really should not be needed by the Factory ultimately...
    VtrLevel& getBaseLevel() { return _levels.front(); }

    int getNumBaseFaces() const    { return GetNumFaces(0); }
    int getNumBaseEdges() const    { return GetNumEdges(0); }
    int getNumBaseVertices() const { return GetNumVertices(0); }

    //  Sizing specifications required before allocation:
    void setNumBaseFaces(   int count) { _levels[0].resizeFaces(count); }
    void setNumBaseEdges(   int count) { _levels[0].resizeEdges(count); }
    void setNumBaseVertices(int count) { _levels[0].resizeVertices(count); }

    void setNumBaseFaceVertices(Index f, int count) { _levels[0].resizeFaceVertices(f, count); }
    void setNumBaseEdgeFaces(   Index e, int count) { _levels[0].resizeEdgeFaces(e, count); }
    void setNumBaseVertexFaces( Index v, int count) { _levels[0].resizeVertexFaces(v, count); }
    void setNumBaseVertexEdges( Index v, int count) { _levels[0].resizeVertexEdges(v, count); }

    //  Access to populate the base level topology after allocation:
    IndexArray setBaseFaceVertices(Index f) { return _levels[0].getFaceVertices(f); }
    IndexArray setBaseFaceEdges(   Index f) { return _levels[0].getFaceEdges(f); }
    IndexArray setBaseEdgeVertices(Index e) { return _levels[0].getEdgeVertices(e); }
    IndexArray setBaseEdgeFaces(   Index e) { return _levels[0].getEdgeFaces(e); }
    IndexArray setBaseVertexFaces( Index v) { return _levels[0].getVertexFaces(v); }
    IndexArray setBaseVertexEdges( Index v) { return _levels[0].getVertexEdges(v); }

    //  Not sure yet if we will determine these internally...
    LocalIndexArray setBaseVertexFaceLocalIndices(Index v) { return _levels[0].getVertexFaceLocalIndices(v); }
    LocalIndexArray setBaseVertexEdgeLocalIndices(Index v) { return _levels[0].getVertexEdgeLocalIndices(v); }

    //  Optionally available to get/set sharpness values:
    float& baseEdgeSharpness(Index e)   { return _levels[0].getEdgeSharpness(e); }
    float& baseVertexSharpness(Index v) { return _levels[0].getVertexSharpness(v); }

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
        dst += GetNumVertices(level);
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

    for (int face = 0; face < parent.getNumFaces(); ++face) {

        VtrIndex cVert = refinement.getFaceChildVertex(face);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent face:
        VtrIndexArray const fVerts = parent.getFaceVertices(face);

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

    for (int edge = 0; edge < parent.getNumEdges(); ++edge) {

        VtrIndex cVert = refinement.getEdgeChildVertex(edge);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent edge:
        VtrIndexArray const eVerts = parent.getEdgeVertices(edge);
        VtrIndexArray const eFaces = parent.getEdgeFaces(edge);

        float eVertWeights[2];
        float eFaceWeights[eFaces.size()];

        VtrMaskInterface eMask(eVertWeights, 0, eFaceWeights);

        eHood.SetIndex(edge);

        SdcRule pRule = (parent.getEdgeSharpness(edge) > 0.0) ? SdcCrease::RULE_CREASE : SdcCrease::RULE_SMOOTH;
        SdcRule cRule = child.getVertexRule(cVert);

        scheme.ComputeEdgeVertexMask(eHood, eMask, pRule, cRule);

        //  Apply the weights to the parent edges's vertices and (if applicable) to
        //  the child vertices of its incident faces:
        T & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[eVerts[0]], eVertWeights[0]);
        vdst.AddWithWeight(src[eVerts[1]], eVertWeights[1]);

        if (eMask.GetNumFaceWeights() > 0) {

            for (int i = 0; i < eFaces.size(); ++i) {

                VtrIndex cVertOfFace = refinement.getFaceChildVertex(eFaces[i]);
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

    for (int vert = 0; vert < parent.getNumVertices(); ++vert) {

        VtrIndex cVert = refinement.getVertexChildVertex(vert);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent edge:
        VtrIndexArray const vEdges = parent.getVertexEdges(vert);
        VtrIndexArray const vFaces = parent.getVertexFaces(vert);

        float  vVertWeight;
        float  vEdgeWeights[2 * vEdges.size()];
        float* vFaceWeights = vEdgeWeights + vEdges.size();

        VtrMaskInterface vMask(&vVertWeight, vEdgeWeights, vFaceWeights);

        vHood.SetIndex(vert, cVert);

        SdcRule pRule = parent.getVertexRule(vert);
        SdcRule cRule = child.getVertexRule(cVert);

        scheme.ComputeVertexVertexMask(vHood, vMask, pRule, cRule);

        //  Apply the weights to the parent vertex, the vertices opposite its incident
        //  edges, and the child vertices of its incident faces:
        T & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[vert], vVertWeight);

        if (vMask.GetNumEdgeWeights() > 0) {

            for (int i = 0; i < vEdges.size(); ++i) {

                VtrIndexArray const eVerts = parent.getEdgeVertices(vEdges[i]);
                VtrIndex pVertOppositeEdge = (eVerts[0] == vert) ? eVerts[1] : eVerts[0];

                vdst.AddWithWeight(src[pVertOppositeEdge], vEdgeWeights[i]);
            }
        }
        if (vMask.GetNumFaceWeights() > 0) {

            for (int i = 0; i < vFaces.size(); ++i) {

                VtrIndex cVertOfFace = refinement.getFaceChildVertex(vFaces[i]);
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
