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

#if defined(__APPLE__)
    #if defined(OSD_USES_GLEW)
        #include <GL/glew.h>
    #else
        #include <OpenGL/gl3.h>
    #endif
    #define GLFW_INCLUDE_GL3
    #define GLFW_NO_GLU
#else
    #include <stdlib.h>
    #include <GL/glew.h>
    #if defined(WIN32)
        #include <GL/wglew.h>
    #endif
#endif

#if defined(GLFW_VERSION_3)
    #include <GLFW/glfw3.h>
    GLFWwindow* g_window=0;
    GLFWmonitor* g_primary=0;
#else
    #include <GL/glfw.h>
#endif

#include <osd/error.h>
#include <osd/vertex.h>
#include <osd/cpuGLVertexBuffer.h>
#include <common/shape_utils.h>
#include "../common/stopwatch.h"
#include "../common/simple_math.h"
#include "../common/gl_common.h"
#include "../common/gl_hud.h"
#include "../common/patchColors.h"

#include "hbr_utils.h"
#include "gl_fontutils.h"

#include <typeinfo>
#include <cfloat>
#include <vector>
#include <set>
#include <fstream>

//------------------------------------------------------------------------------

struct SimpleShape {
    std::string  name;
    Scheme       scheme;
    std::string  data;

    SimpleShape() { }
    SimpleShape( std::string const & idata, char const * iname, Scheme ischeme )
        : name(iname), scheme(ischeme), data(idata) { }
};

std::vector<SimpleShape> g_defaultShapes;

int g_currentShape = 0;

enum HudCheckBox { kHUD_CB_DISPLAY_CAGE_EDGES,
                   kHUD_CB_DISPLAY_CAGE_VERTS,
                   kHUD_CB_ANIMATE_VERTICES,
                   kHUD_CB_DISPLAY_PATCH_COLOR };


int   g_frame = 0,
      g_repeatCount = 0;

// GUI variables
int   g_fullscreen = 0,
      g_freeze = 0,
      g_drawCageEdges = 1,
      g_drawCageVertices = 0,
      g_mbutton[3] = {0, 0, 0},
      g_running = 1;

int   g_displayPatchColor = 1,
      g_drawMode = 1;

float g_rotate[2] = {0, 0},
      g_dolly = 5,
      g_pan[2] = {0, 0},
      g_center[3] = {0, 0, 0},
      g_size = 0;

int   g_prev_x = 0,
      g_prev_y = 0;

int   g_width = 1024,
      g_height = 1024;

GLhud g_hud;

GLFont * g_font=0;


// performance
float g_cpuTime = 0;
float g_gpuTime = 0;
Stopwatch g_fpsTimer;

// geometry
std::vector<float> g_orgPositions,
                   g_positions;

Scheme             g_scheme;

int g_level = 2;

float g_moveScale = 0.0f;

GLuint g_queries[2] = {0, 0};

GLuint g_transformUB = 0,
       g_lightingUB = 0;

struct Transform {
    float ModelViewMatrix[16];
    float ProjectionMatrix[16];
    float ModelViewProjectionMatrix[16];
} g_transformData;

GLuint g_vao = 0,
       g_refinedVAO=0,
       g_refinedVertsEAO=0;

GLuint g_cageEdgeVAO = 0,
       g_cageEdgeVBO = 0,
       g_cageVertexVAO = 0,
       g_cageVertexVBO = 0;

std::vector<int> g_coarseEdges;
std::vector<float> g_coarseEdgeSharpness;
std::vector<float> g_coarseVertexSharpness;

struct Program
{
    GLuint program;
    GLuint uniformModelViewProjectionMatrix;
    GLuint attrPosition;
    GLuint attrColor;
} g_defaultProgram;


static bool
linkDefaultProgram()
{
#if defined(GL_ARB_tessellation_shader) || defined(GL_VERSION_4_0)
    #define GLSL_VERSION_DEFINE "#version 400\n"
#else
    #define GLSL_VERSION_DEFINE "#version 150\n"
#endif

    static const char *vsSrc =
        GLSL_VERSION_DEFINE
        "in vec3 position;\n"
        "in vec3 color;\n"
        "out vec4 fragColor;\n"
        "uniform mat4 ModelViewProjectionMatrix;\n"
        "void main() {\n"
        "  fragColor = vec4(color, 1);\n"
        "  gl_Position = ModelViewProjectionMatrix * "
        "                  vec4(position, 1);\n"
        "}\n";

    static const char *fsSrc =
        GLSL_VERSION_DEFINE
        "in vec4 fragColor;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "  color = fragColor;\n"
        "}\n";

    GLuint program = glCreateProgram();
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSrc);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *infoLog = new char[infoLogLength];
        glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);
        printf("%s\n", infoLog);
        delete[] infoLog;
        exit(1);
    }

    g_defaultProgram.program = program;
    g_defaultProgram.uniformModelViewProjectionMatrix =
        glGetUniformLocation(program, "ModelViewProjectionMatrix");
    g_defaultProgram.attrPosition = glGetAttribLocation(program, "position");
    g_defaultProgram.attrColor = glGetAttribLocation(program, "color");

    return true;
}

//------------------------------------------------------------------------------

