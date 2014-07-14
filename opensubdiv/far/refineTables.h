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

///
///  \brief Stores topology data for a specified set of refinement options.
///
class FarRefineTables
{
public:

    //  Local typedef's for local notational convenience:
    typedef FarIndex       Index;
    typedef FarLocalIndex  LocalIndex;

    typedef FarIndexArray      IndexArray;
    typedef FarLocalIndexArray LocalIndexArray;

public:

    /// \brief Constructor
    FarRefineTables(SdcType type, SdcOptions options = SdcOptions());

    /// \brief Destructor
    ~FarRefineTables();

    /// \brief Returns the subdivision scheme
    SdcType    GetSchemeType() const    { return _subdivType; }

    /// \brief Returns the subdivision options
    SdcOptions GetSchemeOptions() const { return _subdivOptions; }

    /// \brief Returns true if uniform subdivision has been applied
    bool IsUniform() const   { return _isUniform; }

    /// \brief Returns the highest level of refinement
    int  GetMaxLevel() const { return _maxLevel; }

    // XXXX barfowl -- should cache these internally for trivial return)

    /// \brief Returns the total number of vertices in all levels
    int GetNumVerticesTotal() const;

    /// \brief Returns the total number of edges in all levels
    int GetNumEdgesTotal() const;

    /// \brief Returns the total number of edges in all levels
    int GetNumFacesTotal() const;

    /// \brief Returns the total number of face vertices in all levels
    int GetNumFaceVerticesTotal() const;


    //
    //  High level refinement and related methods:
    //
    //  XXXX barfowl --  need some variants here for different refinement
    //                   options, i.e. single refine method plus struct
    //                   RefineOptions
    //


    /// \brief Refine the topology uniformly
    ///
    /// @param maxLevel                 Highest level of subdivision refinement
    ///
    /// @param fullTopologyInLastLevel  Skip secondary topological relationships
    ///                                 at the highest level of refinement.
    ///
    void RefineUniform(int maxLevel, bool fullTopologyInLastLevel = false);

    /// \brief Feature Adaptive topology refinement
    ///
    /// @param maxLevel                 Highest level of subdivision refinement
    ///
    /// @param fullTopologyInLastLevel  Skip secondary topological relationships
    ///                                 at the highest level of refinement.
    ///
    void RefineAdaptive(int maxLevel, bool fullTopologyInLastLevel = false);

    /// \brief Unrefine the topology (keep control cage)
    void Unrefine();

    /// \brief Clear the topology entirely
    void Clear();

#ifdef _VTR_COMPUTE_MASK_WEIGHTS_ENABLED
    void ComputeMaskWeights();
#endif


    //
    //  Primvar data interpolation:
    //


    /// \brief Apply vertex and varying interpolation weights to a primvar
    ///        buffer
    ///
    /// The destination buffer must allocate an array of data for all the
    /// refined vertices (at least GetNumVerticesTotal()-GetNumVertices(0))
    ///
    /// @param src  Source primvar buffer (control vertex data)
    ///
    /// @param dst  Destination primvar buffer (refined vertex data)
    ///
    template <class T, class U> void Interpolate(T const * src, U * dst) const;

    /// \brief Apply vertex and varying interpolation weights to a primvar
    ///        buffer for a single level
    /// level of refinement.
    ///
    /// The destination buffer must allocate an array of data for all the
    /// refined vertices (at least GetNumVertices(level))
    ///
    /// @param level  The refinement level
    ///
    /// @param src    Source primvar buffer (control vertex data)
    ///
    /// @param dst    Destination primvar buffer (refined vertex data)
    ///
    template <class T, class U> void Interpolate(int level, T const * src, U * dst) const;


    /// \brief Apply only varying interpolation weights to a primvar buffer
    ///
    /// This method can be a useful alternative if the varying primvar data
    /// does not need to be re-computed over time.
    ///
    /// The destination buffer must allocate an array of data for all the
    /// refined vertices (at least GetNumVerticesTotal()-GetNumVertices(0))
    ///
    /// @param src  Source primvar buffer (control vertex data)
    ///
    /// @param dst  Destination primvar buffer (refined vertex data)
    ///
    template <class T, class U> void InterpolateVarying(T const * src, U * dst) const;

