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

#include <common/vtr_utils.h>
#include <common/hbr_utils.h>

#include "../common/stopwatch.h"
#include "../common/simple_math.h"
#include "../common/gl_common.h"
#include "../common/gl_hud.h"
#include "../common/patchColors.h"

#include "init_shapes.h"
#include "gl_mesh.h"
#include "gl_fontutils.h"

#include <typeinfo>
#include <cfloat>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>

//------------------------------------------------------------------------------
int g_level = 2,
    g_currentShape = 0;

enum HudCheckBox { kHUD_CB_DISPLAY_CAGE_EDGES,
                   kHUD_CB_DISPLAY_CAGE_VERTS,
                   kHUD_CB_ANIMATE_VERTICES,
                   kHUD_CB_DISPLAY_PATCH_COLOR };

enum DrawMode { kDRAW_NONE = 0,
                kDRAW_VERTICES,
                kDRAW_WIREFRAME,
                kDRAW_FACES };


int   g_frame = 0,
      g_repeatCount = 0;

// GUI variables
int   g_fullscreen = 0,
      g_mbutton[3] = {0, 0, 0},
      g_running = 1;

int   g_displayPatchColor = 1,
      g_drawCageEdges = 1,
      g_drawCageVertices = 0,
      g_HbrDrawMode = kDRAW_NONE,
      g_HbrDrawVertIDs = false,
      g_HbrDrawFaceIDs = false,
      g_VtrDrawMode = kDRAW_FACES,
      g_VtrDrawVertIDs = false,
      g_VtrDrawEdgeIDs = false,
      g_VtrDrawFaceIDs = false;

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

GLuint g_queries[2] = {0, 0};

GLuint g_transformUB = 0,
       g_lightingUB = 0;

struct Transform {
    float ModelViewMatrix[16];
    float ProjectionMatrix[16];
    float ModelViewProjectionMatrix[16];
} g_transformData;

//------------------------------------------------------------------------------
// Vertex class implementation
struct Vertex {

    Vertex() { /* _pos[0]=_pos[1]=_pos[2]=0.0f; */ }

    Vertex( int /*i*/ ) { }

    Vertex( float x, float y, float z ) { _pos[0]=x; _pos[1]=y; _pos[2]=z; }

    Vertex( const Vertex & src ) { _pos[0]=src._pos[0]; _pos[1]=src._pos[1]; _pos[2]=src._pos[2]; }

   ~Vertex( ) { }

    void AddWithWeight(const Vertex& src, float weight) {
        _pos[0]+=weight*src._pos[0];
        _pos[1]+=weight*src._pos[1];
        _pos[2]+=weight*src._pos[2];
    }

    void AddVaryingWithWeight(const Vertex& , float) { }

    void Clear( void * =0 ) { _pos[0]=_pos[1]=_pos[2]=0.0f; }

    void SetPosition(float x, float y, float z) { _pos[0]=x; _pos[1]=y; _pos[2]=z; }

    void ApplyVertexEdit(const OpenSubdiv::HbrVertexEdit<Vertex> & edit) {
        const float *src = edit.GetEdit();
        switch(edit.GetOperation()) {
          case OpenSubdiv::HbrHierarchicalEdit<Vertex>::Set:
            _pos[0] = src[0];
            _pos[1] = src[1];
            _pos[2] = src[2];
            break;
          case OpenSubdiv::HbrHierarchicalEdit<Vertex>::Add:
            _pos[0] += src[0];
            _pos[1] += src[1];
            _pos[2] += src[2];
            break;
          case OpenSubdiv::HbrHierarchicalEdit<Vertex>::Subtract:
            _pos[0] -= src[0];
            _pos[1] -= src[1];
            _pos[2] -= src[2];
            break;
        }
    }

    void ApplyMovingVertexEdit(const OpenSubdiv::HbrMovingVertexEdit<Vertex> &) { }

    const float * GetPos() const { return _pos; }

private:
    float _pos[3];
};

//------------------------------------------------------------------------------
typedef OpenSubdiv::HbrMesh<Vertex>           Hmesh;
typedef OpenSubdiv::HbrFace<Vertex>           Hface;
typedef OpenSubdiv::HbrVertex<Vertex>         Hvertex;
typedef OpenSubdiv::HbrHalfedge<Vertex>       Hhalfedge;

typedef OpenSubdiv::FarRefineTables               FRefineTables;
typedef OpenSubdiv::FarRefineTablesFactory<Shape> FRefineTablesFactory;


