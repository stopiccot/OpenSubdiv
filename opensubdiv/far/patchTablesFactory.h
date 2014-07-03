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

#ifndef FAR_PATCH_TABLES_FACTORY_H
#define FAR_PATCH_TABLES_FACTORY_H

#include "../version.h"

#include "../hbr/mesh.h"
#include "../far/patchTables.h"

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//  Forward declarations (for internal implementation purposes):
class FarRefineTables;
class VtrLevel;
struct PatchFaceTag;
template <typename T> struct PatchTypes;


/// \brief A specialized factory for feature adaptive FarPatchTables
///
/// FarPatchTables contain the lists of vertices for each patch of an adaptive
/// mesh representation.
///
class FarPatchTablesFactory {

public:

    /// \brief Factory constructor for adaptive meshes
    ///
    /// @param refineTables  FarRefineTables from which to generate patches
    ///
    /// @param fvarwidth     The width of the interleaved face-varying data
    ///
    /// @return              A new instance of FarPatchTables
    ///
    static FarPatchTables * Create( FarRefineTables const * refineTables, int fvarwidth=0 );

private:
    //  (I would really prefer to hide all of these implementation details from the header...)

    typedef FarPatchTables::Descriptor Descriptor;

    //
    //  Support for uniform tables is not yet implemented, but should be able to use the same Create()
    //  method above as the FarRefineTables contain most (all?) of what we need:
    //
    static FarPatchTables * createUniform(  FarRefineTables const * refineTables, int fvarwidth=0 );
    static FarPatchTables * createAdaptive( FarRefineTables const * refineTables, int fvarwidth=0 );

    //  High-level methods for identifying and populating patches associated with faces:
    static void identifyAdaptivePatches( FarRefineTables const *     refTables,
                                         PatchTypes<int> &           patchInventory,
                                         std::vector<PatchFaceTag> & patchTags);

    static void populateAdaptivePatches( FarRefineTables const *           refTables,
                                         PatchTypes<int> const &           patchInventory,
                                         std::vector<PatchFaceTag> const & patchTags,
                                         FarPatchTables *                  tables);

    //  Methods for allocating and managing the patch table data arrays:
    static void allocateTables( FarPatchTables * tables, int nlevels, int fvarwidth );

    static void pushPatchArray( FarPatchTables::Descriptor desc,
                                FarPatchTables::PatchArrayVector & parray,
                                int npatches, int * voffset, int * poffset, int * qoffset );

    //  Low-level methods for populating patch properties:
    static float * computeFVarData(int face, const int width, float *coord, bool isAdaptive);

    static FarPatchParam * computePatchParam( FarRefineTables const & refTables, int level,
                                              int face, int rotation, FarPatchParam * coord );

    static void getQuadOffsets(VtrLevel const & level, int face, unsigned int * result);

private:
};

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;

} // end namespace OpenSubdiv

#endif /* FAR_PATCH_TABLES_FACTORY_H */
