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

#ifndef OSD_GL_MESH_H
#define OSD_GL_MESH_H

#include "../version.h"

#include "../osd/mesh.h"
#include "../osd/glDrawContext.h"
#include "../osd/vertexDescriptor.h"

#ifdef OPENSUBDIV_HAS_OPENCL
#  include "../osd/clComputeController.h"
#  include "../osd/opencl.h"
#endif

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

typedef OsdMeshInterface<OsdGLDrawContext> OsdGLMeshInterface;

template <class VERTEX_BUFFER, class COMPUTE_CONTROLLER>
class OsdMesh<VERTEX_BUFFER, COMPUTE_CONTROLLER, OsdGLDrawContext> : public OsdGLMeshInterface {
public:
    typedef VERTEX_BUFFER VertexBuffer;
    typedef COMPUTE_CONTROLLER ComputeController;
    typedef typename ComputeController::ComputeContext ComputeContext;
    typedef OsdGLDrawContext DrawContext;
    typedef typename DrawContext::VertexBufferBinding VertexBufferBinding;

    OsdMesh(ComputeController * computeController,
            FarRefineTables * refTables,
            int numVertexElements,
            int numVaryingElements,
            int level,
            OsdMeshBitset bits) :

            _refTables(refTables),
            _patchTables(0),
            _vertexBuffer(0),
            _varyingBuffer(0),
            _computeContext(0),
            _computeController(computeController),
            _drawContext(0)
    {

        OsdGLMeshInterface::refineMesh(*_refTables, level, bits.test(MeshAdaptive));

        int numElements =
            initializeVertexBuffers(numVertexElements, numVaryingElements, bits);

        initializeComputeContext(numVertexElements, numVaryingElements);

        initializeDrawContext(numElements, bits);
    }

    OsdMesh(ComputeController * computeController,
            FarRefineTables * refTables,
            VertexBuffer * vertexBuffer,
            VertexBuffer * varyingBuffer,
            ComputeContext * computeContext,
            DrawContext * drawContext) :

            _refTables(refTables),
            _vertexBuffer(vertexBuffer),
            _varyingBuffer(varyingBuffer),
            _computeContext(computeContext),
            _computeController(computeController),
            _drawContext(drawContext)
    {
        _drawContext->UpdateVertexTexture(_vertexBuffer);
    }

    virtual ~OsdMesh() {
        delete _refTables;
        delete _patchTables;
        delete _vertexBuffer;
        delete _varyingBuffer;
        delete _computeContext;
        delete _drawContext;
    }

    virtual int GetNumVertices() const {
        assert(_refTables);
        return OsdGLMeshInterface::getNumVertices(*_refTables);
    }


    virtual void UpdateVertexBuffer(float const *vertexData, int startVertex, int numVerts) {
        _vertexBuffer->UpdateData(vertexData, startVertex, numVerts);
    }

    virtual void UpdateVaryingBuffer(float const *varyingData, int startVertex, int numVerts) {
        _varyingBuffer->UpdateData(varyingData, startVertex, numVerts);
    }

    virtual void Refine() {
        _computeController->Compute(_computeContext, _kernelBatches, _vertexBuffer, _varyingBuffer);
    }

    virtual void Refine(OsdVertexBufferDescriptor const * vertexDesc,
                        OsdVertexBufferDescriptor const * varyingDesc,
                        bool interleaved) {
        _computeController->Compute(_computeContext, _kernelBatches,
                                    _vertexBuffer, (interleaved ? _vertexBuffer : _varyingBuffer),
                                    vertexDesc, varyingDesc);
    }

    virtual void Synchronize() {
        _computeController->Synchronize();
    }

    virtual VertexBufferBinding BindVertexBuffer() {
        return _vertexBuffer->BindVBO();
    }

    virtual VertexBufferBinding BindVaryingBuffer() {
        return _varyingBuffer->BindVBO();
    }

    virtual DrawContext * GetDrawContext() {
        return _drawContext;
    }

    virtual VertexBuffer * GetVertexBuffer() {
        return _vertexBuffer;
    }

    virtual VertexBuffer * GetVaryingBuffer() {
        return _varyingBuffer;
    }

    virtual FarRefineTables const * GetRefineTables() const {
        return _refTables;
    }

    virtual void SetFVarDataChannel(int fvarWidth, std::vector<float> const & fvarData) {

        FarPatchTables::FVarPatchTables const * fvarPatchTables =
            _patchTables->GetFVarPatchTables();

        if (_drawContext and fvarPatchTables) {
            _drawContext->SetFVarDataTexture(*fvarPatchTables, fvarWidth, fvarData);
        }
    }

private:

