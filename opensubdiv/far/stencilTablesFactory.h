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

#include "../far/kernelBatch.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

class FarStencil;
class FarStencilTables;
class FarRefineTables;

/// \brief A specialized factory for FarStencilTables
///
/// Note: when using 'sortBySize', vertex indices from FarPatchTables or
///       FarRefineTables need to be remapped to their new location in the
///       vertex buffer.
///
//        XXXX manuelk remap table creation not implemented yet !
//
class FarStencilTablesFactory {

public:

    struct Options {
    
        Options() : generateOffsets(false),    
                    generateAllLevels(true),   
                    sortBySize(false) { }
    
        int generateOffsets   : 1, ///< populate optional "_offsets" field          
            generateAllLevels : 1, ///< vertices at all levels or highest only
            sortBySize        : 1; ///< sort stencils by size (within a level)
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
    /// @param options    Options controlling the creation of the tables
    ///
    static FarStencilTables const * Create(FarRefineTables const & refTables,
        Options options = Options());

    /// \brief Returns a FarKernelBatch applying all the stencil in the tables
    ///        to primvar data.
    ///
    /// @param stencilTables The stencil tables to batch
    ///
    static FarKernelBatch Create(FarStencilTables const &stencilTables);

private:

    // Copy a stencil into FarStencilTables
    template <class T> static void copyStencil(T const & src, FarStencil & dst);

    // (Sort &) Copy a vector of stencils into FarStencilTables
    template <class T> static void copyStencils(std::vector<T> & src,
        FarStencil & dst, bool sortBySize);
        
    std::vector<int> _remap;
};


} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif // FAR_STENCILTABLE_FACTORY_H