static GLMesh g_base_glmesh,
              g_hbr_glmesh,
              g_vtr_glmesh;

//------------------------------------------------------------------------------
// generate display IDs for Hbr faces
static void
createFaceNumbers(std::vector<Hface const *> faces) {

    static char buf[16];

    for (int i=0; i<(int)faces.size(); ++i) {

        Hface const * f = faces[i];

        Vertex center(0.0f, 0.0f, 0.0f);

        int nv = f->GetNumVertices();
        float weight = 1.0f / nv;

        for (int j=0; j<nv; ++j) {
            center.AddWithWeight(f->GetVertex(j)->GetData(), weight);
        }

        snprintf(buf, 16, "%d", f->GetID());
        g_font->Print3D(center.GetPos(), buf, 2);
    }
}

//------------------------------------------------------------------------------
// generate display IDs for Hbr verts
static void
createVertNumbers(std::vector<Hface const *> faces) {

    assert(not faces.empty());

    static char buf[16];

    std::vector<Hvertex const *> verts(faces.size()*4, 0);

    for (int i=0; i<(int)faces.size(); ++i) {

        Hface const * f = faces[i];

        int nv = f->GetNumVertices();
        for (int j=0; j<nv; ++j) {
            Hvertex const * v = f->GetVertex(j);
            verts[v->GetID()] = v;
        }
    }

    for (int i=0; i<(int)verts.size(); ++i) {

        if (verts[i]) {
            snprintf(buf, 16, "%d", verts[i]->GetID());
            g_font->Print3D(verts[i]->GetData().GetPos(), buf, 1);
        }
    }
}

//------------------------------------------------------------------------------
static void
createHbrMesh(Shape * shape, int maxlevel) {

    // create Hbr mesh using functions from hbr_utils
    Hmesh * hmesh = createMesh<Vertex>(shape->scheme, /*fvarwidth*/ 0);

    createVerticesWithPositions<Vertex>(shape, hmesh);

    createTopology<Vertex>(shape, hmesh, shape->scheme);

    std::vector<Hface const *>   coarseFaces,  // list of Hbr coarse faces 
                                 refinedFaces; // list of Hbr faces refined at maxlevel

    int nfaces = hmesh->GetNumFaces();

    { // create control cage GL mesh
        coarseFaces.resize(nfaces);
        for (int i=0; i<nfaces; ++i) {
            coarseFaces[i] = hmesh->GetFace(i);
        }

        GLMesh::Options coarseOptions;
        coarseOptions.vertColorMode=GLMesh::VERTCOLOR_BY_SHARPNESS;
        coarseOptions.edgeColorMode=GLMesh::EDGECOLOR_BY_SHARPNESS;
        coarseOptions.faceColorMode=GLMesh::FACECOLOR_SOLID;

        g_base_glmesh.Initialize(coarseOptions, coarseFaces);
    }

    { // create maxlevel refined GL mesh

        // refine the Hbr mesh uniformly
        for (int level=0, firstface=0; level<=maxlevel; ++level ) {

            if (level==maxlevel) {

                refinedFaces.resize(nfaces-firstface);
                for (int i=firstface, ofs=0; i<nfaces; ++i) {
                    refinedFaces[ofs++]=hmesh->GetFace(i);
                }
            } else {

                for (int i=firstface; i<nfaces; ++i) {
                    Hface * f = hmesh->GetFace(i);
                    assert(f->GetDepth()==level);
                    if (not f->IsHole()) {
                        f->Refine();
                    }
                }
            }

            // Hbr allocates faces sequentially, skip faces that have
            // already been refined.
            firstface = nfaces;
            nfaces = hmesh->GetNumFaces();
        }

        if (g_HbrDrawVertIDs) {
            createVertNumbers(refinedFaces);
        }
        
        // Hbr is a half-edge rep, so edges do not have unique IDs that
        // can be displayed

        if (g_HbrDrawFaceIDs) {
            createFaceNumbers(refinedFaces);
        }

        GLMesh::Options refinedOptions;
        refinedOptions.vertColorMode=GLMesh::VERTCOLOR_BY_SHARPNESS;
        refinedOptions.edgeColorMode=GLMesh::EDGECOLOR_BY_SHARPNESS;
        refinedOptions.faceColorMode=GLMesh::FACECOLOR_SOLID;

        g_hbr_glmesh.Initialize(refinedOptions, refinedFaces);
        g_hbr_glmesh.SetDiffuseColor(1.0f,0.75f,0.9f, 1.0f);
    }

    delete hmesh;
}

