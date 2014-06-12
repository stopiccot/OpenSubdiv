//
//   Copyright 2013 Pixar
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

#ifndef VTR_UTILS_H
#define VTR_UTILS_H

#include <far/refineTablesFactory.h>

#include "../../regression/common/shape_utils.h"

//------------------------------------------------------------------------------

inline OpenSubdiv::SdcType
GetSdcType(Shape const & shape) {

    OpenSubdiv::SdcType type;

    switch (shape.scheme) {
        case kBilinear: type = OpenSubdiv::TYPE_BILINEAR; break;
        case kCatmark : type = OpenSubdiv::TYPE_CATMARK; break;
        case kLoop    : type = OpenSubdiv::TYPE_LOOP; break;
    }
    return type;
}

inline OpenSubdiv::SdcOptions
GetSdcOptions(Shape const & shape) {

    typedef OpenSubdiv::SdcOptions SdcOptions;

    SdcOptions result;

    result.SetVVarBoundaryInterpolation(SdcOptions::VVAR_BOUNDARY_EDGE_ONLY);
    result.SetCreasingMethod(SdcOptions::CREASE_UNIFORM);
    result.SetTriangleSubdivision(SdcOptions::TRI_SUB_NORMAL);
    result.SetNonManifoldInterpolation(SdcOptions::NON_MANIFOLD_SHARP);

    for (int i=0; i<(int)shape.tags.size(); ++i) {

        Shape::tag * t = shape.tags[i];

        if (t->name=="interpolateboundary") {
            if ((int)t->intargs.size()!=1) {
                printf("expecting 1 integer for \"interpolateboundary\" tag n. %d\n", i);
                continue;
            }
            switch( t->intargs[0] ) {
                case 0 : result.SetVVarBoundaryInterpolation(SdcOptions::VVAR_BOUNDARY_NONE); break;
                case 1 : result.SetVVarBoundaryInterpolation(SdcOptions::VVAR_BOUNDARY_EDGE_AND_CORNER); break;
                case 2 : result.SetVVarBoundaryInterpolation(SdcOptions::VVAR_BOUNDARY_EDGE_ONLY); break;
                default: printf("unknown interpolate boundary : %d\n", t->intargs[0] ); break;
            }
        } else if (t->name=="facevaryinginterpolateboundary") {
            if ((int)t->intargs.size()!=1) {
                printf("expecting 1 integer for \"facevaryinginterpolateboundary\" tag n. %d\n", i);
                continue;
            }
            switch( t->intargs[0] ) {
                case 0 : result.SetFVarBoundaryInterpolation(SdcOptions::FVAR_BOUNDARY_BILINEAR); break;
                case 1 : result.SetFVarBoundaryInterpolation(SdcOptions::FVAR_BOUNDARY_EDGE_AND_CORNER); break;
                case 2 : result.SetFVarBoundaryInterpolation(SdcOptions::FVAR_BOUNDARY_EDGE_ONLY); break;
                case 3 : result.SetFVarBoundaryInterpolation(SdcOptions::FVAR_BOUNDARY_ALWAYS_SHARP); break;
                default: printf("unknown interpolate boundary : %d\n", t->intargs[0] ); break;
            }
        } else if (t->name=="facevaryingpropagatecorners") {
            if ((int)t->intargs.size()==1) {
                // XXXX no propagate corners in SdcOptions
            } else
                printf( "expecting single int argument for \"facevaryingpropagatecorners\"\n" );
        } else if (t->name=="smoothtriangles") {

            if (shape.scheme!=kCatmark) {
                printf("the \"smoothtriangles\" tag can only be applied to Catmark meshes\n");
                continue;
            }
        } else if (t->name=="creasemethod") {

            if ((int)t->stringargs.size()==0) {
                printf("the \"creasemethod\" tag expects a string argument\n");
                continue;
            }

            if( t->stringargs[0]=="normal" )
                result.SetCreasingMethod(SdcOptions::CREASE_UNIFORM);
            else if( t->stringargs[0]=="chaikin" )
                result.SetCreasingMethod(SdcOptions::CREASE_CHAIKIN);
            else
                printf("the \"creasemethod\" tag only accepts \"normal\" or \"chaikin\" as value (%s)\n", t->stringargs[0].c_str());
        }
    }

    return result;
}

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

template <>
inline void
FarRefineTablesFactory<Shape>::resizeComponentTopology(
    FarRefineTables & refTables, Shape const & shape) {

    int nfaces = shape.GetNumFaces(),
        nverts = shape.GetNumVertices();

    refTables.setNumBaseFaces(nfaces);
    for (int i=0; i<nfaces; ++i) {

        int nv = shape.nvertsPerFace[i];
        assert(nv==4); // temporary until n-gons are supported

        refTables.setNumBaseFaceVertices(i, nv);
    }

    // Vertices and vert-faces and vert-edges
    refTables.setNumBaseVertices(nverts);
}

//----------------------------------------------------------
template <>
inline void
FarRefineTablesFactory<Shape>::assignComponentTopology(
    FarRefineTables & refTables, Shape const & shape) {

    typedef FarRefineTables::IndexArray IndexArray;

    { // Face relations:
        int nfaces = refTables.getNumBaseFaces();

        for (int i=0, ofs=0; i < nfaces; ++i) {

            IndexArray dstFaceVerts = refTables.setBaseFaceVertices(i);
            IndexArray dstFaceEdges = refTables.setBaseFaceEdges(i);

            for (int j=0; j<dstFaceVerts.size(); ++j) {
                dstFaceVerts[j] = shape.faceverts[ofs++];
            }
        }
    }
}

//----------------------------------------------------------
template <>
inline void
FarRefineTablesFactory<Shape>::assignComponentTags(
    FarRefineTables & refTables, Shape const & shape) {

    for (int i=0; i<(int)shape.tags.size(); ++i) {

        Shape::tag * t = shape.tags[i];

        if (t->name=="crease") {

            for (int j=0; j<(int)t->intargs.size()-1; j += 2) {

                OpenSubdiv::VtrIndex edge = refTables.FindEdge(/*level*/0, t->intargs[j], t->intargs[j+1]);
                if (edge==OpenSubdiv::VTR_INDEX_INVALID) {
                    printf("cannot find edge for crease tag (%d,%d)\n", t->intargs[j], t->intargs[j+1] );
                } else {
                    int nfloat = (int) t->floatargs.size();
                    refTables.baseEdgeSharpness(edge) =
                        std::max(0.0f, ((nfloat > 1) ? t->floatargs[j] : t->floatargs[0]));
                }
            }
        } else if (t->name=="corner") {

            for (int j=0; j<(int)t->intargs.size(); ++j) {
                int vertex = t->intargs[j];
                if (vertex<0 or vertex>=refTables.GetNumVertices(/*level*/0)) {
                    printf("cannot find vertex for corner tag (%d)\n", vertex );
                } else {
                    int nfloat = (int) t->floatargs.size();
                    refTables.baseVertexSharpness(i) =
                        std::max(0.0f, ((nfloat > 1) ? t->floatargs[j] : t->floatargs[0]));
                }
            }
        }
    }
}

} // namespace OPENSUBDIV_VERSION
} // namespace OpenSubdiv


//------------------------------------------------------------------------------

#endif /* VTR_UTILS_H */
