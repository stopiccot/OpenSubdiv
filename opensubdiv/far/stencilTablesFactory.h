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

#ifndef FAR_STENCILTABLE_FACTORY_H
#define FAR_STENCILTABLE_FACTORY_H

#include "../version.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class FarStencilTables;
class FarRefineTables;

/// \brief A specialized factory for FarStencilTables
///
class FarStencilTablesFactory {

public:

    struct Options {
    
        Options() : generateOffsets(false),
                    generateAllLevels(true) { }
    
        unsigned int generateOffsets : 1,
                     generateAllLevels : 1;
    };

    /// \brief Instantiates FarStencilTables from FarRefineTables that have been
    ///        refined uniformly or adaptively.
    ///
    /// \note The factory only creates stencils for vertices that have already
    ///       been refined in the FarRefineTables. Use RefineUniform() or
    ///       RefineAdaptive() before constructing the stencils.
    ///
    /// @param refTables  The FarRefineTables containing the refined topology
    ///
    /// @param allLevels  Set to false to generate stencils for the vertices at
    ///                   the highest level of refinement only
    ///
    static FarStencilTables const * Create(FarRefineTables const & refTables,
        Options options = Options());


    /// \brief Returns the largest stencil size that can be accomodated by the
    ///        factory. This number should be adjusted to process topologies
    ///        with very high valence vertices;
    static int GetMaxStencilSize();

private:
};


} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif // FAR_STENCILTABLE_FACTORY_H
