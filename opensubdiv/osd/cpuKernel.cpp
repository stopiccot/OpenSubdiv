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

#include "../osd/cpuKernel.h"
#include "../osd/vertexDescriptor.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

#if defined ( __INTEL_COMPILER ) or defined ( __ICC )
    #define __ALIGN_DATA __declspec(align(32))
#else
    #define __ALIGN_DATA
#endif


namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {


template <class T> T *
elementAtIndex(T * src, int index, OsdVertexBufferDescriptor const &desc) {

    return src + index * desc.stride;
}

static inline void
clear(float *dst, OsdVertexBufferDescriptor const &desc) {

    assert(dst);
    memset(dst, 0, desc.length*sizeof(float));
}

static inline void
addWithWeight(float *dst, const float *src, int srcIndex, float weight,
              OsdVertexBufferDescriptor const &desc) {

    assert(src and dst);
    src = elementAtIndex(src, srcIndex, desc);
    for (int k = 0; k < desc.length; ++k) {
        dst[k] += src[k] * weight;
    }
}

static inline void
copy(float *dst, int dstIndex, const float *src,
     OsdVertexBufferDescriptor const &desc) {

    assert(src and dst);

    dst = elementAtIndex(dst, dstIndex, desc);
    memcpy(dst, src, desc.length*sizeof(float));
}

template <int numElems> void
ComputeStencilKernel(float const * vertexSrc,
                     float * vertexDst,
                     unsigned char const * sizes,
                     int const * indices,
                     float const * weights,
                     int start,
                     int end) {

    __ALIGN_DATA float result[numElems],
                       result1[numElems];

    float const * src;
    float * dst, weight;
    int nstencils = end-start;

    for (int i=0; i<nstencils; ++i) {

        // Clear
#if defined ( __INTEL_COMPILER ) or defined ( __ICC )
    #pragma simd
    #pragma vector aligned
#endif
        for (int k = 0; k<numElems; ++k)
            result[k] = 0.0f;

        for (int j=0; j<sizes[i]; ++j, ++indices, ++weights) {

            src = vertexSrc + (*indices)*numElems;
            weight = *weights;

            // AddWithWeight
#if defined ( __INTEL_COMPILER ) or defined ( __ICC )
    #pragma simd
    #pragma vector aligned
#endif
            for (int k=0; k<numElems; ++k) {
                result[k] += src[k] * weight;
            }
        }

#if defined ( __INTEL_COMPILER ) or defined ( __ICC )
    #pragma simd
    #pragma vector aligned
#endif
        for (int k=0; k<numElems; ++k) {
            result1[k] = result[k];
        }

        dst = vertexDst + i*numElems;
        memcpy(dst, result1, numElems*sizeof(float));
    }
}

void
OsdCpuComputeStencils(OsdVertexBufferDescriptor const &vertexDesc,
                      float const * vertexSrc,
                      float * vertexDst,
                      unsigned char const * sizes,
                      int const * offsets,
                      int const * indices,
                      float const * weights,
                      int start, int end) {

    assert(start>=0 and start<end);

    if (start>0) {
        sizes += start;
        indices += offsets[start];
        weights += offsets[start];
    }

    if (vertexDesc==OsdVertexBufferDescriptor(0, 4, 4)) {

        // Fast path for aligned primvar data (4 floats)
        ComputeStencilKernel<4>(vertexSrc, vertexDst,
            sizes, indices, weights, start,  end);

    } else if(vertexDesc==OsdVertexBufferDescriptor(0, 8, 8)) {

        // Fast path for aligned primvar data (8 floats)
        ComputeStencilKernel<8>(vertexSrc, vertexDst,
            sizes, indices, weights, start,  end);
    }
    else {

        // Slow path for non-aligned data
        float * result = (float*)alloca(vertexDesc.length * sizeof(float));

        int nstencils = end-start;
        for (int i=0; i<nstencils; ++i, ++sizes) {

            clear(result, vertexDesc);

            for (int j=0; j<*sizes; ++j) {
                addWithWeight(result, vertexSrc, *indices++, *weights++, vertexDesc);
            }

            copy(vertexDst, i, result, vertexDesc);
        }
    }
}

}  // end namespace OPENSUBDIV_VERSION
}  // end namespace OpenSubdiv