//------------------------------------------------------------------------------
// generate display IDs for Vtr verts
static void
createVertNumbers(OpenSubdiv::FarRefineTables const & refTables,
    std::vector<Vertex> const & vertexBuffer) {

    int maxlevel = refTables.GetMaxLevel(),
        firstvert = 0;

    for (int i=0; i<maxlevel; ++i) {
        firstvert += refTables.GetNumVertices(i);
    }

    static char buf[16];
    for (int i=firstvert; i<(int)vertexBuffer.size(); ++i) {

        snprintf(buf, 16, "%d", i);
        g_font->Print3D(vertexBuffer[i].GetPos(), buf, 1);
    }
}

//------------------------------------------------------------------------------
// generate display IDs for Vtr edges
static void
createEdgeNumbers(OpenSubdiv::FarRefineTables const & refTables,
    std::vector<Vertex> const & vertexBuffer) {

    int maxlevel = refTables.GetMaxLevel(),
        firstvert = 0;

    for (int i=0; i<maxlevel; ++i) {
        firstvert += refTables.GetNumVertices(i);
    }
    
    static char buf[16];
    for (int i=0; i<refTables.GetNumEdges(maxlevel); ++i) {

        Vertex center(0.0f, 0.0f, 0.0f);
        
        OpenSubdiv::FarRefineTables::IndexArray const verts =
            refTables.GetEdgeVertices(maxlevel, i);
        assert(verts.size()==2);

        center.AddWithWeight(vertexBuffer[firstvert+verts[0]], 0.5f);
        center.AddWithWeight(vertexBuffer[firstvert+verts[1]], 0.5f);

        snprintf(buf, 16, "%d", i);
        g_font->Print3D(center.GetPos(), buf, 3);    
    }
}

//------------------------------------------------------------------------------
// generate display IDs for Vtr faces
static void
createFaceNumbers(OpenSubdiv::FarRefineTables const & refTables,
    std::vector<Vertex> const & vertexBuffer) {

    int maxlevel = refTables.GetMaxLevel(),
        firstvert = 0;

    for (int i=0; i<maxlevel; ++i) {
        firstvert += refTables.GetNumVertices(i);
    }
    
    static char buf[16];
    for (int i=0; i<refTables.GetNumFaces(maxlevel); ++i) {

        Vertex center(0.0f, 0.0f, 0.0f);
        
        OpenSubdiv::FarRefineTables::IndexArray const verts =
            refTables.GetFaceVertices(maxlevel, i);
        
        float weight = 1.0f / (float)verts.size();
        
        for (int j=0; j<verts.size(); ++j) {
            center.AddWithWeight(vertexBuffer[firstvert+verts[j]], weight);
        }

        snprintf(buf, 16, "%d", i);
        g_font->Print3D(center.GetPos(), buf, 2);
    }
}

//------------------------------------------------------------------------------
static void
createVtrMesh(Shape * shape, int maxlevel) {

    // create Vtr mesh (topology)
    OpenSubdiv::SdcType       sdctype = GetSdcType(*shape);
    OpenSubdiv::SdcOptions sdcoptions = GetSdcOptions(*shape);

    OpenSubdiv::FarRefineTablesFactory<Shape> refFactory(sdctype, sdcoptions);
    
    OpenSubdiv::FarRefineTables * refTables = refFactory.Create(*shape, maxlevel, /*full topology*/ true);

    // create vertex primvar data buffer
    std::vector<Vertex> vertexBuffer(refTables->GetNumVerticesTotal());
    Vertex * verts = &vertexBuffer[0];

    // copy coarse vertices positions
    int ncoarseverts = shape->GetNumVertices();
    for (int i=0; i<ncoarseverts; ++i) {
        float * ptr = &shape->verts[i*3];
        verts[i].SetPosition(ptr[0], ptr[1], ptr[2]);
    }

    // populate buffer with Vtr interpolated vertex data
    refTables->Interpolate<Vertex>(verts, verts + ncoarseverts);


    if (g_VtrDrawVertIDs) {
        createVertNumbers(*refTables, vertexBuffer);
    }

    if (g_VtrDrawEdgeIDs) {
        createEdgeNumbers(*refTables, vertexBuffer);
    }

    if (g_VtrDrawFaceIDs) {
        createFaceNumbers(*refTables, vertexBuffer);
    }

    GLMesh::Options options;
    options.vertColorMode=GLMesh::VERTCOLOR_BY_SHARPNESS;
    options.edgeColorMode=GLMesh::EDGECOLOR_BY_SHARPNESS;
    options.faceColorMode=GLMesh::FACECOLOR_SOLID;

    g_vtr_glmesh.Initialize(options, *refTables, (float *)&verts[0]);
    g_vtr_glmesh.SetDiffuseColor(0.75f, 0.9f, 1.0f, 1.0f);

    delete refTables;
}