    void initializeComputeContext(int numVertexElements,
        int numVaryingElements ) {

        assert(_refTables);

        FarStencilTablesFactory::Options options;
        options.generateOffsets=true;
        options.generateAllLevels=_refTables->IsUniform() ? false : true;

        FarStencilTables const * vertexStencils=0, * varyingStencils=0;

        if (numVertexElements>0) {

            vertexStencils = FarStencilTablesFactory::Create(*_refTables, options);

            _kernelBatches.push_back(FarStencilTablesFactory::Create(*vertexStencils));
        }

        if (numVaryingElements>0) {

            options.interpolationMode = FarStencilTablesFactory::INTERPOLATE_VARYING;

            varyingStencils = FarStencilTablesFactory::Create(*_refTables, options);
        }

        _computeContext = ComputeContext::Create(vertexStencils, varyingStencils);

        delete vertexStencils;
        delete varyingStencils;
    }

    void initializeDrawContext(int numElements, OsdMeshBitset bits) {

        assert(_refTables and _vertexBuffer);
        
        FarPatchTablesFactory::Options options;
        options.generateFVarTables = bits.test(MeshFVarData);

        _patchTables = FarPatchTablesFactory::Create(*_refTables, options);

        _drawContext = DrawContext::Create(_patchTables, numElements);

        _drawContext->UpdateVertexTexture(_vertexBuffer);
    }

    int initializeVertexBuffers(int numVertexElements,
        int numVaryingElements, OsdMeshBitset bits) {

        int numVertices = OsdGLMeshInterface::getNumVertices(*_refTables);

        int numElements = numVertexElements +
            (bits.test(MeshInterleaveVarying) ? numVaryingElements : 0);

        if (numVertexElements) {

            _vertexBuffer = VertexBuffer::Create(numElements, numVertices);
        }

        if (numVaryingElements>0 and (not bits.test(MeshInterleaveVarying))) {
            _varyingBuffer = VertexBuffer::Create(numVaryingElements, numVertices);
        }
        return numElements;
   }

    FarRefineTables * _refTables;
    FarPatchTables * _patchTables;
    FarKernelBatchVector _kernelBatches;

    VertexBuffer *_vertexBuffer;
    VertexBuffer *_varyingBuffer;

    ComputeContext *_computeContext;
    ComputeController *_computeController;

    DrawContext *_drawContext;
};

#ifdef OPENSUBDIV_HAS_OPENCL

template <class VERTEX_BUFFER>
class OsdMesh<VERTEX_BUFFER, OsdCLComputeController, OsdGLDrawContext> : public OsdGLMeshInterface {
public:
    typedef VERTEX_BUFFER VertexBuffer;
    typedef OsdCLComputeController ComputeController;
    typedef typename ComputeController::ComputeContext ComputeContext;
    typedef OsdGLDrawContext DrawContext;
    typedef typename DrawContext::VertexBufferBinding VertexBufferBinding;

    OsdMesh(ComputeController * computeController,
            FarRefineTables * refTables,
            int numVertexElements,
            int numVaryingElements,
            int level,
            OsdMeshBitset bits,
            cl_context clContext,
            cl_command_queue clQueue) :

            _refTables(refTables),
            _vertexBuffer(0),
            _varyingBuffer(0),
            _computeContext(0),
            _computeController(computeController),
            _drawContext(0),
            _clContext(clContext),
            _clQueue(clQueue)
    {
        assert(_refTables);

        OsdGLMeshInterface::refineMesh(*_refTables, level, bits.test(MeshAdaptive));

        int numElements =
            initializeVertexBuffers(numVertexElements, numVaryingElements, bits);

        initializeComputeContext(numVertexElements, numVaryingElements);

        initializeDrawContext(numElements, bits);
    }

    OsdMesh(ComputeController * computeController,
            FarRefineTables * refTables,
            VertexBuffer * vertexBuffer,
            VertexBuffer * varyingBuffer,
            ComputeContext * computeContext,
            DrawContext * drawContext,
            cl_context clContext,
            cl_command_queue clQueue) :

            _refTables(refTables),
            _vertexBuffer(vertexBuffer),
            _varyingBuffer(varyingBuffer),
            _computeContext(computeContext),
            _computeController(computeController),
            _drawContext(drawContext),
            _clContext(clContext),
            _clQueue(clQueue)
    {
        _drawContext->UpdateVertexTexture(_vertexBuffer);
    }

    virtual ~OsdMesh() {
        delete _refTables;
        delete _patchTables;
        delete _vertexBuffer;
        delete _varyingBuffer;
        delete _computeContext;
        delete _drawContext;
    }