    /// \brief Apply only varying interpolation weights to a primvar buffer
    ///        for a single level level of refinement.
    ///
    /// This method can be a useful alternative if the varying primvar data
    /// does not need to be re-computed over time.
    ///
    /// The destination buffer must allocate an array of data for all the
    /// refined vertices (at least GetNumVertices(level))
    ///
    /// @param level  The refinement level
    ///
    /// @param src    Source primvar buffer (control vertex data)
    ///
    /// @param dst    Destination primvar buffer (refined vertex data)
    ///
    template <class T, class U> void InterpolateVarying(int level, T const * src, U * dst) const;

    //
    //  Inspection of components per level:
    //


    /// \brief Returns the number of vertices at a given level of refinement
    int GetNumVertices(int level) const {
        return _levels[level].getNumVertices();
    }

    /// \brief Returns the number of edges at a given level of refinement
    int GetNumEdges(int level) const {
        return _levels[level].getNumEdges();
    }

    /// \brief Returns the number of face vertex indices at a given level of refinement
    int GetNumFaces(int level) const {
        return _levels[level].getNumFaces();
    }
    
    /// \brief Returns the number of faces at a given level of refinement
    int GetNumFaceVertices(int level) const {
        return _levels[level].getNumFaceVerticesTotal();
    }

    /// \brief Returns the sharpness of a given edge (at 'level' of refinement)
    float GetEdgeSharpness(int level, Index edge) const {
        return _levels[level].getEdgeSharpness(edge);
    }

    /// \brief Returns the sharpness of a given vertex (at 'level' of refinement)
    float GetVertexSharpness(int level, Index vert) const {
        return _levels[level].getVertexSharpness(vert);
    }

    /// \brief Returns the subdivision rule of a given vertex (at 'level' of refinement)
    SdcRule GetVertexRule(int level, Index vert) const {
        return _levels[level].getVertexRule(vert);
    }


    //
    //  Topological relations -- incident/adjacent components:
    //


    /// \brief Returns the vertices of a 'face' at 'level'
    IndexArray const GetFaceVertices(int level, Index face) const {
        return _levels[level].getFaceVertices(face);
    }

    /// \brief Returns the edges of a 'face' at 'level'
    IndexArray const GetFaceEdges(   int level, Index face) const {
        return _levels[level].getFaceEdges(face);
    }

    /// \brief Returns the vertices of an 'edge' at 'level' (2 of them)
    IndexArray const GetEdgeVertices(int level, Index edge) const {
        return _levels[level].getEdgeVertices(edge);
    }

    /// \brief Returns the faces incident to 'edge' at 'level'
    IndexArray const GetEdgeFaces(   int level, Index edge) const {
        return _levels[level].getEdgeFaces(edge);
    }

    /// \brief Returns the faces incident to 'vertex' at 'level'
    IndexArray const GetVertexFaces( int level, Index vert) const {
        return _levels[level].getVertexFaces(vert);
    }

    /// \brief Returns the edges incident to 'vertex' at 'level'
    IndexArray const GetVertexEdges( int level, Index vert) const {
        return _levels[level].getVertexEdges(vert);
    }

    /// \brief Returns the local face indices of vertex 'vert' at 'level'
    LocalIndexArray const VertexFaceLocalIndices(int level, Index vert) const {
        return _levels[level].getVertexFaceLocalIndices(vert);
    }

    /// \brief Returns the local edge indices of vertex 'vert' at 'level'
    LocalIndexArray const VertexEdgeLocalIndices(int level, Index vert) const {
        return _levels[level].getVertexEdgeLocalIndices(vert);
    }

    /// \brief Returns the edge with vertices'v0' and 'v1' (or -1 if they are
    ///  not connected)
    Index FindEdge(int level, Index v0, Index v1) const {
        return _levels[level].findEdge(v0, v1);
    }


    //
    //  Parent-to-child relationships, i.e. relationships between components in one level
    //  and the next (entries may be invalid if sparse):
    //


    /// \brief Returns the child faces of face 'f' at 'level'
    IndexArray const GetFaceChildFaces(int level, Index f) const {
        return _refinements[level].getFaceChildFaces(f);
    }

    /// \brief Returns the child edges of face 'f' at 'level'
    IndexArray const GetFaceChildEdges(int level, Index f) const {
        return _refinements[level].getFaceChildEdges(f);
    }

    /// \brief Returns the child edges of edge 'e' at 'level'
    IndexArray const GetEdgeChildEdges(int level, Index e) const {
        return _refinements[level].getEdgeChildEdges(e);
    }

    /// \brief Returns the child vertex of face 'f' at 'level'
    Index GetFaceChildVertex(  int level, Index f) const {
        return _refinements[level].getFaceChildVertex(f);
    }

