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

#include "../osd/d3d11ComputeContext.h"
#include "../osd/error.h"

#include <D3D11.h>
#include <vector>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

template <class T> void
createD3D11Buffer(std::vector<T> const & src, DXGI_FORMAT format,
    ID3D11DeviceContext *deviceContext,
        ID3D11Buffer * buffer, ID3D11ShaderResourceView * srv) {

    size_t size = src.size()*sizeof(T);

    if (size==0) {
        buffer = 0;
        srv = 0;
    }

    ID3D11Device *device = 0;
    deviceContext->GetDevice(&device);
    assert(device);

    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = (unsigned int)size;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &src.at(0);

    HRESULT hr = device->CreateBuffer(&bd, &initData, &buffer);
    if (FAILED(hr)) {
        OsdError(OSD_D3D11_COMPUTE_BUFFER_CREATE_ERROR,
                 "Error creating compute table buffer\n");
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory(&srvd, sizeof(srvd));
    srvd.Format = format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvd.Buffer.FirstElement = 0;
    srvd.Buffer.NumElements = (unsigned int)src.size();

    hr = device->CreateShaderResourceView(buffer, &srvd, &srv);
    if (FAILED(hr)) {
        OsdError(OSD_D3D11_COMPUTE_BUFFER_CREATE_ERROR,
                 "Error creating compute table shader resource view\n");
        return;
    }
}

// ----------------------------------------------------------------------------

struct D3D11Table {

    D3D11Table() : buffer(0), srv(0) { }

    ~D3D11Table() {
        SAFE_RELEASE(buffer);
        SAFE_RELEASE(srv);
    }

    bool IsValid() const {
        return (buffer and srv);
    }

    ID3D11Buffer * buffer;
    ID3D11ShaderResourceView * srv;
};

// ----------------------------------------------------------------------------

class OsdD3D11ComputeContext::D3D11StencilTables {

public:

    D3D11StencilTables(FarStencilTables const & stencilTables,
        ID3D11DeviceContext *deviceContext) {

        createD3D11Buffer(stencilTables.GetSizes(),
            DXGI_FORMAT_R8_UINT, deviceContext, _sizes.buffer, _sizes.srv);

        createD3D11Buffer(stencilTables.GetOffsets(),
            DXGI_FORMAT_R32_SINT, deviceContext, _offsets.buffer, _offsets.srv);

        createD3D11Buffer(stencilTables.GetControlIndices(),
            DXGI_FORMAT_R32_SINT, deviceContext, _indices.buffer, _indices.srv);

        createD3D11Buffer(stencilTables.GetWeights(),
            DXGI_FORMAT_R32_SINT, deviceContext, _weights.buffer, _weights.srv);
    }

    bool IsValid() const {
        return _sizes.IsValid() and _offsets.IsValid() and
            _indices.IsValid() and _weights.IsValid();
    }

    D3D11Table const & GetSizes() const {
        return _sizes;
    }

    D3D11Table const & GetOffsets() const {
        return _offsets;
    }

    D3D11Table const & GetIndices() const {
        return _indices;
    }

    D3D11Table const & GetWeights() const {
        return _weights;
    }

    void Bind(ID3D11DeviceContext * deviceContext) const {
        ID3D11ShaderResourceView *SRViews[] = {
            _sizes.srv,
            _offsets.srv,
            _indices.srv,
            _weights.srv
        };
        deviceContext->CSSetShaderResources(2, 4, SRViews); // t4-t8
    }

    static void Unbind(ID3D11DeviceContext * deviceContext) {
        ID3D11ShaderResourceView *SRViews[] = { 0, 0, 0, 0 };
        deviceContext->CSSetShaderResources(2, 4, SRViews);
    }


private:

    D3D11Table _sizes,
               _offsets,
               _indices,
               _weights;
};

// ----------------------------------------------------------------------------

OsdD3D11ComputeContext::OsdD3D11ComputeContext(
    ID3D11DeviceContext *deviceContext,
        FarStencilTables const * vertexStencilTables,
            FarStencilTables const * varyingStencilTables) :
                _vertexStencilTables(0), _varyingStencilTables(0),
                    _numControlVertices(0) {

    if (vertexStencilTables) {
        _vertexStencilTables =
            new D3D11StencilTables(*vertexStencilTables, deviceContext);
        _numControlVertices = vertexStencilTables->GetNumControlVertices();
    }

    if (varyingStencilTables) {
        _varyingStencilTables =
            new D3D11StencilTables(*varyingStencilTables, deviceContext);

        if (_numControlVertices) {
            assert(_numControlVertices==varyingStencilTables->GetNumControlVertices());
        } else {
            _numControlVertices = varyingStencilTables->GetNumControlVertices();
        }
    }
}

OsdD3D11ComputeContext::~OsdD3D11ComputeContext() {
    delete _vertexStencilTables;
    delete _varyingStencilTables;
}


// ----------------------------------------------------------------------------

bool
OsdD3D11ComputeContext::HasVertexStencilTables() const {
    return _vertexStencilTables ? _vertexStencilTables->IsValid() : false;
}

bool
OsdD3D11ComputeContext::HasVaryingStencilTables() const {
    return _varyingStencilTables ? _varyingStencilTables->IsValid() : false;
}

// ----------------------------------------------------------------------------

void
OsdD3D11ComputeContext::BindVertexStencilTables(ID3D11DeviceContext *deviceContext) const {
    if (_vertexStencilTables) {
        _vertexStencilTables->Bind(deviceContext);
    }
}

void
OsdD3D11ComputeContext::BindVaryingStencilTables(ID3D11DeviceContext *deviceContext) const {
    if (_varyingStencilTables) {
        _varyingStencilTables->Bind(deviceContext);
    }
}

void
OsdD3D11ComputeContext::UnbindStencilTables(ID3D11DeviceContext *deviceContext) const {
    D3D11StencilTables::Unbind(deviceContext);
}


// ----------------------------------------------------------------------------

OsdD3D11ComputeContext *
OsdD3D11ComputeContext::Create(ID3D11DeviceContext *deviceContext,
    FarStencilTables const * vertexStencilTables,
        FarStencilTables const * varyingStencilTables) {

    OsdD3D11ComputeContext *result =
        new OsdD3D11ComputeContext(deviceContext, vertexStencilTables, varyingStencilTables);

    return result;
}

}  // end namespace OPENSUBDIV_VERSION
}  // end namespace OpenSubdiv
