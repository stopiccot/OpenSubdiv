//
//   Copyright 2014 DreamWorks Animation LLC.
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
#ifndef FAR_REFINE_TABLES_FACTORY_H
#define FAR_REFINE_TABLES_FACTORY_H

#include "../version.h"

#include <cassert>

namespace OpenSubdiv {
namespace OPENSUBDIV_VERSION {

//
//  FarRefineTableFactoryBase:
//      This is an abstract base class for subclasses that are intended to construct
//  FarRefineTables from a mesh class that defines the subclass.  The subclasses are
//  parameterized by the mesh type <class MESH>.  The base class provides all
//  implementation details related to assembly and validation that are independent
//  of the subclass' mesh type.
//
//  Its still unclear where the division of functionality lies between construction
//  of the Factory and creation of an instance of the Tables.  We definitely do not
//  want data generation in the construction to require duplication or copying in
//  the creation.  Overloading Create() to "close" the tables (copying the base level
//  and other data, but refining differently) are also possibilities.
//
//  Current thinking is to simply specify the subdiv type/options on construction if
//  they are expected to be fixed for a set of meshes.  Otherwise we'll need the
//  client to specify them for each instance.
//
class FarRefineTablesFactoryBase
{
public:
    FarRefineTablesFactoryBase() { }
    virtual ~FarRefineTablesFactoryBase() { }

    //  The Create() method is defined by the subclass template as it needs an argument
    //  of the subclass mesh type.

protected:
    void validateComponentTopologySizing(FarRefineTables& refTables);
    void applyBoundariesToSharpness(FarRefineTables& refTables);
    void computeBaseVertexRules(FarRefineTables& refTables);

protected:
    //  Are there any useful members to retain in the Factory class?
};


//
//  FarRefineTableFactory<MESH>:
//      The factory class template to convert and refine an instance of FarRefineTables
//  from an arbitrary mesh class.  While a class template, the implementation is not
//  (cannot) be complete, so specialization of a few methods is required.
//      This template provides both the interface and high level assembly for the
//  construction of the FarRefineTables instance.  The high level construction executes
//  a specific set of operations to convert the client's MESH into FarRefineTables,
//  using methods independent of MESH from the base class and those specialized for
//  class MESH appropriately.
//
template <class MESH>
class FarRefineTablesFactory : public FarRefineTablesFactoryBase
{
public:
    FarRefineTablesFactory() : FarRefineTablesFactoryBase() { }
    ~FarRefineTablesFactory() { }

    FarRefineTables* Create(MESH const& mesh, int maxLevel = 0, bool fullTopology = false);

protected:
    void populateBaseLevel(FarRefineTables& refTables, MESH const& mesh);

    //
    //  Methods to be specialized that implement all details specific to class MESH
    //  required to convert MESH data to FarRefineTables.  Note that some of these
    //  *must* be specialized in order to complete construction.
    //
    //  Minimal construction requirements:
    void resizeComponentTopology(FarRefineTables& refTables, MESH const& mesh);
    void assignComponentTopology(FarRefineTables& refTables, MESH const& mesh);

    //  Optional:
    void assignComponentTags(FarRefineTables& refTables, MESH const& mesh);
};


template <class MESH>
FarRefineTables*
FarRefineTablesFactory<MESH>::Create(MESH const& mesh, int maxLevel, bool fullTopology)
{
    FarRefineTables *refTables = new FarRefineTables;

    populateBaseLevel(*refTables, mesh);

    if (maxLevel > 0) {
        refTables->RefineUniform(maxLevel, fullTopology);
    }
    return refTables;
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::populateBaseLevel(FarRefineTables& refTables, MESH const& mesh)
{
    VtrLevel& level = refTables.GetBaseLevel();

    //
    //  The following three methods may end up virtual:
    //      - resize the component counts and relation counts for individual components:
    //      - assign the topological relations for all components:
    //      - assign any sharpness values, hole tags, etc:
    //  Note that we can do some sanity checking (independent of the type MESH) between these
    //  to ensure that a client has done what is necessary at each stage.
    //

    //  Required specialization for MESH:
    resizeComponentTopology(refTables, mesh);

    validateComponentTopologySizing(refTables);

    //  Required specialization for MESH:
    assignComponentTopology(refTables, mesh);

    //  Optional specialization for MESH:
    assignComponentTags(refTables, mesh);

    level._hasTopology = true;

    //
    //  Finalize the translation of the mesh after its full specification above:
    //
    applyBoundariesToSharpness(refTables);

    //  Unclear if this is warranted now, or if it should be computed later if/when needed
    computeBaseVertexRules(refTables);
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::resizeComponentTopology(FarRefineTables& refTables, MESH const& mesh)
{
    assert("Missing specialization for FarRefineTablesFactory<MESH>::resizeComponentTopology()" == 0);

    //
    //  This method is for determining the sizes of various data types associated with the
    //  mesh.  Once completed, the appropriate memory will be allocated and an additional
    //  method will populate it accordingly.
    //
    //  Make use of the following methods to populate the vertex, edge and face data --
    //  the size of faces, edges and vertices must be specified before any of the methods
    //  requiring face, edge and vertex indices:
    //
    //      void FarRefineTables::resizeFaces(int size);
    //      void FarRefineTables::resizeEdges(int size);
    //      void FarRefineTables::resizeVerts(int size);
    //
    //      void FarRefineTables::resizeFaceVerts(int faceIndex, int size);
    //      void FarRefineTables::resizeEdgeFaces(int edgeIndex, int size);
    //      void FarRefineTables::resizeVertFaces(int vertIndex, int size);
    //      void FarRefineTables::resizeVertEdges(int vertIndex, int size);
    //
    //  Note that it is only necessary to size 4 of the 6 possible relations -- the number
    //  of edge-verts is simply twice the number of edges, and the number of face-edges is
    //  the same as the number of face-verts.
    //
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::assignComponentTopology(FarRefineTables& refTables, MESH const& mesh)
{
    assert("Missing specialization for FarRefineTablesFactory<MESH>::assignComponentTopology()" == 0);
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::assignComponentTags(FarRefineTables& refTables, MESH const& mesh)
{
    //
    //  This method is for specify edge and vertex creasing and face holes:
    //
    //  Make use of the following methods fo populate the vertex, edge and face data:
    //      void FarRefineTables::edgeSharpness(Index edgeIndex);
    //      void FarRefineTables::vertSharpness(Index vertIndex);
    //      void FarRefineTables::faceHole(     Index faceIndex);
    //
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* FAR_REFINE_TABLES_FACTORY_H */