//------------------------------------------------------------------------------
static void
createMeshes(ShapeDesc const & desc, int maxlevel) {

    if (not g_font) {
        g_font = new GLFont(g_hud.GetFontTexture());
    }
    g_font->Clear();

    Shape * shape = Shape::parseObj(desc.data.c_str(), desc.scheme);

    createHbrMesh(shape, maxlevel);
    createVtrMesh(shape, maxlevel);

    delete shape;
}

//------------------------------------------------------------------------------
static void
fitFrame() {

    g_pan[0] = g_pan[1] = 0;
    g_dolly = g_size;
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

    // Update and bind transform state ---------------------
    if (! g_transformUB) {
        glGenBuffers(1, &g_transformUB);
        glBindBuffer(GL_UNIFORM_BUFFER, g_transformUB);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(g_transformData), NULL, GL_STATIC_DRAW);
    };
    glBindBuffer(GL_UNIFORM_BUFFER, g_transformUB);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(g_transformData), &g_transformData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Update and bind lighting state ----------------------
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

    // Draw stuff ------------------------------------------

    // control cage edges & verts
    if (g_drawCageVertices) {
        g_base_glmesh.Draw(GLMesh::COMP_VERT, g_transformUB, g_lightingUB);
    }

    if (g_drawCageEdges) {
        g_base_glmesh.Draw(GLMesh::COMP_EDGE, g_transformUB, g_lightingUB);
    }

    // Hbr mesh
    if (g_HbrDrawMode!=kDRAW_NONE) {

        GLMesh::Component comp;
        switch (g_HbrDrawMode) {
            case kDRAW_VERTICES  : comp=GLMesh::COMP_VERT; break;
            case kDRAW_WIREFRAME : comp=GLMesh::COMP_EDGE; break;
            case kDRAW_FACES     : comp=GLMesh::COMP_FACE; break;
            default:
                assert(0);
        }
        g_hbr_glmesh.Draw(comp, g_transformUB, g_lightingUB);
    }

    // Vtr mesh
    if (g_VtrDrawMode!=kDRAW_NONE) {

        GLMesh::Component comp;
        switch (g_VtrDrawMode) {
            case kDRAW_VERTICES  : comp=GLMesh::COMP_VERT; break;
            case kDRAW_WIREFRAME : comp=GLMesh::COMP_EDGE; break;
            case kDRAW_FACES     : comp=GLMesh::COMP_FACE; break;
            default:
                assert(0);
        }
        g_vtr_glmesh.Draw(comp, g_transformUB, g_lightingUB);
    }
    
    assert(g_font);
    g_font->Draw(g_transformUB);

    // -----------------------------------------------------

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

        static char const * schemeNames[3] = { "BILINEAR", "CATMARK", "LOOP" };

        g_hud.DrawString(10, -140, "Primitives : %d", numPrimsGenerated);
        g_hud.DrawString(10, -120, "Scheme     : %s", schemeNames[ g_shapes[g_currentShape].scheme ]);
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
    }
}

//------------------------------------------------------------------------------
static void
rebuildOsdMeshes() {

    createMeshes(g_shapes[ g_currentShape ], g_level);
}

static void
callbackLevel(int l) {

    g_level = l;
    rebuildOsdMeshes();
}

static void
callbackModel(int m) {

    if (m < 0)
        m = 0;

    if (m >= (int)g_shapes.size())
        m = (int)g_shapes.size() - 1;

    g_currentShape = m;
    rebuildOsdMeshes();
}

static void
callbackCheckBox(bool checked, int button) {

    switch (button) {
        case kHUD_CB_DISPLAY_CAGE_EDGES : g_drawCageEdges = checked; break;
        case kHUD_CB_DISPLAY_CAGE_VERTS : g_drawCageVertices = checked; break;
        case kHUD_CB_DISPLAY_PATCH_COLOR: g_displayPatchColor = checked; break;
    }
}

static void
callbackHbrDrawMode(int m) {

    g_HbrDrawMode = m;
}

static void
callbackVtrDrawMode(int m) {

    g_VtrDrawMode = m;
}

