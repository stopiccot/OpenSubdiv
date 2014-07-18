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

#include "../far/stencilTables.h"

#include "../osd/cudaComputeContext.h"

#include <cuda_runtime.h>
#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

// ----------------------------------------------------------------------------

template <class T> void *
createCudaBuffer(std::vector<T> const & src) {

    void * devicePtr=0;

    size_t size = src.size()*sizeof(T);

    cudaError_t err = cudaMalloc(&devicePtr, size);
    if (err != cudaSuccess) {
        return devicePtr;
    }

    err = cudaMemcpy(devicePtr, &src.at(0), size, cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        cudaFree(devicePtr);
        return 0;
    }
    return devicePtr;
}

// ----------------------------------------------------------------------------

class OsdCudaComputeContext::CudaStencilTables {

public:

    CudaStencilTables(FarStencilTables const & stencilTables) {

        _sizes = createCudaBuffer(stencilTables.GetSizes());
        _offsets = createCudaBuffer(stencilTables.GetOffsets());
        _indices = createCudaBuffer(stencilTables.GetControlIndices());
        _weights = createCudaBuffer(stencilTables.GetWeights());
    }

    ~CudaStencilTables() {
        if (_sizes) { cudaFree(_sizes); }
        if (_offsets) { cudaFree(_offsets); }
        if (_indices) { cudaFree(_indices); }
        if (_weights) { cudaFree(_weights); }
    }

    bool IsValid() const {
        return _sizes and _offsets and _indices and _weights;
    }

    void * GetSizes() const {
        return _sizes;
    }

    void * GetOffsets() const {
        return _offsets;
    }

    void * GetIndices() const {
        return _indices;
    }

    void * GetWeights() const {
        return _weights;
    }

private:
    void * _sizes,
         * _offsets,
         * _indices,
         * _weights;
};

// ----------------------------------------------------------------------------

OsdCudaComputeContext::OsdCudaComputeContext(
    FarStencilTables const * vertexStencilTables,
        FarStencilTables const * varyingStencilTables) :
            _vertexStencilTables(0), _varyingStencilTables(0),
                _numControlVertices(0) {

    if (vertexStencilTables) {
        _vertexStencilTables = new CudaStencilTables(*vertexStencilTables);
        _numControlVertices = vertexStencilTables->GetNumControlVertices();
    }

    if (varyingStencilTables) {
        _varyingStencilTables = new CudaStencilTables(*varyingStencilTables);

        if (_numControlVertices) {
            assert(_numControlVertices==varyingStencilTables->GetNumControlVertices());
        } else {
            _numControlVertices = varyingStencilTables->GetNumControlVertices();
        }
    }
    
}

OsdCudaComputeContext::~OsdCudaComputeContext() {
    delete _vertexStencilTables;
    delete _varyingStencilTables;
}

// ----------------------------------------------------------------------------

bool
OsdCudaComputeContext::HasVertexStencilTables() const {
    return _vertexStencilTables ? _vertexStencilTables->IsValid() : false;
}

bool
OsdCudaComputeContext::HasVaryingStencilTables() const {
    return _varyingStencilTables ? _varyingStencilTables->IsValid() : false;
}

// ----------------------------------------------------------------------------

void *
OsdCudaComputeContext::GetVertexStencilTablesSizes() const {
    return _vertexStencilTables ? _vertexStencilTables->GetSizes() : 0;
}

void *
OsdCudaComputeContext::GetVertexStencilTablesOffsets() const {
    return _vertexStencilTables ? _vertexStencilTables->GetOffsets() : 0;
}

void *
OsdCudaComputeContext::GetVertexStencilTablesIndices() const {
    return _vertexStencilTables ? _vertexStencilTables->GetIndices() : 0;
}

void *
OsdCudaComputeContext::GetVertexStencilTablesWeights() const {
    return _vertexStencilTables ? _vertexStencilTables->GetWeights() : 0;
}

// ----------------------------------------------------------------------------

void *
OsdCudaComputeContext::GetVaryingStencilTablesSizes() const {
    return _varyingStencilTables ? _varyingStencilTables->GetSizes() : 0;
}

void *
OsdCudaComputeContext::GetVaryingStencilTablesOffsets() const {
    return _varyingStencilTables ? _varyingStencilTables->GetOffsets() : 0;
}

void *
OsdCudaComputeContext::GetVaryingStencilTablesIndices() const {
    return _varyingStencilTables ? _varyingStencilTables->GetIndices() : 0;
}

void *
OsdCudaComputeContext::GetVaryingStencilTablesWeights() const {
    return _varyingStencilTables ? _varyingStencilTables->GetWeights() : 0;
}

// ----------------------------------------------------------------------------

OsdCudaComputeContext *
OsdCudaComputeContext::Create(FarStencilTables const * vertexStencilTables,
                              FarStencilTables const * varyingStencilTables) {

    OsdCudaComputeContext *result =
        new OsdCudaComputeContext(vertexStencilTables, varyingStencilTables);

    return result;
}

}  // end namespace OPENSUBDIV_VERSION
}  // end namespace OpenSubdiv
