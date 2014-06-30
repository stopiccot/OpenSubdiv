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

#ifndef OSD_CPU_COMPUTE_CONTROLLER_H
#define OSD_CPU_COMPUTE_CONTROLLER_H

#include "../version.h"

#include "../far/kernelBatchDispatcher.h"
#include "../osd/cpuComputeContext.h"
#include "../osd/vertexDescriptor.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

/// \brief Compute controller for launching CPU subdivision kernels.
///
/// OsdCpuComputeController is a compute controller class to launch
/// single threaded CPU subdivision kernels. It requires
/// OsdCpuVertexBufferInterface as arguments of the Refine() function.
///
/// The Osd Compute module provides functionality to interpolate primitive
/// variable data according to a subdivision scheme.
///
/// Controller entities execute requests from Context instances that they share
/// common interfaces with. Controllers are attached to discrete compute devices
/// and share the devices resources with Context entities.
///
class OsdCpuComputeController {
public:
    typedef OsdCpuComputeContext ComputeContext;

    /// Constructor.
    OsdCpuComputeController();

    /// Destructor.
    ~OsdCpuComputeController();


    /// Execute subdivision kernels and apply to given vertex buffers.
    ///
    /// @param  context       The OsdCpuContext to apply refinement operations to
    ///
    /// @param  batches       Vector of batches of vertices organized by operative 
    ///                       kernel
    ///
    /// @param  vertexBuffer  Vertex-interpolated data buffer
    ///
    /// @param  vertexDesc    The descriptor of vertex elements to be refined.
    ///                       if it's null, all primvars in the vertex buffer
    ///                       will be refined.
    ///
    template<class VERTEX_BUFFER>
        void Compute(OsdCpuComputeContext const * context,
                     FarKernelBatchVector const & batches,
                     VERTEX_BUFFER *vertexBuffer,
                     OsdVertexBufferDescriptor const *vertexDesc=NULL) {

        bind(vertexDesc, vertexBuffer);

        FarKernelBatchDispatcher::Apply(this, context, batches, /*maxlevel*/ -1); 
        
        unbind();
    }

    /// Waits until all running subdivision kernels finish.
    void Synchronize();


protected:

    friend class FarKernelBatchDispatcher;

    void ApplyStencilTableKernel(FarKernelBatch const &batch,
        ComputeContext const *context) const;

    template<class VERTEX_BUFFER>
        void bind(OsdVertexBufferDescriptor const *desc,
              VERTEX_BUFFER *buffer) {                       

        // if the vertex buffer descriptor is specified, use it.
        // otherwise, assumes the data is tightly packed in the vertex buffer.
        if (desc) {
            _currentBindState.desc = *desc;
        } else {
            int numElements = buffer ? buffer->GetNumElements() : 0;
            _currentBindState.desc = OsdVertexBufferDescriptor(0, numElements, numElements);
        }

        // apply vertex offset here
        _currentBindState.buffer = buffer ?
            buffer->BindCpuBuffer() + _currentBindState.desc.offset : 0;

    }

    void unbind() {
        _currentBindState.Reset();
    }

private:

    // Bind state is a transitional state during refinement.
    // It doesn't take an ownership of the vertex buffers.
    struct BindState {

        BindState() : buffer(0) { }

        float * GetElement(int index) const {
            return buffer + index*desc.stride;
        }

        void Reset() {
            buffer = 0;
            desc.Reset();
        }

        float * buffer;

        OsdVertexBufferDescriptor desc;
    };

    BindState _currentBindState;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_CPU_COMPUTE_CONTROLLER_H
