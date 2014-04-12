// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

//  Excluding a whole bunch of stuff that is distracting to the examples...

//
//  These examples are taken from some test code for Vtr:
//      Between each pair of VtrLevels (a parent and child) there is a
//  VtrRefinement that relates the data between the two.  The refinement
//  has options associated with it -- some of which will affect the
//  stencil or creasing computations.
//      Each example below iterates through the components of each type
//  in the parent level to see if it spawned a child vertex.  For those
//  that do, the child vertex is identified and its "position" computed
//  from stencils obtained from SdcScheme<CATMARK>.  The appropriate
//  topological Neighborhood is identified for the parent component and
//  since VtrLevels have their vertices classified, i.e. the Rule for
//  each is known, we use those to accelerate the stencil queries (as
//  Hbr and Far would too, as they also know the vertex Rule/mask).
//

//
//  Example to compute and apply stencils for face-vertices:
//
inline void
interpolateChildVertsFromFaces(VtrRefinement const&  refinement,
                               PositionVector const& parentPos,
                               PositionVector&       childPos)
{
    SdcScheme<CATMARK> scheme(refinement.GetOptions());

    SdcFaceNeighborhood  fHood;
    SdcStencil<float>    fStencil;

    VtrLevel const& parent = *refinement.mParent;

    for (int pFace = 0; pFace < parent.faceCount(); ++pFace) {
        VtrIndex cVert = refinement.mFaceChildVertIndex[pFace];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrIndexAccessor fVerts      = parent.accessFaceVerts(pFace);
        int              fVertCount  = fVerts.size();
        float            fVertWeights[fVertCount];

        fHood.SetVertexCount(fVertCount);
        fStencil.SetVertexWeights(fVertWeights);

        scheme.ComputeFaceVertexStencil(fHood, fStencil);
        assert(fStencil.GetVertexWeightCount() == fVertCount);

        Position& dstPos = childPos[cVert];
        dstPos = fVertWeights[0] * parentPos[fVerts[0]];
        for (int j = 1; j < fVertCount; ++j) {
            dstPos += fVertWeights[j] * parentPos[fVerts[j]];
        }
    }
}


//
//  Example to compute and apply stencils for edge-vertices:
//
inline void
interpolateChildVertsFromEdges(VtrRefinement const&  refinement,
                               PositionVector const& parentPos,
                               PositionVector&       childPos)
{
    SdcOptions         options = refinement.GetOptions();  
    SdcScheme<CATMARK> scheme(options);

    SdcEdgeNeighborhood  eHood;
    SdcStencil<float>    eStencil;

    VtrLevel const& parent = *refinement.mParent;
    VtrLevel const& child  = *refinement.mChild;

    bool useNewTriangleRule = (parent.depth() == 0) &&
            (options.GetTriangleSubdivision() != SdcOptions::TRI_SUB_NORMAL);

    //
    //  Vertices from parent edges -- combination of edge verts and adjacent face
    //  midpoints (computed previously):
    //
    for (int pEdge = 0; pEdge < parent.edgeCount(); ++pEdge) {
        VtrIndex cVert = refinement.mEdgeChildVertIndex[pEdge];
        if (!VtrIndexIsValid(cVert)) continue;

        VtrIndexAccessor eVerts = parent.accessEdgeVerts(pEdge);
        VtrIndexAccessor eFaces = parent.accessEdgeFaces(pEdge);

        //
        //  Initialize the Neighborhood -- remember we may need to specify face valence:
        //
        float eSharpness = parent.edgeSharpness(pEdge);
        int   eFaceCount = eFaces.size();
        int   eFaceValence[eFaceCount];

        eHood.SetSharpness(eSharpness);
        eHood.SetFaceCount(eFaceCount);
        if (useNewTriangleRule) {
            eHood.SetFaceValence(eFaceValence);
        }

        //
        //  Initialize and compute the Stencil, using the Rule previously computed to
        //  help (and remember all weights, including vertex weights, must be specified
        //  externally -- we could do this once outside the loop for high valence and
        //  only modify the Stencil when we need more (potentially from the heap))
        //
        float eVertWeights[2];
        float eFaceWeights[eFaceCount];

        eStencil.SetVertexWeights(eVertWeights);
        eStencil.SetFaceWeights(eFaceWeights);

        SdcRule pRule = (eSharpness > 0.0) ? SdcRule::RULE_CREASE : SdcRule::RULE_SMOOTH;
        SdcRule cRule = child.vertRule(cVert);

        scheme.ComputeEdgeVertexStencil(eHood, eStencil, pRule, cRule);

        //
        //  Identify the parent edge-verts and child verts of the incident parent faces
        //  and apply the weights from the stencil:
        //
        //  (Note on the child verts of incident faces -- if a vert is from an edge that
        //  was interior in the parent, it will also be interior in the child as one of
        //  its end verts must have had all incident face-children generated for it to
        //  be supported.  So every face incident to the parent edge is guaranteed to
        //  have a corresponding child vertex generated in the refinement.)
        //
        childPos[cVert] = eVertWeights[0] * parentPos[eVerts[0]] +
                          eVertWeights[1] * parentPos[eVerts[1]];

        if (eStencil.GetFaceWeightCount() > 0) {
            assert(eStencil.GetFaceWeightCount() == eFaceCount);

            if (eFaceCount == 2) {
                VtrIndex cVertFace0 = refinement.mFaceChildVertIndex[eFaces[0]];
                VtrIndex cVertFace1 = refinement.mFaceChildVertIndex[eFaces[1]];

                childPos[cVert] += eFaceWeights[0] * childPos[cVertFace0] +
                                   eFaceWeights[1] * childPos[cVertFace1];
            } else {
                for (int i = 0; i < eFaceCount; ++i) {
                    VtrIndex cVertOfFace = refinement.mFaceChildVertIndex[eFaces[i]];

                    childPos[cVert] += eFaceWeights[i] * childPos[cVertOfFace];
                }
            }
        }
    }
}