static void
initializeShapes( ) {

#include <shapes/catmark_cube_corner0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_corner0, "catmark_cube_corner0", kCatmark));

#include <shapes/catmark_cube_corner1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_corner1, "catmark_cube_corner1", kCatmark));

#include <shapes/catmark_cube_corner2.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_corner2, "catmark_cube_corner2", kCatmark));

#include <shapes/catmark_cube_corner3.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_corner3, "catmark_cube_corner3", kCatmark));

#include <shapes/catmark_cube_corner4.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_corner4, "catmark_cube_corner4", kCatmark));

#include <shapes/catmark_cube_creases0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_creases0, "catmark_cube_creases0", kCatmark));

#include <shapes/catmark_cube_creases1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube_creases1, "catmark_cube_creases1", kCatmark));

#include <shapes/catmark_cube.h>
    g_defaultShapes.push_back(SimpleShape(catmark_cube, "catmark_cube", kCatmark));

#include <shapes/catmark_dart_edgecorner.h>
    g_defaultShapes.push_back(SimpleShape(catmark_dart_edgecorner, "catmark_dart_edgecorner", kCatmark));

#include <shapes/catmark_dart_edgeonly.h>
    g_defaultShapes.push_back(SimpleShape(catmark_dart_edgeonly, "catmark_dart_edgeonly", kCatmark));

#include <shapes/catmark_edgecorner.h>
    g_defaultShapes.push_back(SimpleShape(catmark_edgecorner ,"catmark_edgecorner", kCatmark));

#include <shapes/catmark_edgeonly.h>
    g_defaultShapes.push_back(SimpleShape(catmark_edgeonly, "catmark_edgeonly", kCatmark));

#include <shapes/catmark_chaikin0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_chaikin0, "catmark_chaikin0", kCatmark));

#include <shapes/catmark_chaikin1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_chaikin1, "catmark_chaikin1", kCatmark));

#include <shapes/catmark_fan.h>
    g_defaultShapes.push_back(SimpleShape(catmark_fan, "catmark_fan", kCatmark));

#include <shapes/catmark_gregory_test1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_gregory_test1, "catmark_gregory_test1", kCatmark));

#include <shapes/catmark_gregory_test2.h>
    g_defaultShapes.push_back(SimpleShape(catmark_gregory_test2, "catmark_gregory_test2", kCatmark));

#include <shapes/catmark_gregory_test3.h>
    g_defaultShapes.push_back(SimpleShape(catmark_gregory_test3, "catmark_gregory_test3", kCatmark));

#include <shapes/catmark_gregory_test4.h>
    g_defaultShapes.push_back(SimpleShape(catmark_gregory_test4, "catmark_gregory_test4", kCatmark));

#include <shapes/catmark_hole_test1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_hole_test1, "catmark_hole_test1", kCatmark));

#include <shapes/catmark_hole_test2.h>
    g_defaultShapes.push_back(SimpleShape(catmark_hole_test2, "catmark_hole_test2", kCatmark));

#include <shapes/catmark_pyramid_creases0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_pyramid_creases0, "catmark_pyramid_creases0", kCatmark));

#include <shapes/catmark_pyramid_creases1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_pyramid_creases1, "catmark_pyramid_creases1", kCatmark));

#include <shapes/catmark_pyramid.h>
    g_defaultShapes.push_back(SimpleShape(catmark_pyramid, "catmark_pyramid", kCatmark));

#include <shapes/catmark_tent_creases0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_tent_creases0, "catmark_tent_creases0", kCatmark));

#include <shapes/catmark_tent_creases1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_tent_creases1, "catmark_tent_creases1", kCatmark));

#include <shapes/catmark_tent.h>
    g_defaultShapes.push_back(SimpleShape(catmark_tent, "catmark_tent", kCatmark));

#include <shapes/catmark_torus.h>
    g_defaultShapes.push_back(SimpleShape(catmark_torus, "catmark_torus", kCatmark));

#include <shapes/catmark_torus_creases0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_torus_creases0, "catmark_torus_creases0", kCatmark));

#include <shapes/catmark_square_hedit0.h>
    g_defaultShapes.push_back(SimpleShape(catmark_square_hedit0, "catmark_square_hedit0", kCatmark));

#include <shapes/catmark_square_hedit1.h>
    g_defaultShapes.push_back(SimpleShape(catmark_square_hedit1, "catmark_square_hedit1", kCatmark));

#include <shapes/catmark_square_hedit2.h>
    g_defaultShapes.push_back(SimpleShape(catmark_square_hedit2, "catmark_square_hedit2", kCatmark));

#include <shapes/catmark_square_hedit3.h>
    g_defaultShapes.push_back(SimpleShape(catmark_square_hedit3, "catmark_square_hedit3", kCatmark));

#include <shapes/catmark_square_hedit4.h>
    g_defaultShapes.push_back(SimpleShape(catmark_square_hedit4, "catmark_square_hedit4", kCatmark));

#include <shapes/catmark_bishop.h>
    g_defaultShapes.push_back(SimpleShape(catmark_bishop, "catmark_bishop", kCatmark));

#include <shapes/catmark_car.h>
    g_defaultShapes.push_back(SimpleShape(catmark_car, "catmark_car", kCatmark));

#include <shapes/catmark_helmet.h>
    g_defaultShapes.push_back(SimpleShape(catmark_helmet, "catmark_helmet", kCatmark));

#include <shapes/catmark_pawn.h>
    g_defaultShapes.push_back(SimpleShape(catmark_pawn, "catmark_pawn", kCatmark));

#include <shapes/catmark_rook.h>
    g_defaultShapes.push_back(SimpleShape(catmark_rook, "catmark_rook", kCatmark));

#include <shapes/bilinear_cube.h>
    g_defaultShapes.push_back(SimpleShape(bilinear_cube, "bilinear_cube", kBilinear));


#include <shapes/loop_cube_creases0.h>
    g_defaultShapes.push_back(SimpleShape(loop_cube_creases0, "loop_cube_creases0", kLoop));

#include <shapes/loop_cube_creases1.h>
    g_defaultShapes.push_back(SimpleShape(loop_cube_creases1, "loop_cube_creases1", kLoop));

#include <shapes/loop_cube.h>
    g_defaultShapes.push_back(SimpleShape(loop_cube, "loop_cube", kLoop));

#include <shapes/loop_icosahedron.h>
    g_defaultShapes.push_back(SimpleShape(loop_icosahedron, "loop_icosahedron", kLoop));

#include <shapes/loop_saddle_edgecorner.h>
    g_defaultShapes.push_back(SimpleShape(loop_saddle_edgecorner, "loop_saddle_edgecorner", kLoop));

#include <shapes/loop_saddle_edgeonly.h>
    g_defaultShapes.push_back(SimpleShape(loop_saddle_edgeonly, "loop_saddle_edgeonly", kLoop));

#include <shapes/loop_triangle_edgecorner.h>
    g_defaultShapes.push_back(SimpleShape(loop_triangle_edgecorner, "loop_triangle_edgecorner", kLoop));

#include <shapes/loop_triangle_edgeonly.h>
    g_defaultShapes.push_back(SimpleShape(loop_triangle_edgeonly, "loop_triangle_edgeonly", kLoop));

#include <shapes/loop_chaikin0.h>
    g_defaultShapes.push_back(SimpleShape(loop_chaikin0, "loop_chaikin0", kLoop));

#include <shapes/loop_chaikin1.h>
    g_defaultShapes.push_back(SimpleShape(loop_chaikin1, "loop_chaikin1", kLoop));
}

