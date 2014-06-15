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

#include "../far/refineTables.h"

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
//  The subdiv type/options are specified on construction of the factory and are passed
//  on to each instance of FarRefineTables that it creates.  They can be modified as
//  there is nothing in the Factory tied to these properties.  Consider overloading
//  the Create() method (defined by subclasses) to vary these if greater flexibility
//  per instance is desired.
//
class FarRefineTablesFactoryBase {

public:

    /// \brief Instantiates FarRefineTables from indexing arrays.
    ///
    /// Note: because only face-vertices topological relationships are specified
    ///       with this factory, edge relationships have to be inferred, which
    ///       requires additional processing. If the client application can 
    ///       provide these relationships, then FarRefineTablesFactory::Create()
    ///       should be used instead.
    ///
    /// @param type          Subdivision scheme (catmark, loop ,..).
    ///
    /// @param options       Subdivision options (boundary interpolation rules, ...).
    ///
    /// @param numVertices   The number of vertices in the mesh.
    ///
    /// @param numFaces      The number of faces in the mesh.
    ///
    /// @param vertsPerFace  Array containing the number of vertices per face.
    ///
    /// @param vertIndices   Array containing the indices of the vertices for
    ///                      each face
    ///
    /// @return              An instance of FarRefineTables
    ///
    static FarRefineTables * Create( SdcType type,
                                     SdcOptions options,
                                     unsigned int numVertices,
                                     unsigned int numFaces,
                                     unsigned int const * vertsPerFace,
                                     unsigned int const * vertIndices   );


    /// \brief Adds edge crease weights to FarRefineTables
    ///
    /// Warning: this function will always fail and reutrn 0 if called after
    ///          any type of Refinement has been applied.
    ///
    /// @param refTables       The topology to add creases to.
    ///
    /// @param numEdges        The number of edges to be creased.
    ///
    /// @param vertIndexPairs  An array containing pairs of vertex indices.
    ///                        describing each edge
    ///
    /// @param creaseWeights   The creases sharpness.
    ///
    /// @return                The number of edges successfully sharpened.
    ///
    static unsigned int AddCreases( FarRefineTables & refTables,
                                    unsigned int numEdges,
                                    unsigned int const * vertIndexPairs,
                                    float const * creaseWeights          );



    /// \brief Adds corner crease weights to FarRefineTables
    ///
    /// Warning: this function will always fail and reutrn 0 if called after
    ///          any type of Refinement has been applied.
    ///
    /// @param refTables       The topology to add creases to.
    ///
    /// @param numVertices     The number of vertices (corners) to be creased.
    ///
    /// @param vertIndices     An array containing the vertex indices.
    ///
    /// @param creaseWeights   The creases sharpness.
    ///
    /// @return                The number of corners successfully sharpened.
    ///
    static unsigned int AddCorners( FarRefineTables & refTables,
                                    unsigned int numVertices,
                                    unsigned int const * vertIndices,
                                    float const * cornerWeights );
protected:

    static void validateComponentTopologySizing(FarRefineTables& refTables);
    static void validateComponentTopologyAssignment(FarRefineTables& refTables);

    static void applyComponentTagsAndBoundarySharpness(FarRefineTables& refTables);
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
class FarRefineTablesFactory : public FarRefineTablesFactoryBase {

public:
    FarRefineTablesFactory() : FarRefineTablesFactoryBase() { }

//    FarRefineTablesFactory(SdcType type, SdcOptions options = SdcOptions())
//            : FarRefineTablesFactoryBase(type, options) { }

    ~FarRefineTablesFactory() { }

    //
    //  Need to start bundling these options up more -- adding feature-adaptive to start.
    //  Current testing is making separate calls to Create(mesh) to create just the base
    //  level, then calling FarRefineTables::RefineAdaptive(), e.g.:
    //
    //      MyRefineTablesFactory refTablesFactory(type, options);
    //      FarRefineTables*      refTablesPtr = refTablesFactory.Create(myMesh);
    //      refTablesPtr->RefineAdaptive(maxLevel, fullTopology);
    //
    //  Given the public ability to re-refine the FarRefineTables (making use of the same
    //  base mesh), the definition of the Options belongs with FarRefineTables and not
    //  these Factory classes, though we may want the Factory to have an instance member
    //  for repeated application to the meshes it processes.
    //
    static FarRefineTables* Create(SdcType type, SdcOptions options, MESH const& mesh,
        int maxLevel = 0, bool fullTopology = false);

protected:
    //
    //  Methods to be specialized that implement all details specific to class MESH required
    //  to convert MESH data to FarRefineTables.  Note that some of these *must* be specialized
    //  in order to complete construction.
    //
    //  There are two minimal construction requirements and one optional.
    //
    //  See the comments in the generic stubs for details on how to write these.
    //
    //  Required:
    static void resizeComponentTopology(FarRefineTables& refTables, MESH const& mesh);
    static void assignComponentTopology(FarRefineTables& refTables, MESH const& mesh);