    virtual int GetNumVertices() const { return _refTables->GetNumVerticesTotal(); }

    virtual void UpdateVertexBuffer(float const *vertexData, int startVertex, int numVerts) {
        _vertexBuffer->UpdateData(vertexData, startVertex, numVerts, _clQueue);
    }

    virtual void UpdateVaryingBuffer(float const *varyingData, int startVertex, int numVerts) {
        _varyingBuffer->UpdateData(varyingData, startVertex, numVerts, _clQueue);
    }

    virtual void Refine() {
        _computeController->Compute(_computeContext, _kernelBatches, _vertexBuffer, _varyingBuffer);
    }

    virtual void Refine(OsdVertexBufferDescriptor const *vertexDesc,
                        OsdVertexBufferDescriptor const *varyingDesc,
                        bool interleaved) {
        _computeController->Compute(_computeContext, _kernelBatches,
                                    _vertexBuffer, (interleaved ? _vertexBuffer : _varyingBuffer),
                                    vertexDesc, varyingDesc);
    }

    virtual void Synchronize() {
        _computeController->Synchronize();
    }

    virtual VertexBufferBinding BindVertexBuffer() {
        return _vertexBuffer->BindVBO();
    }

    virtual VertexBufferBinding BindVaryingBuffer() {
        return _varyingBuffer->BindVBO();
    }

    virtual DrawContext * GetDrawContext() {
        return _drawContext;
    }

    virtual VertexBuffer * GetVertexBuffer() {
        return _vertexBuffer;
    }

    virtual VertexBuffer * GetVaryingBuffer() {
        return _varyingBuffer;
    }

    virtual FarRefineTables const * GetRefineTables() const {
        return _refTables;
    }

    virtual void SetFVarDataChannel(int fvarWidth, std::vector<float> const & fvarData) {

        FarPatchTables::FVarPatchTables const * fvarPatchTables =
            _patchTables->GetFVarPatchTables();

        if (_drawContext and fvarPatchTables) {
            _drawContext->SetFVarDataTexture(*fvarPatchTables, fvarWidth, fvarData);
        }
    }

private:

    void initializeComputeContext(int numVertexElements,
        int numVaryingElements ) {

        assert(_refTables);

        FarStencilTablesFactory::Options options;
        options.generateOffsets=true;
        options.generateAllLevels=_refTables->IsUniform() ? false : true;

        FarStencilTables const * vertexStencils=0, * varyingStencils=0;

        if (numVertexElements>0) {

            vertexStencils = FarStencilTablesFactory::Create(*_refTables, options);

            _kernelBatches.push_back(FarStencilTablesFactory::Create(*vertexStencils));
        }

        if (numVaryingElements>0) {

            options.interpolationMode = FarStencilTablesFactory::INTERPOLATE_VARYING;

            varyingStencils = FarStencilTablesFactory::Create(*_refTables, options);
        }

        _computeContext = ComputeContext::Create(_clContext, vertexStencils, varyingStencils);

        delete vertexStencils;
        delete varyingStencils;
    }

    void initializeDrawContext(int numElements, OsdMeshBitset bits) {

        assert(_refTables and _vertexBuffer);

        FarPatchTablesFactory::Options options;
        options.generateFVarTables = bits.test(MeshFVarData);

        _patchTables = FarPatchTablesFactory::Create(*_refTables);

        _drawContext = DrawContext::Create(_patchTables, numElements);

        _drawContext->UpdateVertexTexture(_vertexBuffer);
    }

    int initializeVertexBuffers(int numVertexElements,
        int numVaryingElements, OsdMeshBitset bits) {

        int numVertices = OsdGLMeshInterface::getNumVertices(*_refTables);

        int numElements = numVertexElements +
            (bits.test(MeshInterleaveVarying) ? numVaryingElements : 0);

        if (numVertexElements) {

            _vertexBuffer = VertexBuffer::Create(numElements, numVertices, _clContext);
        }

        if (numVaryingElements>0 and (not bits.test(MeshInterleaveVarying))) {
            _varyingBuffer = VertexBuffer::Create(numVaryingElements, numVertices, _clContext);
        }
        return numElements;
   }

    FarRefineTables * _refTables;
    FarPatchTables * _patchTables;
    FarKernelBatchVector _kernelBatches;

    VertexBuffer *_vertexBuffer;
    VertexBuffer *_varyingBuffer;

    ComputeContext *_computeContext;
    ComputeController *_computeController;

    DrawContext *_drawContext;

    cl_context _clContext;
    cl_command_queue _clQueue;
};
#endif

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_GL_MESH_H