//------------------------------------------------------------------------------
static void
updateGeom() {

    int nverts = (int)g_orgPositions.size() / 3;

    std::vector<float> vertex, varying;

    vertex.reserve(nverts*3);

    const float *p = &g_orgPositions[0];

    float r = sin(g_frame*0.001f) * g_moveScale;
    for (int i = 0; i < nverts; ++i) {
        //float move = 0.05f*cosf(p[0]*20+g_frame*0.01f);
        float ct = cos(p[2] * r);
        float st = sin(p[2] * r);
        g_positions[i*3+0] = p[0]*ct + p[1]*st;
        g_positions[i*3+1] = -p[0]*st + p[1]*ct;
        g_positions[i*3+2] = p[2];

        p += 3;
    }

    p = &g_orgPositions[0];
    const float *pp = &g_positions[0];
    for (int i = 0; i < nverts; ++i) {
        vertex.push_back(pp[0]);
        vertex.push_back(pp[1]);
        vertex.push_back(pp[2]);
        pp += 3;
    }

/*
    Stopwatch s;

    s.Start();
    s.Stop();
    g_cpuTime = float(s.GetElapsed() * 1000.0f);

    s.Start();
    s.Stop();
    g_gpuTime = float(s.GetElapsed() * 1000.0f);
*/
}

class Vertex {

public:
    Vertex() { }

    void Clear( void * =0 ) { pos[0]=pos[1]=pos[2]=0.0f; }

    void AddWithWeight(const Vertex& src, float weight, void * =0 ) {
        pos[0]+=weight*src.pos[0];
        pos[1]+=weight*src.pos[1];
        pos[2]+=weight*src.pos[2];
    }

    void AddVaryingWithWeight(const Vertex& , float, void * =0 ) {
    }

    float pos[3],
          col[3];
};

// Display VBO (collects outputs of updated stencils)
OpenSubdiv::OsdCpuGLVertexBuffer * g_refinedVertsVBO=0;

OpenSubdiv::FarRefineTables * g_refTables=0;

//------------------------------------------------------------------------------
static Vertex *
initializeRefinedVertsVBO(OpenSubdiv::FarRefineTables * refTables) {

#define COLOR_BY_LEVEL
//#define COLOR_BY_PARENT_TYPE

#ifdef COLOR_BY_LEVEL
    static float g_levelColors[10][4] = {{1.0f,  1.0f,  1.0f},
                                         {0.8f,  0.0f,  0.0f},
                                         {0.0f,  1.0f,  0.0f},
                                         {1.0f,  1.0f,  0.0f},
                                         {1.0f,  0.5f,  0.0f},
                                         {0.0f,  1.0f,  1.0f},
                                         {0.0f,  0.5f,  1.0f},
                                         {0.0f,  0.5f,  0.5f},
                                         {0.5f,  0.0f,  1.0f},
                                         {1.0f,  0.5f,  1.0f}};
#endif

#ifdef COLOR_BY_PARENT_TYPE
    static float g_parentTypeColors[4][4] = {{0.9f,  0.9f,  0.9f},
                                             {0.4f,  0.8f,  0.4f},
                                             {0.8f,  0.8f,  0.4f},
                                             {0.8f,  0.4f,  0.4f}};
#endif
    delete g_refinedVertsVBO;
    g_refinedVertsVBO = OpenSubdiv::OsdCpuGLVertexBuffer::Create(6, refTables->GetVertCount());

    Vertex * verts = (Vertex *)g_refinedVertsVBO->BindCpuBuffer(), * lverts=verts;

    for (int level=0; level<=g_refTables->GetMaxLevel(); ++level) {

        // populate coarse mesh positions
        if (level==0) {
            for (int i=0; i<g_refTables->GetVertCount(0); ++i) {
                memcpy(&lverts[i].pos[0], &g_orgPositions[i*3], sizeof(float)*3);
            }
        }

#ifdef COLOR_BY_LEVEL
        for (int i=0; i<g_refTables->GetVertCount(level); ++i) {
            memcpy(&lverts[i].col[0], g_levelColors[level], sizeof(float)*3);
        }
#endif

#ifdef COLOR_BY_PARENT_TYPE
        if (level==0) {
            for (int i=0; i<g_refTables->GetVertCount(level); ++i) {
                memcpy(&lverts[i].col[0], g_parentTypeColors[0], sizeof(float)*3);
            }
        } else {
            OpenSubdiv::VtrLevel const & parent = refTables->GetLevel(level-1);
            OpenSubdiv::VtrRefinement const & refinement = refTables->GetRefinement(level-1);

            for (int i=0; i<parent.faceCount(); ++i) {
                memcpy(&lverts[refinement.faceChildVertexIndex(i)].col[0],
                    g_parentTypeColors[1], sizeof(float)*3);
            }
            for (int i=0; i<parent.edgeCount(); ++i) {
                memcpy(&lverts[refinement.edgeChildVertexIndex(i)].col[0],
                    g_parentTypeColors[2], sizeof(float)*3);
            }
            for (int i=0; i<parent.vertCount(); ++i) {
                memcpy(&lverts[refinement.vertexChildVertexIndex(i)].col[0],
                    g_parentTypeColors[3], sizeof(float)*3);
            }
        }
#endif
        lverts += g_refTables->GetVertCount(level);
    }

    return verts;
}

