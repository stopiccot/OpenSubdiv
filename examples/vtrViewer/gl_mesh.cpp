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

#include "gl_mesh.h"
#include "gl_fontutils.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

//------------------------------------------------------------------------------
// color palettes
static float         g_solidColor[4] = {1.0f,  1.0f,  1.0f, 1.0f},
                   g_ambientColor[4] = {0.1f,  0.1f,  0.1f, 1.0f};

static float    g_levelColors[10][4] = {{1.0f,  1.0f,  1.0f},
                                        {1.0f,  1.0f,  0.0f},
                                        {1.0f,  0.5f,  0.0f},
                                        {0.8f,  0.0f,  0.0f},
                                        {0.0f,  1.0f,  0.5f},
                                        {0.0f,  1.0f,  1.0f},
                                        {0.0f,  0.5f,  1.0f},
                                        {0.0f,  0.5f,  0.5f},
                                        {0.5f,  0.0f,  1.0f},
                                        {1.0f,  0.5f,  1.0f}};

static float g_parentTypeColors[4][4] = {{0.9f,  0.9f,  0.9f},
                                         {0.4f,  0.8f,  0.4f},
                                         {0.8f,  0.8f,  0.4f},
                                         {0.8f,  0.4f,  0.4f}};

//------------------------------------------------------------------------------
void
GLMesh::setSolidColor(float * color) {

    color[0] = _diffuseColor[0];
    color[1] = _diffuseColor[1];
    color[2] = _diffuseColor[2];
}

void
GLMesh::setColorByLevel(int level, float * color) {

    color[0] = g_levelColors[level][0];
    color[1] = g_levelColors[level][1];
    color[2] = g_levelColors[level][2];
}

void
GLMesh::setColorBySharpness(float sharpness, float * color) {

    //  0.0       2.0       4.0
    // green --- yellow --- red
    color[0] = std::min(1.0f, sharpness * 0.5f);
    color[1] = std::min(1.0f, 2.0f - sharpness * 0.5f);
    color[2] = 0;
}

//------------------------------------------------------------------------------
GLMesh::GLMesh() {

    for (int i=0; i<COMP_NUM_COMPONENTS; ++i) {
        _VAO[i]=0;
        _VBO[i]=0;
        _EAO[i]=0;

        _numComps[i] = 0;
    }

    memcpy(_ambientColor, g_ambientColor, sizeof(float)*4);
    memcpy(_diffuseColor, g_solidColor, sizeof(float)*4);
}

GLMesh::~GLMesh() {

    for (int i=0; i<COMP_NUM_COMPONENTS; ++i) {
        if (_VAO[i]) {
            glDeleteVertexArrays(1, &_VAO[i]);
        }
        if (_VBO[i]) {
            glDeleteBuffers(1, &_VBO[i]);
        }
        if (_EAO[i]) {
            glDeleteBuffers(1, &_EAO[i]);
        }
    }
}

//------------------------------------------------------------------------------
void
GLMesh::Initialize(Options options, RefineTables & refTables, float * vertexData) {

    initializeBuffers(options, refTables, vertexData);

    _numComps[COMP_FACE] = _eao[COMP_FACE].size();
    _numComps[COMP_EDGE] = _eao[COMP_EDGE].size();
    _numComps[COMP_VERT] = _eao[COMP_VERT].size();

    initializeDeviceBuffers();
}

