// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

#include "../sdc/type.h"
#include "../sdc/crease.h"
#include "../sdc/arrayInterface.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"

#include <cassert>
#include <vector>


//
//  NOTES on short-cuts and assumptions...
//      This was developed to prototype some ideas, and some issues were simplified
//  (or just plain ignored) in order to validate the more general cases first.  Use
//  the following list of keywords to search for notes in comments where these issues
//  are discussed:
//
//  CORRECTNESS:
//      - shortcuts taken when progress was more important than correctness
//
//  ORIENTATION:
//      - OSD/HBr orient subdivided faces in a specific way, on which hierarchical
//        edits strongly depend
//      - regular faces (quads) are split so that the vertex in the middle of the
//        faces is the i'th vertex in the i'th child face -- this preserves the
//        rectangular parameterization of the new collection of faces.
//      - extra-ordinary faces (including triangels) are split so that the vertex
//        in the middle of the faces is the 0'th vertex in all child faces.
//
//  N-GONS:
//      - OSD/HBr support N-sided faces at level 0, which will ultimately need support
//
namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


//
//  Simple (for now) constructor and destructor:
//
VtrLevel::VtrLevel() :
    _faceCount(0),
    _edgeCount(0),
    _vertCount(0),
    _depth(0),
    _hasTopology(false)
{
}

VtrLevel::~VtrLevel()
{
}


//
//  Method to "classify" a vertex by inspecting its neighborhood (which we only
//  need for child vertices generated from a parent or for all vertices at the
//  coarsest level).
//    
//  The resulting classication is one of a topological type, i.e. Corner, Crease,
//  Dart or Smooth based on the following information:
//
//    - sharpness value of the given vertex
//    - boundary status of its incident edges
//    - sharpness values of its incident edges
//
//  NOTE:  this is where using sharpness to tag boundary features really pays
//  off.  Testing sharpness and topology independently or combined is a real
//  pain here otherwise.
//
//  What we are really doing here is counting the number of "singular" edges,
//  where singularity may be determined topologically by a boundary, or by
//  the presence of sharp edges.  Face-varying data will need a similar kind
//  of classification, and having some edge-related property would help.
//
//  Given the above point, and the eventual need to classify face-varying
//  data, putting the topology information in the sharpness would help both.
//  We can create a seperate sharpness vector for face-varying data and that
//  can be used to detect the singular edges.
//
//  (Note this is likely to move to an Sdc query in some form, though Tony has
//  expressed the desire to hide these "masks" as an implementation detail.  It
//  may be difficult to port Hbr to Sdc without them.)
//
/*
SdcRule
VtrLevel::classifyVertFromTopology(VtrIndex vIndex)
{
    return SdcVertexRuleFromGlobalSharpness(accessVertEdges(vIndex),
                                            SdcConstArrayInterface<float>(mEdgeSharpness),
                                            vertSharpness(vIndex));
}
*/