//------------------------------------------------------------------------------
static void
createComponents() {

    assert(g_font);
    
    g_font->Clear();

    Vertex * verts = (Vertex *)g_refinedVertsVBO->BindCpuBuffer();

    for(int l=0; l<=g_level; ++l) {

        int nverts = g_refTables->GetVertCount(l);

        if (l==g_level) {

            //OpenSubdiv::VtrLevel const & level = g_refTables->GetLevel(l);

            for (int i=0; i<nverts; ++i) {

                static char buff[16];
                snprintf(buff, 16, "%d", i);

                g_font->Print3D( &verts[i].pos[0], buff);
            }

        } else {
            verts += nverts;
        }
    }
}

//------------------------------------------------------------------------------
static void
createMesh( const std::string &shape, int level, Scheme scheme=kCatmark ) {

    // generate Hbr representation from "obj" description
    OsdHbrMesh * hmesh = simpleHbr<OpenSubdiv::OsdVertex>(shape.c_str(), scheme, g_orgPositions, /*fvar*/false);

    typedef HbrConverter<OpenSubdiv::OsdVertex> HbrConverter;

    HbrConverter conv(*hmesh);

    OpenSubdiv::FarRefineTablesFactory<HbrConverter> refFactory(conv.GetType(), conv.GetOptions());

    delete g_refTables;
    g_refTables = refFactory.Create(conv, level, /*full topology*/ true);

    {
        glBindVertexArray(g_vao);

        glBindVertexArray(g_refinedVAO);

        Vertex * verts = initializeRefinedVertsVBO(g_refTables);

        g_refTables->Interpolate<Vertex>(verts, verts + g_refTables->GetVertCount(0));

        std::vector<int> vbo;

        for (int level=0, vofs=0, nverts=0; level<=g_level; ++level, vofs += nverts) {
            OpenSubdiv::VtrLevel const & vtrLevel = g_refTables->GetLevel(level);
            nverts = vtrLevel.vertCount();
            if (level==g_level) {
                OpenSubdiv::VtrIndexArray const fverts = vtrLevel.accessFaceVerts(0);
                vbo.resize(vtrLevel.faceVertCount());
                for (int i=0; i<(int)vbo.size(); ++i) {
                    vbo[i] = fverts[i] + vofs;
                }
            }
        }


        if (not g_refinedVertsEAO) {
            glGenBuffers(1, &g_refinedVertsEAO);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_refinedVertsEAO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vbo.size()*sizeof(int), &vbo[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, g_refinedVertsVBO->BindVBO());

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), 0);

        glDisableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    createComponents();

    //------------------------------------------------------
    g_positions.resize(g_orgPositions.size(),0.0f);

    // save coarse topology (used for coarse mesh drawing)
    g_coarseEdges.clear();
    g_coarseEdgeSharpness.clear();
    g_coarseVertexSharpness.clear();
    int nf = hmesh->GetNumFaces();
    for(int i=0; i<nf; ++i) {
        OsdHbrFace *face = hmesh->GetFace(i);
        int nv = face->GetNumVertices();
        for(int j=0; j<nv; ++j) {
            g_coarseEdges.push_back(face->GetVertex(j)->GetID());
            g_coarseEdges.push_back(face->GetVertex((j+1)%nv)->GetID());
            g_coarseEdgeSharpness.push_back(face->GetEdge(j)->GetSharpness());
        }
    }
    int nv = hmesh->GetNumVertices();
    for(int i=0; i<nv; ++i) {
        g_coarseVertexSharpness.push_back(hmesh->GetVertex(i)->GetSharpness());
    }

    // compute model bounding
    float min[3] = { FLT_MAX,  FLT_MAX,  FLT_MAX};
    float max[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    for (size_t i=0; i <g_orgPositions.size()/3; ++i) {
        for(int j=0; j<3; ++j) {
            float v = g_orgPositions[i*3+j];
            min[j] = std::min(min[j], v);
            max[j] = std::max(max[j], v);
        }
    }
    for (int j=0; j<3; ++j) {
        g_center[j] = (min[j] + max[j]) * 0.5f;
        g_size += (max[j]-min[j])*(max[j]-min[j]);
    }

    // Hbr mesh can be deleted
    delete hmesh;

    g_scheme = scheme;

    updateGeom();
    checkGLErrors("createMesh");
}

//------------------------------------------------------------------------------
static void
fitFrame() {

    g_pan[0] = g_pan[1] = 0;
    g_dolly = g_size;
}

//------------------------------------------------------------------------------
static inline void
setSharpnessColor(float s, float *r, float *g, float *b)
{
    //  0.0       2.0       4.0
    // green --- yellow --- red
    *r = std::min(1.0f, s * 0.5f);
    *g = std::min(1.0f, 2.0f - s*0.5f);
    *b = 0;
}

static void
drawCageEdges() {

    glUseProgram(g_defaultProgram.program);
    glUniformMatrix4fv(g_defaultProgram.uniformModelViewProjectionMatrix,
                       1, GL_FALSE, g_transformData.ModelViewProjectionMatrix);

    std::vector<float> vbo;
    vbo.reserve(g_coarseEdges.size() * 6);
    float r, g, b;
    for (int i = 0; i < (int)g_coarseEdges.size(); i+=2) {
        setSharpnessColor(g_coarseEdgeSharpness[i/2], &r, &g, &b);
        for (int j = 0; j < 2; ++j) {
            vbo.push_back(g_positions[g_coarseEdges[i+j]*3]);
            vbo.push_back(g_positions[g_coarseEdges[i+j]*3+1]);
            vbo.push_back(g_positions[g_coarseEdges[i+j]*3+2]);
            vbo.push_back(r);
            vbo.push_back(g);
            vbo.push_back(b);
        }
    }

    glBindVertexArray(g_cageEdgeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, g_cageEdgeVBO);
    glBufferData(GL_ARRAY_BUFFER, (int)vbo.size() * sizeof(float), &vbo[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(g_defaultProgram.attrPosition);
    glEnableVertexAttribArray(g_defaultProgram.attrColor);
    glVertexAttribPointer(g_defaultProgram.attrPosition,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, 0);
    glVertexAttribPointer(g_defaultProgram.attrColor,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, (void*)12);

    glDrawArrays(GL_LINES, 0, (int)g_coarseEdges.size());

    glBindVertexArray(0);
    glUseProgram(0);
}

static void
drawCageVertices() {

    glUseProgram(g_defaultProgram.program);
    glUniformMatrix4fv(g_defaultProgram.uniformModelViewProjectionMatrix,
                       1, GL_FALSE, g_transformData.ModelViewProjectionMatrix);

    int numPoints = (int)g_positions.size()/3;
    std::vector<float> vbo;
    vbo.reserve(numPoints*6);
    float r, g, b;
    for (int i = 0; i < numPoints; ++i) {
        setSharpnessColor(g_coarseVertexSharpness[i], &r, &g, &b);
        vbo.push_back(g_positions[i*3+0]);
        vbo.push_back(g_positions[i*3+1]);
        vbo.push_back(g_positions[i*3+2]);
        vbo.push_back(r);
        vbo.push_back(g);
        vbo.push_back(b);
    }

    glBindVertexArray(g_cageVertexVAO);

    glBindBuffer(GL_ARRAY_BUFFER, g_cageVertexVBO);
    glBufferData(GL_ARRAY_BUFFER, (int)vbo.size() * sizeof(float), &vbo[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(g_defaultProgram.attrPosition);
    glEnableVertexAttribArray(g_defaultProgram.attrColor);
    glVertexAttribPointer(g_defaultProgram.attrPosition,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, 0);
    glVertexAttribPointer(g_defaultProgram.attrColor,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, (void*)12);

    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glPointSize(1.0f);

    glBindVertexArray(0);
    glUseProgram(0);
}


static void
drawRefinedVerts() {

    glUseProgram(g_defaultProgram.program);

    glUniformMatrix4fv(g_defaultProgram.uniformModelViewProjectionMatrix,
                       1, GL_FALSE, g_transformData.ModelViewProjectionMatrix);


    glBindVertexArray(g_refinedVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_refinedVertsVBO->BindVBO());

    glEnableVertexAttribArray(g_defaultProgram.attrPosition);
    glEnableVertexAttribArray(g_defaultProgram.attrColor);
    glVertexAttribPointer(g_defaultProgram.attrPosition,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, 0);
    glVertexAttribPointer(g_defaultProgram.attrColor,
                          3, GL_FLOAT, GL_FALSE, sizeof (GLfloat) * 6, (void*)12);

    int firstVert=0, numVerts = g_refTables->GetVertCount(g_level);
    for (int i=0; i<g_level; ++i) {
        firstVert+=g_refTables->GetVertCount(i);
    }

    glPointSize(3.0f);
    glDrawArrays(GL_POINTS, firstVert, numVerts);
    glPointSize(1.0f);

    glBindVertexArray(0);
    glUseProgram(0);
}


// Update and bind transform state
static void
bindProgram( char const * shaderSource,
             GLuint * program,
             GLuint * transformBinding=0,
             GLuint * lightingBinding=0) {

    assert(program);

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
               geometryShader = compileShader(GL_GEOMETRY_SHADER, gsSrc.c_str()),
               fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSrc.c_str());

        glAttachShader(*program, vertexShader);
        glAttachShader(*program, geometryShader);
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

        GLuint uboIndex;

        if (transformBinding) {
            *transformBinding = 0;
            uboIndex = glGetUniformBlockIndex(*program, "Transform");
            if (uboIndex != GL_INVALID_INDEX)
                glUniformBlockBinding(*program, uboIndex, *transformBinding);

        }

        if (lightingBinding) {
            *lightingBinding = 1;
            uboIndex = glGetUniformBlockIndex(*program, "Lighting");
            if (uboIndex != GL_INVALID_INDEX)
                glUniformBlockBinding(*program, uboIndex, *lightingBinding);
        }
    }
    glUseProgram(*program);

    if (transformBinding) {
        assert(g_transformUB);
        glBindBufferBase(GL_UNIFORM_BUFFER, *transformBinding, g_transformUB);
    }

    if (lightingBinding) {
        assert(g_lightingUB);
        glBindBufferBase(GL_UNIFORM_BUFFER, *lightingBinding, g_lightingUB);
    }
}

//------------------------------------------------------------------------------
static void
drawRefinedQuads() {

    static const char *shaderSource =
#include "shader.gen.h"
;

    static GLuint program=0,
                  transformBinding=0,
                  lightingBinding=0;

    bindProgram(shaderSource, &program, &transformBinding, &lightingBinding);

    GLuint diffuseColor = glGetUniformLocation(program, "diffuseColor");
    glProgramUniform4f(program, diffuseColor, 0.4f, 0.4f, 0.8f, 1);

    glBindVertexArray(g_refinedVAO);

    OpenSubdiv::VtrLevel const & level = g_refTables->GetLevel(g_level);

    glDrawElements(GL_LINES_ADJACENCY, level.faceVertCount(), GL_UNSIGNED_INT, 0);

    glUseProgram(0);
}

//------------------------------------------------------------------------------
static void
drawComponents() {

    g_font->Draw(g_transformUB);
}

//------------------------------------------------------------------------------
static void
display() {

    g_hud.GetFrameBuffer()->Bind();

    Stopwatch s;
    s.Start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, g_width, g_height);

    // prepare view matrix
    double aspect = g_width/(double)g_height;
    identity(g_transformData.ModelViewMatrix);
    translate(g_transformData.ModelViewMatrix, -g_pan[0], -g_pan[1], -g_dolly);
    rotate(g_transformData.ModelViewMatrix, g_rotate[1], 1, 0, 0);
    rotate(g_transformData.ModelViewMatrix, g_rotate[0], 0, 1, 0);
    rotate(g_transformData.ModelViewMatrix, -90, 1, 0, 0);
    translate(g_transformData.ModelViewMatrix,
              -g_center[0], -g_center[1], -g_center[2]);
    perspective(g_transformData.ProjectionMatrix,
                45.0f, (float)aspect, 0.1f, 500.0f);
    multMatrix(g_transformData.ModelViewProjectionMatrix,
               g_transformData.ModelViewMatrix,
               g_transformData.ProjectionMatrix);

    glEnable(GL_DEPTH_TEST);

    s.Stop();
    float drawCpuTime = float(s.GetElapsed() * 1000.0f);

    glBindVertexArray(0);

    glUseProgram(0);

    // primitive counting
    glBeginQuery(GL_PRIMITIVES_GENERATED, g_queries[0]);
#if defined(GL_VERSION_3_3)
    glBeginQuery(GL_TIME_ELAPSED, g_queries[1]);
#endif

    // Update and bind transform state
    if (! g_transformUB) {
        glGenBuffers(1, &g_transformUB);
        glBindBuffer(GL_UNIFORM_BUFFER, g_transformUB);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(g_transformData), NULL, GL_STATIC_DRAW);
    };
    glBindBuffer(GL_UNIFORM_BUFFER, g_transformUB);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(g_transformData), &g_transformData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Update and bind lighting state
    struct Lighting {
        struct Light {
            float position[4];
            float ambient[4];
            float diffuse[4];
            float specular[4];
        } lightSource[2];
    } lightingData = {
       {{  { 0.5,  0.2f, 1.0f, 0.0f },
           { 0.1f, 0.1f, 0.1f, 1.0f },
           { 0.7f, 0.7f, 0.7f, 1.0f },
           { 0.8f, 0.8f, 0.8f, 1.0f } },

         { { -0.8f, 0.4f, -1.0f, 0.0f },
           {  0.0f, 0.0f,  0.0f, 1.0f },
           {  0.5f, 0.5f,  0.5f, 1.0f },
           {  0.8f, 0.8f,  0.8f, 1.0f } }}
    };
    if (! g_lightingUB) {
        glGenBuffers(1, &g_lightingUB);
        glBindBuffer(GL_UNIFORM_BUFFER, g_lightingUB);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(lightingData), NULL, GL_STATIC_DRAW);
    };
    glBindBuffer(GL_UNIFORM_BUFFER, g_lightingUB);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightingData), &lightingData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // draw stuff
    if (g_drawCageEdges)
        drawCageEdges();

    if (g_drawCageVertices)
        drawCageVertices();

    if (g_drawMode==0) {
        drawRefinedVerts();
    } else if (g_drawMode==1) {
        drawRefinedQuads();
    }

    drawComponents();

    g_hud.GetFrameBuffer()->ApplyImageShader();
    GLuint numPrimsGenerated = 0;
    GLuint timeElapsed = 0;

    glGetQueryObjectuiv(g_queries[0], GL_QUERY_RESULT, &numPrimsGenerated);