//------------------------------------------------------------------------------
void
GLMesh::initializeBuffers(Options options, RefineTables & refTables, float * vertexData) {

    typedef OpenSubdiv::FarRefineTables::IndexArray IndexArray;

    int maxlevel = refTables.GetMaxLevel(),
        nverts = refTables.GetNumVertices(maxlevel),
        nedges = refTables.GetNumEdges(maxlevel),
        firstvert = 0;
    

    for (int i=0; i<maxlevel; ++i) {
        firstvert += refTables.GetNumVertices(i);
    }

    float * vertData =  &vertexData[firstvert*3];

    { // vertex color component ----------------------------

        std::vector<float> & vbo = _vbo[COMP_VERT];
        vbo.resize(nverts * 6);

        std::vector<int> & eao = _eao[COMP_VERT];
        eao.resize(nverts);

        for (int vert=0; vert<nverts; ++vert) {

            // copy positions
            memcpy(&vbo[vert*6], &vertData[vert*3], sizeof(float)*3);

            // populate EAO
            eao[vert] = vert;
        }

        // set colors
        if (options.vertColorMode==VERTCOLOR_BY_LEVEL) {

            for (int level=0, ofs=3; level<=refTables.GetMaxLevel(); ++level) {
                for (int vert=0; vert<refTables.GetNumVertices(level); ++vert, ofs+=6) {
                    setColorByLevel(level, &vbo[ofs]);
                }
            }
        } else if (options.vertColorMode==VERTCOLOR_BY_SHARPNESS) {

            for (int vert=0, ofs=3; vert<refTables.GetNumVertices(maxlevel); ++vert, ofs+=6) {
               setColorBySharpness(refTables.GetVertexSharpness(maxlevel, vert), &vbo[ofs]);
            }
        } else if (options.vertColorMode==VERTCOLOR_BY_PARENT_TYPE) {

            int ofs=3;
            if (maxlevel>0) {
                for (int vert=0; vert<refTables.GetNumFaces(maxlevel-1); ++vert, ofs+=6) {
                    memcpy(&vbo[ofs], g_parentTypeColors[1], sizeof(float)*3);
                }
                for (int vert=0; vert<refTables.GetNumEdges(maxlevel-1); ++vert, ofs+=6) {
                    memcpy(&vbo[ofs], g_parentTypeColors[2], sizeof(float)*3);
                }
                for (int vert=0; vert<refTables.GetNumVertices(maxlevel-1); ++vert, ofs+=6) {
                    memcpy(&vbo[ofs], g_parentTypeColors[3], sizeof(float)*3);
                }
            } else {
                for (int vert=0; vert<refTables.GetNumVertices(maxlevel); ++vert, ofs+=6) {
                    memcpy(&vbo[ofs], g_parentTypeColors[0], sizeof(float)*3);
                }
            }
        } else {

            for (int vert=0, ofs=3; vert<nverts; ++vert) {
                setSolidColor(&vbo[ofs+=6]);
            }
        }
    }


    { // edge color component ------------------------------

        std::vector<float> & vbo = _vbo[COMP_EDGE];
        vbo.resize(nedges * 2 * 6);

        std::vector<int> & eao = _eao[COMP_EDGE];
        eao.resize(nedges*2);

        for (int edge=0; edge<nedges; ++edge) {

            eao[edge*2  ] = edge*2;
            eao[edge*2+1] = edge*2+1;

            IndexArray const verts = refTables.GetEdgeVertices(maxlevel, edge);

            float * v0 = &vbo[edge*2*6],
                  * v1 = v0+6;

            // copy position
            memcpy(v0, vertData + verts[0]*3, sizeof(float)*3);
            memcpy(v1, vertData + verts[1]*3, sizeof(float)*3);

            // set colors
            if (options.edgeColorMode==EDGECOLOR_BY_LEVEL) {

                setColorByLevel(maxlevel, v0+3);
                setColorByLevel(maxlevel, v1+3);
             } else  if (options.edgeColorMode==EDGECOLOR_BY_SHARPNESS) {

                float sharpness = refTables.GetEdgeSharpness(maxlevel, edge);
                setColorBySharpness(sharpness, v0+3);
                setColorBySharpness(sharpness, v1+3);
            } else {

                // default to solid color
                setSolidColor(v0+3);
                setSolidColor(v1+3);
            }
        }
    }

    { // face component ------------------------------------

        std::vector<float> & vbo = _vbo[COMP_FACE];
        vbo.resize(nverts * 3);

        memcpy(&vbo[0], vertData, nverts*sizeof(float)*3);

        int nfaceverts = 0; // XXXX this should be provided by FarefineTables
        for (int face=0; face<refTables.GetNumFaces(maxlevel); ++face) {

            nfaceverts+=refTables.GetFaceVertices(maxlevel, face).size();
        }

        std::vector<int> & eao = _eao[COMP_FACE];
        eao.resize(nfaceverts);

        for (int face=0, ofs=0; face<refTables.GetNumFaces(maxlevel); ++face) {

            IndexArray fverts = refTables.GetFaceVertices(maxlevel, face);
            for (int vert=0; vert<fverts.size(); ++vert) {
                eao[ofs++] = fverts[vert];
            }
        }
    }
}

//------------------------------------------------------------------------------
void
GLMesh::initializeDeviceBuffers() {

    // copy buffers to device
    for (int i=0; i<COMP_NUM_COMPONENTS; ++i) {

        if (not _VAO[i]) {
            glGenVertexArrays(1, &_VAO[i]);
        }
        glBindVertexArray(_VAO[i]);

        if (not _vbo[i].empty()) {
            if (not _VBO[i]) {
                glGenBuffers(1, &_VBO[i]);
            }
            glBindBuffer(GL_ARRAY_BUFFER, _VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, _vbo[i].size()*sizeof(GLfloat), &_vbo[i][0], GL_STATIC_DRAW);
 
            glEnableVertexAttribArray(0);
            
            int numelements = (i==COMP_FACE) ? 3 : 6;
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numelements*sizeof(GLfloat), 0);

            if (i==COMP_FACE) {
                 // face vbo has no color component
                 glDisableVertexAttribArray(1);
             } else {
                 glEnableVertexAttribArray(1);
                 glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)12);
             }
        }
 
        if (not _eao[i].empty()) {

            if (not _EAO[i]) {
                glGenBuffers(1, &_EAO[i]);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EAO[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _eao[i].size()*sizeof(int), &_eao[i][0], GL_STATIC_DRAW);
        }
        
        checkGLErrors("init");
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    clearBuffers();
}