    /// \brief Returns the child vertex of edge 'e' at 'level'
    Index GetEdgeChildVertex(  int level, Index e) const {
        return _refinements[level].getEdgeChildVertex(e);
    }

    /// \brief Returns the child vertex of vertex 'v' at 'level'
    Index GetVertexChildVertex(int level, Index v) const {
        return _refinements[level].getVertexChildVertex(v);
    }


    //
    //  Debugging aides:
    //


    /// \brief Returns true if the topology of 'level' is valid
    bool ValidateTopology(int level) const {
        return _levels[level].validateTopology();
    }

    /// \brief Prints topology information to console
    void PrintTopology(int level, bool children = true) const {
        _levels[level].print(children ? &_refinements[level] : 0);
    }


protected:

    //
    //  For use by the Factory base and subclasses to construct the base level:
    //
    template <class MESH>
    friend class FarRefineTablesFactory;
    friend class FarRefineTablesFactoryBase;
    friend class FarPatchTablesFactory;

    int                   getNumLevels() const { return (int)_levels.size(); }
    VtrLevel            & getBaseLevel() { return _levels.front(); }
    VtrLevel            & getLevel(int l) { return _levels[l]; }
    VtrLevel const      & getLevel(int l) const { return _levels[l]; }
    VtrRefinement const & getRefinement(int l) const { return _refinements[l]; }

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

    template <class T, class U> void interpolateChildVertsFromFaces(VtrRefinement const &, T const * src, U * dst) const;
    template <class T, class U> void interpolateChildVertsFromEdges(VtrRefinement const &, T const * src, U * dst) const;
    template <class T, class U> void interpolateChildVertsFromVerts(VtrRefinement const &, T const * src, U * dst) const;

    template <class T, class U> void varyingInterpolateChildVertsFromFaces(VtrRefinement const &, T const * src, U * dst) const;
    template <class T, class U> void varyingInterpolateChildVertsFromEdges(VtrRefinement const &, T const * src, U * dst) const;
    template <class T, class U> void varyingInterpolateChildVertsFromVerts(VtrRefinement const &, T const * src, U * dst) const;

private:
    //  The following should be private but leaving it open while still early...
    SdcType    _subdivType;
    SdcOptions _subdivOptions;

    bool _isUniform;
    int  _maxLevel;

    std::vector<VtrLevel>      _levels;
    std::vector<VtrRefinement> _refinements;
};

template <class T, class U>
inline void
FarRefineTables::Interpolate(T const * src, U * dst) const {

    assert(_subdivType == TYPE_CATMARK);

    for (int level=1; level<=GetMaxLevel(); ++level) {

        Interpolate(level, src, dst);

        src = dst;
        dst += GetNumVertices(level);
    }
}

template <class T, class U>
inline void
FarRefineTables::Interpolate(int level, T const * src, U * dst) const {

    assert(level>0 and level<=(int)_refinements.size());

    VtrRefinement const & refinement = _refinements[level-1];

    interpolateChildVertsFromFaces(refinement, src, dst);
    interpolateChildVertsFromEdges(refinement, src, dst);
    interpolateChildVertsFromVerts(refinement, src, dst);
}

template <class T, class U>
inline void
FarRefineTables::interpolateChildVertsFromFaces(
    VtrRefinement const & refinement, T const * src, U * dst) const {

    SdcScheme<TYPE_CATMARK> scheme(_subdivOptions);

    const VtrLevel& parent = refinement.parent();

    for (int face = 0; face < parent.getNumFaces(); ++face) {

        VtrIndex cVert = refinement.getFaceChildVertex(face);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent face:
        VtrIndexArray const fVerts = parent.getFaceVertices(face);

        float fVertWeights[fVerts.size()],
              fVaryingWeight = 1.0f / (float) fVerts.size();

        VtrMaskInterface fMask(fVertWeights, 0, 0);
        VtrFaceInterface fHood(fVerts.size());

        scheme.ComputeFaceVertexMask(fHood, fMask);

        //  Apply the weights to the parent face's vertices:
        U & vdst = dst[cVert];

        vdst.Clear();

        for (int i = 0; i < fVerts.size(); ++i) {

            vdst.AddWithWeight(src[fVerts[i]], fVertWeights[i]);

            vdst.AddVaryingWithWeight(src[fVerts[i]], fVaryingWeight);
        }
    }
}