#if defined(GL_VERSION_3_3)
    glGetQueryObjectuiv(g_queries[1], GL_QUERY_RESULT, &timeElapsed);
#endif

    float drawGpuTime = timeElapsed / 1000.0f / 1000.0f;

    if (g_hud.IsVisible()) {
        g_fpsTimer.Stop();
        double fps = 1.0/g_fpsTimer.GetElapsed();
        g_fpsTimer.Start();

        g_hud.DrawString(10, -160, "Primitives : %d", numPrimsGenerated);
        g_hud.DrawString(10, -140, "Vertices   : %d", g_refTables->GetVertCount());
        g_hud.DrawString(10, -120, "Scheme     : %s", g_scheme==kBilinear ? "BILINEAR" : (g_scheme == kLoop ? "LOOP" : "CATMARK"));
        g_hud.DrawString(10, -100, "GPU Kernel : %.3f ms", g_gpuTime);
        g_hud.DrawString(10, -80,  "CPU Kernel : %.3f ms", g_cpuTime);
        g_hud.DrawString(10, -60,  "GPU Draw   : %.3f ms", drawGpuTime);
        g_hud.DrawString(10, -40,  "CPU Draw   : %.3f ms", drawCpuTime);
        g_hud.DrawString(10, -20,  "FPS        : %3.1f", fps);

        g_hud.Flush();
    }
    glFinish();

    //checkGLErrors("display leave");
}

