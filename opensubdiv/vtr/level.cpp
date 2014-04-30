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
#include "../sdc/type.h"
#include "../sdc/crease.h"
#include "../vtr/array.h"
#include "../vtr/level.h"
#include "../vtr/refinement.h"

#include <cassert>
#include <vector>
#include <cstdio>


//
//  NOTES on short-cuts and assumptions...
//      This was developed to prototype some ideas, and some issues were simplified
//  (or just plain ignored) in order to validate the more general cases first.  Use
//  the following list of keywords to search for notes in comments where these issues
//  are discussed -- though most code that they may have referred to may have since
//  been moved to VtrRefinement:
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
        VtrIndexArray const fVerts      = accessFaceVerts(fIndex);
        int                 fVertCount  = fVerts.size();

        for (int i = 0; i < fVertCount; ++i) {
            VtrIndex vIndex = fVerts[i];

            VtrIndexArray const      vFaces = accessVertFaces(vIndex);
            VtrLocalIndexArray const vInFace = accessVertFaceLocalIndices(vIndex);

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
        VtrIndexArray const fEdges      = accessFaceEdges(fIndex);
        int                 fEdgeCount  = fEdges.size();

        for (int i = 0; i < fEdgeCount; ++i) {
            int eIndex = fEdges[i];

            VtrIndexArray const eFaces      = accessEdgeFaces(eIndex);
            int                 eFaceCount  = eFaces.size();

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
        VtrIndexArray const eVerts = accessEdgeVerts(eIndex);

        for (int i = 0; i < 2; ++i) {
            VtrIndex vIndex = eVerts[i];

            VtrIndexArray const      vEdges = accessVertEdges(vIndex);
            VtrLocalIndexArray const vInEdge = accessVertEdgeLocalIndices(vIndex);

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
    printIndexArray(VtrArray<INT_TYPE> const& array)
    {
        printf("%d [%d", array.size(), array[0]);
        for (int i = 1; i < array.size(); ++i) {
            printf(" %d", array[i]);
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
            default:
                assert(0);
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
        printIndexArray(accessFaceVerts(i));
    }
    printf("      face-edge indices = %lu\n", mFaceEdgeIndices.size());
    for (int i = 0; printFaceEdges && i < faceCount(); ++i) {
        printf("        face %4d edges:  ", i);
        printIndexArray(accessFaceEdges(i));
    }
    if (pRefinement) {
        printf("      face child-verts = %lu\n", pRefinement->_faceChildVertIndex.size());
        for (int i = 0; printFaceChildVerts && i < (int)pRefinement->_faceChildVertIndex.size(); ++i) {
            printf("        face %4d child vert:  %d\n", i, pRefinement->_faceChildVertIndex[i]);
        }
    }

    printf("    Edge relations:\n");
    printf("      edge-vert indices = %lu\n", mEdgeVertIndices.size());
    for (int i = 0; printEdgeVerts && i < edgeCount(); ++i) {
        printf("        edge %4d verts:  ", i);
        printIndexArray(accessEdgeVerts(i));
    }
    printf("      edge-face counts/offset = %lu\n", mEdgeFaceCountsAndOffsets.size());
    printf("      edge-face indices = %lu\n", mEdgeFaceIndices.size());
    for (int i = 0; printEdgeFaces && i < edgeCount(); ++i) {
        printf("        edge %4d faces:  ", i);
        printIndexArray(accessEdgeFaces(i));
    }
    if (pRefinement) {
        printf("      edge child-verts = %lu\n", pRefinement->_edgeChildVertIndex.size());
        for (int i = 0; printEdgeChildVerts && i < (int)pRefinement->_edgeChildVertIndex.size(); ++i) {
            printf("        edge %4d child vert:  %d\n", i, pRefinement->_edgeChildVertIndex[i]);
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
        printIndexArray(accessVertFaces(i));

        printf("             face-verts:  ");
        printIndexArray(accessVertFaceLocalIndices(i));
    }
    printf("      vert-edge counts/offset = %lu\n", mVertEdgeCountsAndOffsets.size());
    printf("      vert-edge indices  = %lu\n", mVertEdgeIndices.size());
    printf("      vert-edge children = %lu\n", mVertEdgeLocalIndices.size());
    for (int i = 0; printVertEdges && i < vertCount(); ++i) {
        printf("        vert %4d edges:  ", i);
        printIndexArray(accessVertEdges(i));

        printf("             edge-verts:  ");
        printIndexArray(accessVertEdgeLocalIndices(i));
    }
    if (pRefinement) {
        printf("      vert child-verts = %lu\n", pRefinement->_vertChildVertIndex.size());
        for (int i = 0; printVertChildVerts && i < (int)pRefinement->_vertChildVertIndex.size(); ++i) {
            printf("        vert %4d child vert:  %d\n", i, pRefinement->_vertChildVertIndex[i]);
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