//
//  Debugging method to validate topology, i.e. verify appropriate symmetry
//  between the relations, etc.
//
bool
VtrLevel::validateTopology() const
{
    //
    //  Verify internal topological consistency (eventually a Level method?):
    //      - each face-vert has corresponding vert-face (and child)
    //      - each face-edge has corresponding edge-face
    //      - each edge-vert has corresponding vert-edge (and child)
    //  The above three are enough for most cases, but it is still possible
    //  the latter relation in each above has no correspondent in the former,
    //  so apply the symmetric tests:
    //      - each edge-face has corresponding face-edge
    //      - each vert-face has corresponding face-vert
    //      - each vert-edge has corresponding edge-vert
    //  We are still left with the possibility of duplicate references in
    //  places we don't want them.  Currently a component can exist multiple
    //  times in a component of higher dimension.
    //      - each vert-face <face,child> pair is unique
    //      - each vert-edge <edge,child> pair is unique
    //
    bool returnOnFirstError = true;
    bool isValid = true;

    //  Verify each face-vert has corresponding vert-face and child:
    for (int fIndex = 0; fIndex < faceCount(); ++fIndex) {
        VtrIndexAccessor fVerts      = accessFaceVerts(fIndex);
        int              fVertCount  = fVerts.size();

        for (int i = 0; i < fVertCount; ++i) {
            VtrIndex vIndex = fVerts[i];

            VtrIndexAccessor      vFaces = accessVertFaces(vIndex);
            VtrLocalIndexAccessor vInFace = accessVertFaceLocalIndices(vIndex);

            bool vertFaceOfFaceExists = false;
            for (int j = 0; j < vFaces.size(); ++j) {
                if ((vFaces[j] == fIndex) && (vInFace[j] == i)) {
                    vertFaceOfFaceExists = true;
                    break;
                }
            }
            if (!vertFaceOfFaceExists) {
                isValid = false;
                if (returnOnFirstError) {
                    printf("Error in fIndex = %d:  correlation of vert %d failed\n", fIndex, i);
                    return isValid;
                }
            }
        }
    }

    //  Verify each face-edge has corresponding edge-face:
    for (int fIndex = 0; fIndex < faceCount(); ++fIndex) {
        VtrIndexAccessor fEdges      = accessFaceEdges(fIndex);
        int              fEdgeCount  = fEdges.size();

        for (int i = 0; i < fEdgeCount; ++i) {
            int eIndex = fEdges[i];

            VtrIndexAccessor eFaces      = accessEdgeFaces(eIndex);
            int              eFaceCount  = eFaces.size();

            bool edgeFaceOfFaceExists = false;
            for (int j = 0; j < eFaceCount; ++j) {
                if (eFaces[j] == fIndex) {
                    edgeFaceOfFaceExists = true;
                    break;
                }
            }
            if (!edgeFaceOfFaceExists) {
                isValid = false;
                if (returnOnFirstError) {
                    printf("Error in fIndex = %d:  correlation of edge %d failed\n", fIndex, i);
                    return isValid;
                }
            }
        }
    }

    //  Verify each edge-vert has corresponding vert-edge and child:
    for (int eIndex = 0; eIndex < edgeCount(); ++eIndex) {
        VtrIndexAccessor eVerts = accessEdgeVerts(eIndex);

        for (int i = 0; i < 2; ++i) {
            VtrIndex vIndex = eVerts[i];

            VtrIndexAccessor      vEdges = accessVertEdges(vIndex);
            VtrLocalIndexAccessor vInEdge = accessVertEdgeLocalIndices(vIndex);

            bool vertEdgeOfEdgeExists = false;
            for (int j = 0; j < vEdges.size(); ++j) {
                if ((vEdges[j] == eIndex) && (vInEdge[j] == i)) {
                    vertEdgeOfEdgeExists = true;
                    break;
                }
            }
            if (!vertEdgeOfEdgeExists) {
                isValid = false;
                if (returnOnFirstError) {
                    printf("Error in eIndex = %d:  correlation of vert %d failed\n", eIndex, i);
                    return isValid;
                }
            }
        }
    }
    return isValid;
}

//
//  Anonymous helper functions for debugging output -- yes, using printf(), this is not
//  intended to serve anyone other than myself for now and I favor its formatting control
//
namespace {
    template <typename INT_TYPE>
    void
    printIndexAccessor(SdcConstArrayInterface<INT_TYPE> const& accessor)
    {
        printf("%d [%d", accessor.size(), accessor[0]);
        for (int i = 1; i < accessor.size(); ++i) {
            printf(" %d", accessor[i]);
        }
        printf("]\n");
    }

    const char*
    ruleString(SdcRule rule)
    {
        switch (rule) {
        case SdcCrease::RULE_SMOOTH:  return "Smooth";
        case SdcCrease::RULE_DART:    return "Dart";
        case SdcCrease::RULE_CREASE:  return "Crease";
        case SdcCrease::RULE_CORNER:  return "Corner";
        }
        return 0;
    }
}