//------------------------------------------------------------------------------
static void
#if GLFW_VERSION_MAJOR>=3
motion(GLFWwindow *, double dx, double dy) {
    int x=(int)dx, y=(int)dy;
#else
motion(int x, int y) {
#endif
    if (g_hud.MouseCapture()) {
        // check gui
        g_hud.MouseMotion(x, y);
    } else if (g_mbutton[0] && !g_mbutton[1] && !g_mbutton[2]) {
        // orbit
        g_rotate[0] += x - g_prev_x;
        g_rotate[1] += y - g_prev_y;
    } else if (!g_mbutton[0] && !g_mbutton[1] && g_mbutton[2]) {
        // pan
        g_pan[0] -= g_dolly*(x - g_prev_x)/g_width;
        g_pan[1] += g_dolly*(y - g_prev_y)/g_height;
    } else if ((g_mbutton[0] && !g_mbutton[1] && g_mbutton[2]) or
               (!g_mbutton[0] && g_mbutton[1] && !g_mbutton[2])) {
        // dolly
        g_dolly -= g_dolly*0.01f*(x - g_prev_x);
        if(g_dolly <= 0.01) g_dolly = 0.01f;
    }

    g_prev_x = x;
    g_prev_y = y;
}

//------------------------------------------------------------------------------
static void
#if GLFW_VERSION_MAJOR>=3
mouse(GLFWwindow *, int button, int state, int /* mods */) {
#else
mouse(int button, int state) {
#endif

    if (state == GLFW_RELEASE)
        g_hud.MouseRelease();

    if (button == 0 && state == GLFW_PRESS && g_hud.MouseClick(g_prev_x, g_prev_y))
        return;

    if (button < 3) {
        g_mbutton[button] = (state == GLFW_PRESS);
    }
}

//------------------------------------------------------------------------------
static void
#if GLFW_VERSION_MAJOR>=3
reshape(GLFWwindow *, int width, int height) {
#else
reshape(int width, int height) {
#endif

    g_width = width;
    g_height = height;

    int windowWidth = g_width, windowHeight = g_height;
#if GLFW_VERSION_MAJOR>=3
    // window size might not match framebuffer size on a high DPI display
    glfwGetWindowSize(g_window, &windowWidth, &windowHeight);
#endif
    g_hud.Rebuild(windowWidth, windowHeight);
}

//------------------------------------------------------------------------------
#if GLFW_VERSION_MAJOR>=3
void windowClose(GLFWwindow*) {
    g_running = false;
}
#else
int windowClose() {
    g_running = false;
    return GL_TRUE;
}
#endif

//------------------------------------------------------------------------------
static void
toggleFullScreen() {
    // XXXX manuelk : to re-implement from glut
}

//------------------------------------------------------------------------------
static void
#if GLFW_VERSION_MAJOR>=3
keyboard(GLFWwindow *, int key, int /* scancode */, int event, int /* mods */) {
#else
#define GLFW_KEY_ESCAPE GLFW_KEY_ESC
keyboard(int key, int event) {
#endif

    if (event == GLFW_RELEASE) return;
    if (g_hud.KeyDown(tolower(key))) return;

    switch (key) {
        case 'Q': g_running = 0; break;
        case 'F': fitFrame(); break;
        case GLFW_KEY_TAB: toggleFullScreen(); break;
        case GLFW_KEY_ESCAPE: g_hud.SetVisible(!g_hud.IsVisible()); break;
//        case 'X': g_hud.GetFrameBuffer()->Screenshot(); break;
    }
}

//------------------------------------------------------------------------------
static void
rebuildOsdMesh()
{
    createMesh( g_defaultShapes[ g_currentShape ].data, g_level, g_defaultShapes[ g_currentShape ].scheme );
}

static void
callbackLevel(int l)
{
    g_level = l;
    rebuildOsdMesh();
}

static void
callbackModel(int m)
{
    if (m < 0)
        m = 0;

    if (m >= (int)g_defaultShapes.size())
        m = (int)g_defaultShapes.size() - 1;

    g_currentShape = m;
    rebuildOsdMesh();
}

static void
callbackCheckBox(bool checked, int button)
{
    switch (button) {
    case kHUD_CB_DISPLAY_CAGE_EDGES:
        g_drawCageEdges = checked;
        break;
    case kHUD_CB_DISPLAY_CAGE_VERTS:
        g_drawCageVertices = checked;
        break;
    case kHUD_CB_ANIMATE_VERTICES:
        g_moveScale = checked;
        break;
    case kHUD_CB_DISPLAY_PATCH_COLOR:
        g_displayPatchColor = checked;
        break;
    }
}

static void
callbackDrawMode(int m)
{
    g_drawMode = m;
}

static void
callbackScale(float value, int)
{
    g_font->SetFontScale(value);
}

//------------------------------------------------------------------------------
static void
initHUD()
{
    int windowWidth = g_width, windowHeight = g_height;
#if GLFW_VERSION_MAJOR>=3
    // window size might not match framebuffer size on a high DPI display
    glfwGetWindowSize(g_window, &windowWidth, &windowHeight);
#endif


    g_hud.Init(windowWidth, windowHeight);

    g_hud.SetFrameBuffer(new GLFrameBuffer);

    g_hud.AddCheckBox("Cage Edges (H)", g_drawCageEdges != 0,
                      10, 10, callbackCheckBox, kHUD_CB_DISPLAY_CAGE_EDGES, 'h');
    g_hud.AddCheckBox("Cage Verts (J)", g_drawCageVertices != 0,
                      10, 30, callbackCheckBox, kHUD_CB_DISPLAY_CAGE_VERTS, 'j');
    g_hud.AddCheckBox("Animate vertices (M)", g_moveScale != 0,
                      10, 50, callbackCheckBox, kHUD_CB_ANIMATE_VERTICES, 'm');

    int drawing_pulldown = g_hud.AddPullDown("Draw Mode (W)", 200, 10, 250, callbackDrawMode, 'w');
    g_hud.AddPullDownButton(drawing_pulldown, "Vertices", 0, g_drawMode==0);
    g_hud.AddPullDownButton(drawing_pulldown, "Quads", 1, g_drawMode==1);

    g_hud.AddSlider("Font Scale", 0.0f, 0.25f, 0.03f,
                    -850, -100, 100, false, callbackScale, 0);

    for (int i = 1; i < 11; ++i) {
        char level[16];
        sprintf(level, "Lv. %d", i);
        g_hud.AddRadioButton(3, level, i==g_level, 10, 210+i*20, callbackLevel, i, '0'+(i%10));
    }

    int shapes_pulldown = g_hud.AddPullDown("Shape (N)", -300, 10, 300, callbackModel, 'n');
    for (int i = 0; i < (int)g_defaultShapes.size(); ++i) {
        g_hud.AddPullDownButton(shapes_pulldown, g_defaultShapes[i].name.c_str(),i, (g_currentShape==i));
    }

    if (not g_font) {
        g_font = new GLFont( g_hud.GetFontTexture() );
    }
}


//------------------------------------------------------------------------------
static void
initGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glGenQueries(2, g_queries);

    glGenVertexArrays(1, &g_vao);
    glGenVertexArrays(1, &g_cageVertexVAO);
    glGenVertexArrays(1, &g_cageEdgeVAO);
    glGenBuffers(1, &g_cageVertexVBO);
    glGenBuffers(1, &g_cageEdgeVBO);

    glGenVertexArrays(1, &g_refinedVAO);
}

//------------------------------------------------------------------------------
static void
uninitGL() {

    glDeleteQueries(2, g_queries);

    glDeleteBuffers(1, &g_cageVertexVBO);
    glDeleteBuffers(1, &g_cageEdgeVBO);
    glDeleteVertexArrays(1, &g_vao);
    glDeleteVertexArrays(1, &g_cageVertexVAO);
    glDeleteVertexArrays(1, &g_cageEdgeVAO);

    glDeleteVertexArrays(1, &g_refinedVAO);
}

//------------------------------------------------------------------------------
static void
idle() {

    if (not g_freeze) {
        g_frame++;
        updateGeom();
    }

    if (g_repeatCount != 0 and g_frame >= g_repeatCount)
        g_running = 0;
}


//------------------------------------------------------------------------------
static void
setGLCoreProfile()
{
#if GLFW_VERSION_MAJOR>=3
    #define glfwOpenWindowHint glfwWindowHint
    #define GLFW_OPENGL_VERSION_MAJOR GLFW_CONTEXT_VERSION_MAJOR
    #define GLFW_OPENGL_VERSION_MINOR GLFW_CONTEXT_VERSION_MINOR
#endif

    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if not defined(__APPLE__)
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
#ifdef OPENSUBDIV_HAS_GLSL_COMPUTE
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
#else
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
#endif

#else
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
#endif
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}


//------------------------------------------------------------------------------
int main(int argc, char ** argv)
{
    bool fullscreen = false;
    std::string str;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-d"))
            g_level = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-c"))
            g_repeatCount = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-f"))
            fullscreen = true;
        else {
            std::ifstream ifs(argv[1]);
            if (ifs) {
                std::stringstream ss;
                ss << ifs.rdbuf();
                ifs.close();
                str = ss.str();
                g_defaultShapes.push_back(SimpleShape(str.c_str(), argv[1], kCatmark));
            }
        }
    }
    initializeShapes();

    if (not glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return 1;
    }

    static const char windowTitle[] = "OpenSubdiv glViewer";

#define CORE_PROFILE
#ifdef CORE_PROFILE
    setGLCoreProfile();
#endif

#if GLFW_VERSION_MAJOR>=3
    if (fullscreen) {

        g_primary = glfwGetPrimaryMonitor();

        // apparently glfwGetPrimaryMonitor fails under linux : if no primary,
        // settle for the first one in the list
        if (not g_primary) {
            int count=0;
            GLFWmonitor ** monitors = glfwGetMonitors(&count);

            if (count)
                g_primary = monitors[0];
        }

        if (g_primary) {
            GLFWvidmode const * vidmode = glfwGetVideoMode(g_primary);
            g_width = vidmode->width;
            g_height = vidmode->height;
        }
    }

    if (not (g_window=glfwCreateWindow(g_width, g_height, windowTitle,
                                       fullscreen and g_primary ? g_primary : NULL, NULL))) {
        printf("Failed to open window.\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(g_window);

    // accommocate high DPI displays (e.g. mac retina displays)
    glfwGetFramebufferSize(g_window, &g_width, &g_height);
    glfwSetFramebufferSizeCallback(g_window, reshape);

    glfwSetKeyCallback(g_window, keyboard);
    glfwSetCursorPosCallback(g_window, motion);
    glfwSetMouseButtonCallback(g_window, mouse);
    glfwSetWindowCloseCallback(g_window, windowClose);
#else
    if (glfwOpenWindow(g_width, g_height, 8, 8, 8, 8, 24, 8,
                       fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_FALSE) {
        printf("Failed to open window.\n");
        glfwTerminate();
        return 1;
    }
    glfwSetWindowTitle(windowTitle);
    glfwSetKeyCallback(keyboard);
    glfwSetMousePosCallback(motion);
    glfwSetMouseButtonCallback(mouse);
    glfwSetWindowSizeCallback(reshape);
    glfwSetWindowCloseCallback(windowClose);
#endif


#if defined(OSD_USES_GLEW)
#ifdef CORE_PROFILE
    // this is the only way to initialize glew correctly under core profile context.
    glewExperimental = true;
#endif
    if (GLenum r = glewInit() != GLEW_OK) {
        printf("Failed to initialize glew. Error = %s\n", glewGetErrorString(r));
        exit(1);
    }
#ifdef CORE_PROFILE
    // clear GL errors which was generated during glewInit()
    glGetError();
#endif
#endif

    initGL();
    linkDefaultProgram();

    glfwSwapInterval(0);

    initHUD();
    rebuildOsdMesh();

    checkGLErrors("before loop");
    while (g_running) {
        idle();
        display();

#if GLFW_VERSION_MAJOR>=3
        glfwPollEvents();
        glfwSwapBuffers(g_window);
#else
        glfwSwapBuffers();
#endif

        glFinish();
    }

    uninitGL();
    glfwTerminate();
}

//------------------------------------------------------------------------------