//------------------------------------------------------------------------------
void
GLMesh::clearBuffers() {

    for (int i=0; i<COMP_NUM_COMPONENTS; ++i) {
        _vbo[i].clear();
        _eao[i].clear();
    }
}


//------------------------------------------------------------------------------
static const char * g_simpleShaderSrc =
#include "simpleShader.gen.h"
;

static const char * g_faceShaderSrc =
#include "faceShader.gen.h"
;

GLuint g_simpleProgram=0,
       g_faceProgram=0;

// Update and bind transform state
static void
bindProgram( char const * shaderSource,
             GLuint * program,
             GLuint transformUB,
             GLuint lightingUB,
             bool geometry) {

    assert(program);

    GLuint uboIndex=GL_INVALID_INDEX,
           transformBinding=0,
           lightingBinding=1;

    // Update and bind transform state
    if (not *program) {

        *program = glCreateProgram();

        static char const versionStr[] = "#version 330\n",
                          vtxDefineStr[] = "#define VERTEX_SHADER\n",
                          geoDefineStr[] = "#define GEOMETRY_SHADER\n",
                          fragDefineStr[] = "#define FRAGMENT_SHADER\n";

        std::string vsSrc = std::string(versionStr) + vtxDefineStr + shaderSource,
                    gsSrc = std::string(versionStr) + geoDefineStr + shaderSource,
                    fsSrc = std::string(versionStr) + fragDefineStr + shaderSource;

        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vsSrc.c_str()),
               geometryShader = geometry ? compileShader(GL_GEOMETRY_SHADER, gsSrc.c_str()) : 0,
               fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSrc.c_str());

        glAttachShader(*program, vertexShader);
        if (geometry) {
            glAttachShader(*program, geometryShader);
        }
        glAttachShader(*program, fragmentShader);

        glLinkProgram(*program);

        GLint status;
        glGetProgramiv(*program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            glGetProgramiv(*program, GL_INFO_LOG_LENGTH, &infoLogLength);
            char *infoLog = new char[infoLogLength];
            glGetProgramInfoLog(*program, infoLogLength, NULL, infoLog);
            printf("%s\n", infoLog);
            delete[] infoLog;
            exit(1);
        }

        uboIndex = glGetUniformBlockIndex(*program, "Transform");
        if (uboIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(*program, uboIndex, transformBinding);
        }

        uboIndex = glGetUniformBlockIndex(*program, "Lighting");
        if (uboIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(*program, uboIndex, lightingBinding);
        }
    }
    glUseProgram(*program);

    if (transformUB) {
        glBindBufferBase(GL_UNIFORM_BUFFER, transformBinding, transformUB);
    }

    if (lightingUB) {
        glBindBufferBase(GL_UNIFORM_BUFFER, lightingBinding, lightingUB);
    }
}

//------------------------------------------------------------------------------
void
GLMesh::Draw(Component comp, GLuint transformUB, GLuint lightingUB) {

    if (comp==COMP_VERT) {

        bindProgram(g_simpleShaderSrc, &g_simpleProgram, transformUB, lightingUB, false);

        glBindVertexArray(_VAO[COMP_VERT]);
 
        glPointSize(4.0f);
        glDrawElements(GL_POINTS, _numComps[COMP_VERT], GL_UNSIGNED_INT, (void *)0);
        glPointSize(1.0f);

    } else if (comp==COMP_EDGE) {

        bindProgram(g_simpleShaderSrc, &g_simpleProgram, transformUB, lightingUB, false);

        glBindVertexArray(_VAO[COMP_EDGE]);

        glDrawElements(GL_LINES, _numComps[COMP_EDGE], GL_UNSIGNED_INT, (void *)0);

    } else if (comp==COMP_FACE) {

        glEnable(GL_CULL_FACE);

        bindProgram(g_faceShaderSrc, &g_faceProgram, transformUB, lightingUB, true);


        GLuint diffuseColor = glGetUniformLocation(g_faceProgram, "diffuseColor");
        glProgramUniform4f(g_faceProgram, diffuseColor, _diffuseColor[0], _diffuseColor[1], _diffuseColor[2], _diffuseColor[3]);
        

        glBindVertexArray(_VAO[COMP_FACE]);

        glDrawElements(GL_LINES_ADJACENCY, _numComps[COMP_FACE], GL_UNSIGNED_INT, (void *)0);

        glDisable(GL_CULL_FACE);
    }
}

//------------------------------------------------------------------------------
void
GLMesh::SetDiffuseColor(float r, float g, float b, float a) {

    _diffuseColor[0] = r;
    _diffuseColor[1] = g;
    _diffuseColor[2] = b;
    _diffuseColor[3] = a;
}

//------------------------------------------------------------------------------