static void
callbackDrawIDs(bool checked, int button) {

    switch (button) {
        case 0: g_HbrDrawVertIDs = checked; break;
        case 1: g_HbrDrawFaceIDs = checked; break;

        case 2: g_VtrDrawVertIDs = checked; break;
        case 3: g_VtrDrawEdgeIDs = checked; break;
        case 4: g_VtrDrawFaceIDs = checked; break;
        default: break;
    }
    rebuildOsdMeshes();
}

static void
callbackScale(float value, int) {

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

    g_hud.AddCheckBox("Cage Edges (e)", g_drawCageEdges != 0,
                      10, 10, callbackCheckBox, kHUD_CB_DISPLAY_CAGE_EDGES, 'e');
    g_hud.AddCheckBox("Cage Verts (r)", g_drawCageVertices != 0,
                      10, 30, callbackCheckBox, kHUD_CB_DISPLAY_CAGE_VERTS, 'r');

    int pulldown = g_hud.AddPullDown("Hbr Draw Mode (h)", 10, 75, 250, callbackHbrDrawMode, 'h');
    g_hud.AddPullDownButton(pulldown, "None",      0, g_HbrDrawMode==kDRAW_NONE);
    g_hud.AddPullDownButton(pulldown, "Vertices",  1, g_HbrDrawMode==kDRAW_VERTICES);
    g_hud.AddPullDownButton(pulldown, "Wireframe", 2, g_HbrDrawMode==kDRAW_WIREFRAME);
    g_hud.AddPullDownButton(pulldown, "Faces",     3, g_HbrDrawMode==kDRAW_FACES);

    g_hud.AddCheckBox("Vert IDs", g_HbrDrawVertIDs, 10, 95, callbackDrawIDs, 0);
    g_hud.AddCheckBox("Face IDs", g_HbrDrawFaceIDs, 10, 115, callbackDrawIDs, 1);

    pulldown = g_hud.AddPullDown("Vtr Draw Mode (v)", 10, 175, 250, callbackVtrDrawMode, 'v');
    g_hud.AddPullDownButton(pulldown, "None",      0, g_VtrDrawMode==kDRAW_NONE);
    g_hud.AddPullDownButton(pulldown, "Vertices",  1, g_VtrDrawMode==kDRAW_VERTICES);
    g_hud.AddPullDownButton(pulldown, "Wireframe", 2, g_VtrDrawMode==kDRAW_WIREFRAME);
    g_hud.AddPullDownButton(pulldown, "Faces",     3, g_VtrDrawMode==kDRAW_FACES);

    g_hud.AddCheckBox("Vert IDs", g_VtrDrawVertIDs, 10, 195, callbackDrawIDs, 2);
    g_hud.AddCheckBox("Edge IDs", g_VtrDrawEdgeIDs, 10, 215, callbackDrawIDs, 3);
    g_hud.AddCheckBox("Face IDs", g_VtrDrawFaceIDs, 10, 235, callbackDrawIDs, 4);

    g_hud.AddSlider("Font Scale", 0.0f, 0.1f, 0.025f,
                    -900, -50, 100, false, callbackScale, 0);

    for (int i = 1; i < 11; ++i) {
        char level[16];
        sprintf(level, "Lv. %d", i);
        g_hud.AddRadioButton(3, level, i==g_level, 10, 300+i*20, callbackLevel, i, '0'+(i%10));
    }

    int shapes_pulldown = g_hud.AddPullDown("Shape (N)", -300, 10, 300, callbackModel, 'n');
    for (int i = 0; i < (int)g_shapes.size(); ++i) {
        g_hud.AddPullDownButton(shapes_pulldown, g_shapes[i].name.c_str(),i, (g_currentShape==i));
    }

    if (not g_font) {
        g_font = new GLFont( g_hud.GetFontTexture() );
    }
}

//------------------------------------------------------------------------------
static void
initGL() {

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glGenQueries(2, g_queries);
}

//------------------------------------------------------------------------------
static void
uninitGL() {

    glDeleteQueries(2, g_queries);
}

//------------------------------------------------------------------------------
static void
idle() {

    if (g_repeatCount != 0 and g_frame >= g_repeatCount)
        g_running = 0;
}


//------------------------------------------------------------------------------
static void
setGLCoreProfile() {

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
                g_shapes.push_back(ShapeDesc(str.c_str(), argv[1], kCatmark));
            }
        }
    }
    initShapes();

    if (not glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return 1;
    }

    static const char windowTitle[] = "OpenSubdiv vtrViewer";

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

    glfwSwapInterval(0);

    initHUD();
    rebuildOsdMeshes();

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