void
VtrLevel::print(const VtrRefinement* pRefinement) const
{
    bool printFaceVerts      = true;
    bool printFaceEdges      = true;
    bool printFaceChildVerts = false;

    bool printEdgeVerts      = true;
    bool printEdgeFaces      = false;
    bool printEdgeChildVerts = true;
    bool printEdgeSharpness  = true;

    bool printVertFaces      = false;
    bool printVertEdges      = false;
    bool printVertChildVerts = false;
    bool printVertSharpness  = true;
    bool printVertRules      = true;

    printf("Level (0x%p):\n", this);
    printf("  Depth = %d\n", _depth);

    printf("  Primary component counts:\n");
    printf("    faces = %d\n", _faceCount);
    printf("    edges = %d\n", _edgeCount);
    printf("    verts = %d\n", _vertCount);

    printf("  Topology relation sizes:\n");

    printf("    Face relations:\n");
    printf("      face-vert counts/offset = %lu\n", mFaceVertCountsAndOffsets.size());
    printf("      face-vert indices = %lu\n", mFaceVertIndices.size());
    for (int i = 0; printFaceVerts && i < faceCount(); ++i) {
        printf("        face %4d verts:  ", i);
        printIndexAccessor(accessFaceVerts(i));
    }
    printf("      face-edge indices = %lu\n", mFaceEdgeIndices.size());
    for (int i = 0; printFaceEdges && i < faceCount(); ++i) {
        printf("        face %4d edges:  ", i);
        printIndexAccessor(accessFaceEdges(i));
    }
    if (pRefinement) {
        printf("      face child-verts = %lu\n", pRefinement->mFaceChildVertIndex.size());
        for (int i = 0; printFaceChildVerts && i < (int)pRefinement->mFaceChildVertIndex.size(); ++i) {
            printf("        face %4d child vert:  %d\n", i, pRefinement->mFaceChildVertIndex[i]);
        }
    }

    printf("    Edge relations:\n");
    printf("      edge-vert indices = %lu\n", mEdgeVertIndices.size());
    for (int i = 0; printEdgeVerts && i < edgeCount(); ++i) {
        printf("        edge %4d verts:  ", i);
        printIndexAccessor(accessEdgeVerts(i));
    }
    printf("      edge-face counts/offset = %lu\n", mEdgeFaceCountsAndOffsets.size());
    printf("      edge-face indices = %lu\n", mEdgeFaceIndices.size());
    for (int i = 0; printEdgeFaces && i < edgeCount(); ++i) {
        printf("        edge %4d faces:  ", i);
        printIndexAccessor(accessEdgeFaces(i));
    }
    if (pRefinement) {
        printf("      edge child-verts = %lu\n", pRefinement->mEdgeChildVertIndex.size());
        for (int i = 0; printEdgeChildVerts && i < (int)pRefinement->mEdgeChildVertIndex.size(); ++i) {
            printf("        edge %4d child vert:  %d\n", i, pRefinement->mEdgeChildVertIndex[i]);
        }
    }
    printf("      edge sharpness = %lu\n", mEdgeSharpness.size());
    for (int i = 0; printEdgeSharpness && i < (int)mEdgeSharpness.size(); ++i) {
        printf("        edge %4d sharpness:  %f\n", i, mEdgeSharpness[i]);
    }

    printf("    Vert relations:\n");
    printf("      vert-face counts/offset = %lu\n", mVertFaceCountsAndOffsets.size());
    printf("      vert-face indices  = %lu\n", mVertFaceIndices.size());
    printf("      vert-face children = %lu\n", mVertFaceLocalIndices.size());
    for (int i = 0; printVertFaces && i < vertCount(); ++i) {
        printf("        vert %4d faces:  ", i);
        printIndexAccessor(accessVertFaces(i));

        printf("             face-verts:  ");
        printIndexAccessor(accessVertFaceLocalIndices(i));
    }
    printf("      vert-edge counts/offset = %lu\n", mVertEdgeCountsAndOffsets.size());
    printf("      vert-edge indices  = %lu\n", mVertEdgeIndices.size());
    printf("      vert-edge children = %lu\n", mVertEdgeLocalIndices.size());
    for (int i = 0; printVertEdges && i < vertCount(); ++i) {
        printf("        vert %4d edges:  ", i);
        printIndexAccessor(accessVertEdges(i));

        printf("             edge-verts:  ");
        printIndexAccessor(accessVertEdgeLocalIndices(i));
    }
    if (pRefinement) {
        printf("      vert child-verts = %lu\n", pRefinement->mVertChildVertIndex.size());
        for (int i = 0; printVertChildVerts && i < (int)pRefinement->mVertChildVertIndex.size(); ++i) {
            printf("        vert %4d child vert:  %d\n", i, pRefinement->mVertChildVertIndex[i]);
        }
    }
    printf("      vert sharpness = %lu\n", mVertSharpness.size());
    for (int i = 0; printVertSharpness && i < (int)mVertSharpness.size(); ++i) {
        printf("        vert %4d sharpness:  %f\n", i, mVertSharpness[i]);
    }
    printf("      vert rules = %lu\n", mVertRule.size());
    for (int i = 0; printVertRules && i < (int)mVertRule.size(); ++i) {
        printf("        vert %4d type:  %s\n", i, ruleString(mVertRule[i]));
    }
}

} // end namespace OPENSUBDIV_VERSION
} // end namespace OpenSubdiv

// TM and (c) 2014 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