template <class T, class U>
inline void
FarRefineTables::interpolateChildVertsFromEdges(
    VtrRefinement const & refinement, T const * src, U * dst) const {

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
        U & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[eVerts[0]], eVertWeights[0]);
        vdst.AddWithWeight(src[eVerts[1]], eVertWeights[1]);

        vdst.AddVaryingWithWeight(src[eVerts[0]], 0.5f);
        vdst.AddVaryingWithWeight(src[eVerts[1]], 0.5f);

        if (eMask.GetNumFaceWeights() > 0) {

            for (int i = 0; i < eFaces.size(); ++i) {

                VtrIndex cVertOfFace = refinement.getFaceChildVertex(eFaces[i]);
                assert(VtrIndexIsValid(cVertOfFace));
                vdst.AddWithWeight(dst[cVertOfFace], eFaceWeights[i]);
            }
        }
    }
}

template <class T, class U>
inline void
FarRefineTables::interpolateChildVertsFromVerts(
    VtrRefinement const & refinement, T const * src, U * dst) const {

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
        U & vdst = dst[cVert];

        vdst.Clear();
        vdst.AddWithWeight(src[vert], vVertWeight);
        vdst.AddVaryingWithWeight(src[vert], 1.0f);

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

//
// Varying only interpolation
//

template <class T, class U>
inline void
FarRefineTables::InterpolateVarying(T const * src, U * dst) const {

    assert(_subdivType == TYPE_CATMARK);

    for (int level=1; level<=GetMaxLevel(); ++level) {

        InterpolateVarying(level, src, dst);

        src = dst;
        dst += GetNumVertices(level);
    }
}

template <class T, class U>
inline void
FarRefineTables::InterpolateVarying(int level, T const * src, U * dst) const {

    assert(level>0 and level<=(int)_refinements.size());

    VtrRefinement const & refinement = _refinements[level-1];

    varyingInterpolateChildVertsFromFaces(refinement, src, dst);
    varyingInterpolateChildVertsFromEdges(refinement, src, dst);
    varyingInterpolateChildVertsFromVerts(refinement, src, dst);
}

template <class T, class U>
inline void
FarRefineTables::varyingInterpolateChildVertsFromFaces(
    VtrRefinement const & refinement, T const * src, U * dst) const {

    const VtrLevel& parent = refinement.parent();

    for (int face = 0; face < parent.getNumFaces(); ++face) {

        VtrIndex cVert = refinement.getFaceChildVertex(face);
        if (!VtrIndexIsValid(cVert))
            continue;

        VtrIndexArray const fVerts = parent.getFaceVertices(face);

        float fVaryingWeight = 1.0f / (float) fVerts.size();

        //  Apply the weights to the parent face's vertices:
        U & vdst = dst[cVert];

        vdst.Clear();

        for (int i = 0; i < fVerts.size(); ++i) {
            vdst.AddVaryingWithWeight(src[fVerts[i]], fVaryingWeight);
        }
    }
}

template <class T, class U>
inline void
FarRefineTables::varyingInterpolateChildVertsFromEdges(
    VtrRefinement const & refinement, T const * src, U * dst) const {

    assert(_subdivType == TYPE_CATMARK);

    const VtrLevel& parent = refinement.parent();

    for (int edge = 0; edge < parent.getNumEdges(); ++edge) {

        VtrIndex cVert = refinement.getEdgeChildVertex(edge);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Declare and compute mask weights for this vertex relative to its parent edge:
        VtrIndexArray const eVerts = parent.getEdgeVertices(edge);

        //  Apply the weights to the parent edges's vertices
        U & vdst = dst[cVert];

        vdst.Clear();

        vdst.AddVaryingWithWeight(src[eVerts[0]], 0.5f);
        vdst.AddVaryingWithWeight(src[eVerts[1]], 0.5f);
    }
}

template <class T, class U>
inline void
FarRefineTables::varyingInterpolateChildVertsFromVerts(
    VtrRefinement const & refinement, T const * src, U * dst) const {

    assert(_subdivType == TYPE_CATMARK);

    const VtrLevel& parent = refinement.parent();

    for (int vert = 0; vert < parent.getNumVertices(); ++vert) {

        VtrIndex cVert = refinement.getVertexChildVertex(vert);
        if (!VtrIndexIsValid(cVert))
            continue;

        //  Apply the weights to the parent vertex
        U & vdst = dst[cVert];

        vdst.Clear();
    }
}




} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* FAR_REFINE_TABLES_H */
