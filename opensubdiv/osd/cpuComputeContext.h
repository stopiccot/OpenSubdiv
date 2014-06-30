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

#ifndef OSD_CPU_COMPUTE_CONTEXT_H
#define OSD_CPU_COMPUTE_CONTEXT_H

#include "../version.h"

#include "../far/stencilTables.h"
#include "../osd/vertexDescriptor.h"
#include "../osd/nonCopyable.h"

#include <stdlib.h>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

struct OsdVertexDescriptor;


///
/// \brief CPU Compute Context
///
/// The CPU implementation of the Compute module contextual functionality. 
///
/// The Osd Compute module provides functionality to interpolate primitive
/// variable data according to a subdivision scheme.
///
/// Contexts provide an interface between the serialized topological data 
/// of a geometric primitive and the computation resources of a compute device.
///
class OsdCpuComputeContext : private OsdNonCopyable<OsdCpuComputeContext> {

public:
    /// Creates an OsdCpuComputeContext instance
    ///
    /// @param stencilTables  The FarStencilTables used for this Context.
    ///
    static OsdCpuComputeContext * Create(FarStencilTables const &stencilTables);

    /// Destructor
    virtual ~OsdCpuComputeContext();

    /// Returns one of the vertex refinement tables.
    ///
    /// @param tableIndex the type of table
    ///
    FarStencilTables const & GetStencilTables() const {
        return _stencilTables;
    }

protected:

    explicit OsdCpuComputeContext(FarStencilTables const &subdivisionTables);

private:

    FarStencilTables _stencilTables;
};

}  // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

}  // end namespace OpenSubdiv

#endif  // OSD_CPU_COMPUTE_CONTEXT_H


