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

#include <common/vtr_utils.h>
#include <common/hbr_utils.h>
#include <osd/vertex.h>

#include "../common/gl_common.h"

#include <algorithm>

// Wrapper class for drawing Hbr & Vtr meshes & components
class GLMesh {

public:

    enum Component {
        COMP_FACE=0,
        COMP_EDGE,
        COMP_VERT,
        COMP_NUM_COMPONENTS
    };

    enum VertColorMode {
        VERTCOLOR_SOLID=0,
        VERTCOLOR_BY_LEVEL,
        VERTCOLOR_BY_SHARPNESS,
        VERTCOLOR_BY_PARENT_TYPE
    };

    enum EdgeColorMode {
        EDGECOLOR_SOLID=0,
        EDGECOLOR_BY_LEVEL,
        EDGECOLOR_BY_SHARPNESS
    };

    enum FaceColorMode {
        FACECOLOR_SOLID=0,
        EDGECOLOR_BY_PATCH_TYPE
    };

    struct Options {
        unsigned int vertColorMode:3,
                     edgeColorMode:2,
                     faceColorMode:2;
    };

    // -----------------------------------------------------
    // Hbr initialization
    template <class T>
    void Initialize(Options options,
        std::vector<OpenSubdiv::HbrFace<T> const *> const & faces) {

        assert(not faces.empty());

        OpenSubdiv::HbrMesh<T> const * hmesh = faces[0]->GetMesh();

        int nfaces = (int)faces.size(),
            nverts = hmesh->GetNumVertices(),
            nedgeverts = 0;

        { // EAOs ------------------------------------------
            _eao[COMP_VERT].reserve(nfaces*4);
            _eao[COMP_FACE].reserve(nfaces*4);
            for (int i=0; i<nfaces; ++i) {

                OpenSubdiv::HbrFace<T> const * f = faces[i];

                int nv = f->GetNumVertices();
                for (int j=0; j<nv; ++j) {
                    _eao[COMP_VERT].push_back(f->GetVertex(j)->GetID());
                    _eao[COMP_FACE].push_back(f->GetVertex(j)->GetID());
                }
            }
            std::sort(_eao[COMP_VERT].begin(), _eao[COMP_VERT].end());
            std::unique(_eao[COMP_VERT].begin(), _eao[COMP_VERT].end()); // XXXX this might be slow...

            nedgeverts = _eao[COMP_FACE].size()*2;
            _eao[COMP_EDGE].resize(nedgeverts);
            for (int i=0; i<nedgeverts; ++i) {
                _eao[COMP_EDGE][i]=i;
            }
        }

        { // VBOs ------------------------------------------
            _vbo[COMP_VERT].resize(nverts*6);
            _vbo[COMP_FACE].resize(nverts*3);
            for (int i=0; i<nverts; ++i) {

                OpenSubdiv::HbrVertex<T> const * v = hmesh->GetVertex(i);

                float * vertdata = &_vbo[COMP_VERT][v->GetID()*6],
                      * facedata = &_vbo[COMP_FACE][v->GetID()*3];

                // copy position
                memcpy(vertdata, v->GetData().GetPos(), sizeof(float)*3);
                memcpy(facedata, v->GetData().GetPos(), sizeof(float)*3);

                // set color
                if (options.vertColorMode==VERTCOLOR_BY_LEVEL) {
                    int depth=0;
                    if (v->IsConnected()) {
                        depth = v->GetIncidentEdge()->GetFace()->GetDepth();
                    }
                    setColorByLevel(depth, vertdata+3);
                } else if (options.vertColorMode==VERTCOLOR_BY_SHARPNESS) {
                    setColorBySharpness(v->GetSharpness(), vertdata+3);
                } else {
                    setSolidColor(vertdata+3);
                }
            }

            _vbo[COMP_EDGE].resize(nedgeverts*6);
            for (int i=0, ofs=0; i<nfaces; ++i) {

                OpenSubdiv::HbrFace<T> const * f = faces[i];
                int nv = f->GetNumVertices();
                for (int j=0; j<nv; ++j) {

                    OpenSubdiv::HbrHalfedge<T> const * e = f->GetEdge(j);
                    OpenSubdiv::HbrVertex<T> const * v0 = e->GetOrgVertex(),
                                                   * v1 = e->GetDestVertex();

                    float * v0data = &_vbo[COMP_EDGE][ofs],
                          * v1data = &_vbo[COMP_EDGE][ofs+6];

                    // copy position
                    memcpy(v0data, v0->GetData().GetPos(), sizeof(float)*3);
                    memcpy(v1data, v1->GetData().GetPos(), sizeof(float)*3);

                    // set color
                    if (options.vertColorMode==EDGECOLOR_BY_LEVEL) {

                        int depth = f->GetDepth();
                        setColorByLevel(depth, v0data+3);
                        setColorByLevel(depth, v1data+3);
                    } else if (options.vertColorMode==EDGECOLOR_BY_SHARPNESS) {

                        setColorBySharpness(e->GetSharpness(), v0data+3);
                        setColorBySharpness(e->GetSharpness(), v1data+3);
                    } else {

                        setSolidColor(v0data+3);
                        setSolidColor(v1data+3);
                    }
                    ofs += 12;
                }
            }
        }

        _numComps[COMP_FACE] = _eao[COMP_FACE].size();
        _numComps[COMP_EDGE] = _eao[COMP_EDGE].size();
        _numComps[COMP_VERT] = _eao[COMP_VERT].size();

        initializeDeviceBuffers();
    }

    // -----------------------------------------------------
    // Vtr initialization
    typedef OpenSubdiv::FarRefineTables RefineTables;

    void Initialize(Options options, RefineTables & refTables, float * vertexData);

    // -----------------------------------------------------

    GLMesh();

    ~GLMesh();

    void Draw(Component comp, GLuint transformUB, GLuint lightingUB);

    void SetDiffuseColor(float r, float g, float b, float a);

private:

           void setSolidColor(float * color);

    static void setColorByLevel(int level, float * color);

    static void setColorBySharpness(float sharpness, float * color);

    void initializeBuffers(Options options, RefineTables & refTables, float * vertexData);

    void initializeDeviceBuffers();

    void clearBuffers();


    int _numComps[COMP_NUM_COMPONENTS];

    GLuint _VAO[COMP_NUM_COMPONENTS],
           _VBO[COMP_NUM_COMPONENTS],
           _EAO[COMP_NUM_COMPONENTS];

    std::vector<float> _vbo[COMP_NUM_COMPONENTS];
    std::vector<int>   _eao[COMP_NUM_COMPONENTS];
    
    float _ambientColor[4],
          _diffuseColor[4];
};