//
//  Example to compute and apply stencils for edge-vertices:
//
inline void
interpolateChildVertsFromVerts(VtrRefinement const&  refinement,
                               PositionVector const& parentPos,
                               PositionVector&       childPos)
{
    SdcOptions         options = refinement.GetOptions();
    SdcScheme<CATMARK> scheme(options);

    SdcVertexNeighborhood vHood;
    SdcStencil<float>     vStencil;

    VtrLevel const& parent = *refinement.mParent;
    VtrLevel const& child  = *refinement.mChild;

    //
    //  Vertices from parent vertices -- the Rule for both parent and child have
    //  already been determined, so we can use them to accelerate the stencils:
    //
    for (int pVert = 0; pVert < parent.vertCount(); ++pVert) {
        VtrIndex cVert = refinement.mVertChildVertIndex[pVert];
        if (!VtrIndexIsValid(cVert)) return;

        VtrIndexAccessor vEdges = parent.accessVertEdges(pVert);
        VtrIndexAccessor vFaces = parent.accessVertFaces(pVert);

        //
        //  Initialize the Neighborhood -- we will be using parent and child Rules to
        //  help compute the Stencil weights, so inspect them to determine what might
        //  be required in the VertexNeighborhood.  The sharpness values for incident
        //  parent edges are needed if we have a Crease.  If we have a transition
        //  between rules, then we need all sharpness values (vertex and edge) for
        //  both parent and child to determine the appropriate transitional weight.
        //
        int vFaceCount = vFaces.size();
        int vEdgeCount = vEdges.size();

        vHood.SetFaceCount(vFaceCount);
        vHood.SetEdgeCount(vEdgeCount);

        SdcRule pRule = parent.vertRule(pVert);
        SdcRule cRule = child.vertRule(cVert);

        float pSharpness[vEdgeCount];
        float cSharpness[vEdgeCount];

        if ((cRule != pRule) || (pRule == SdcCrease::RULE_CREASE)) {
            for (int i = 0; i < vEdgeCount; ++i) {
                pSharpness[i] = parent.edgeSharpness(vEdges[i]);
            }
            if (cRule != pRule) {
                //  Likely faster to just recompute sharpness locally than gather:
                SdcCrease(options).SubdivideEdgeSharpnessAroundVertex(vEdgeCount, pSharpness, cSharpness);

                vHood.SetChildSharpness(child.vertSharpness(pVert), cSharpness);
            }
            vHood.SetSharpness(parent.vertSharpness(pVert), pSharpness);
        }

        //
        //  Initialize/assign the Stencil weights and compute:
        //
        float vVertWeight;
        float vFaceWeights[vFaceCount];
        float vEdgeWeights[vEdgeCount];

        vStencil.SetVertexWeights(&vVertWeight);
        vStencil.SetFaceWeights(vFaceWeights);
        vStencil.SetEdgeWeights(vEdgeWeights);

        scheme.ComputeVertexVertexStencil(vHood, vStencil, pRule, cRule);

        //
        //  Apply the Stencil weights to the appropriate vertices -- edge-verts and the
        //  face-verts of the child need only be identified if needed:
        //
        childPos[cVert] = vVertWeight * parentPos[pVert];

        if (vStencil.GetEdgeWeightCount() > 0) {
            for (int i = 0; i < vEdgeCount; ++i) {
                VtrIndexAccessor eVerts = parent.accessEdgeVerts(vEdges[i]);
                VtrIndex pVertOppositeEdge = (eVerts[0] == pVert) ? eVerts[1] : eVerts[0];

                childPos[cVert] += vEdgeWeights[i] * parentPos[pVertOppositeEdge];
            }
        }
        if (vStencil.GetFaceWeightCount() > 0) {
            for (int i = 0; i < vFaceCount; ++i) {
                VtrIndex cVertOfFace = refinement.mFaceChildVertIndex[vFaces[i]];

                childPos[cVert] += vFaceWeights[i] * childPos[cVertOfFace];
            }
        }
    }
}

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