    //  Optional:
    static void assignComponentTags(FarRefineTables& refTables, MESH const& mesh);

protected:
    //  Other protected details -- not to be specialized:
    static void populateBaseLevel(FarRefineTables& refTables, MESH const& mesh);
};


//
//  Generic implementations:
//
template <class MESH>
FarRefineTables*
FarRefineTablesFactory<MESH>::Create(SdcType type, SdcOptions options, MESH const& mesh, int maxLevel, bool fullTopology)
{
    FarRefineTables *refTables = new FarRefineTables(type, options);

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

    validateComponentTopologyAssignment(refTables);

    //  Optional specialization for MESH:
    assignComponentTags(refTables, mesh);

    //  Finalize the translation of the mesh after its full specification above:
    applyComponentTagsAndBoundarySharpness(refTables);
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::resizeComponentTopology(FarRefineTables& /* refTables */, MESH const& /* mesh */)
{
    assert("Missing specialization for FarRefineTablesFactory<MESH>::resizeComponentTopology()" == 0);

    //
    //  Sizing the topology tables:
    //      This method is for determining the sizes of the various topology tables (and other
    //  data) associated with the mesh.  Once completed, appropriate memory will be allocated
    //  and an additional method invoked to populate it accordingly.
    //
    //  The following methods should be called -- first those to specify the number of faces,
    //  edges and vertices in the mesh:
    //
    //      void FarRefineTables::setBaseFaceCount(int count)
    //      void FarRefineTables::setBaseEdgeCount(int count)
    //      void FarRefineTables::setBaseVertexCount(int count)
    //
    //  and then for each face, edge and vertex, the number of its incident components:
    //
    //      void FarRefineTables::setBaseFaceVertexCount(Index face, int count)
    //      void FarRefineTables::setBaseEdgeFaceCount(  Index edge, int count)
    //      void FarRefineTables::setBaseVertexFaceCount(Index vertex, int count)
    //      void FarRefineTables::setBaseVertexEdgeCount(Index vertex, int count)
    //
    //  The count/size for a component type must be set before indices associated with that
    //  component type can be used.
    //
    //  Note that it is only necessary to size 4 of the 6 supported topological relations --
    //  the number of edge-vertices is fixed at two per edge, and the number of face-edges is
    //  the same as the number of face-vertices.
    //
    //  So a single pass through your mesh to gather up all of this sizing information will
    //  allow the Tables to be allocated appropriately once and avoid any dynamic resizing as
    //  it grows.
    //
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::assignComponentTopology(FarRefineTables& /* refTables */, MESH const& /* mesh */)
{
    assert("Missing specialization for FarRefineTablesFactory<MESH>::assignComponentTopology()" == 0);

    //
    //  Assigning the topology tables:
    //      Once the topology tables have been allocated, the six required topological
    //  relations can be directly populated using the following methods:
    //
    //      void IndexArray FarRefineTables::baseFaceVertices(Index face)
    //      void IndexArray FarRefineTables::baseFaceEdges(Index face)
    //
    //      void IndexArray FarRefineTables::baseEdgeVertices(Index edge)
    //      void IndexArray FarRefineTables::baseEdgeFaces(Index edge)
    //
    //      void IndexArray FarRefineTables::baseVertexEdges(Index vertex)
    //      void IndexArray FarRefineTables::baseVertexFaces(Index vertex)
    //
    //  For the last two relations -- the faces and edges incident a vertex -- there are
    //  also "local indices" that must be specified (considering doing this internally),
    //  where the "local index" of each incident face or edge is the index of the vertex
    //  within that face or edge, and so ranging from 0-3 for incident quads and 0-1 for
    //  incident edges.  These are assigned through similarly retrieved arrays:
    //
    //      LocalIndexArray FarRefineTables::baseVertexFaceLocalIndices(Index vertex)
    //      LocalIndexArray FarRefineTables::baseVertexEdgeLocalIndices(Index vertex)
    //
    //  As noted, we are considering determining these internally to avoid this complexity,
    //  but that will require iteration through the sets of vertex-faces and edges to find
    //  the location of the vertex within each.  If that is known at the time the incident
    //  componets are assigned, they we can avoid that separate pass.
    //
    //  We also need to tag vertices as manifold or not here.  Failure to do so explicitly
    //  will require the factory analyze the local neighborhood of each component, which
    //  is costly and often unnecessary.
    //
}

template <class MESH>
void
FarRefineTablesFactory<MESH>::assignComponentTags(FarRefineTables& /* refTables */, MESH const& /* mesh */)
{
    //
    //  Optional tagging:
    //      This is where any additional feature tags -- sharpness, holes, etc. -- can be
    //  specified.  For now, this is limited to sharpness using the following:
    //
    //      float& FarRefineTables::baseEdgeSharpness(Index edge)
    //      float& FarRefineTables::baseVertexSharpness(Index vertex)
    //
    //  which can be used on the LHS of assignments.
    //
    //  Tagging holes will become available in the near future as sets of bitfields for
    //  each component type are introduced and propogated through the refinement hierarchy.
    //
}

} // end namespace OPENSUBDIV_VERSION
using namespace OPENSUBDIV_VERSION;
} // end namespace OpenSubdiv

#endif /* FAR_REFINE_TABLES_FACTORY_H */
