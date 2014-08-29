..
     Copyright 2013 Pixar

     Licensed under the Apache License, Version 2.0 (the "Apache License")
     with the following modification; you may not use this file except in
     compliance with the Apache License and the following modification to it:
     Section 6. Trademarks. is deleted and replaced with:

     6. Trademarks. This License does not grant permission to use the trade
        names, trademarks, service marks, or product names of the Licensor
        and its affiliates, except as required to comply with Section 4(c) of
        the License and to reproduce the content of the NOTICE file.

     You may obtain a copy of the Apache License at

         http://www.apache.org/licenses/LICENSE-2.0

     Unless required by applicable law or agreed to in writing, software
     distributed under the Apache License with the above modification is
     distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
     KIND, either express or implied. See the Apache License for the specific
     language governing permissions and limitations under the Apache License.


Vtr Overview
------------

.. contents::
   :local:
   :backlinks: none

.. image:: images/api_layers_3_0.png
   :width: 100px
   :target: images/api_layers_3_0.png


Vectorized Topology Representation (Vtr)
========================================

This document is written to accompany the opensubdiv/vtr source.  It is intended
to give a high level overview of the organization of the source, the rationale
for various decisions, as well as highlight open issues or situations where the
choices made thus far warrant further deliberation.

Vtr is the newly proposed layer to support the "vectorized topological
representation" of a mesh and its associated refinement. Vtr is intended for
internal use and is currently accessed through the public FarTopologyRefiner
class and its Factory class that internally transforms an instance of a
client's mesh into Vtr form.

FarTopologyRefiner, in the Far API layer, is intended to be one of a set of
modular, table-oriented classes made available for public use. As such, it is
debatable whether the Vtr classes that support FarTopologyRefiner should be
part of Far or the new Vtr layer. We are prepared to rename all Vtr classes as
Far in future if necessary.

The Vtr API can be broken into the following elements:

    * Common Vtr utilities
    * `VtrLevel <#vtrlevel>`__ - a class representing complete vertex topology
      for a level
    * `VtrRefinement <#vtrrefinement>`__ - a class mapping a parent VtrLevel
      to a child level
    * VtrFVarLevel - a class representing complete face-varying topology for
      a level
    * VtrFVarRefinement - a class mapping a parent VtrFVarLevel to a child
      level

The following synopsis of each provides some of the motivation behind the
contents along with current issues being addressed. More details may be
provided in the headers themselves.

Vtr is very much a work in progress. There is still considerable functionality
to be added and potentially more performance tuning to be done. Work to be done
includes:

    * addition of more per-component tags to propogate with refinement
      (e.g. holes, etc.)
    * support for tri-split refinement required by Loop subdivision
    * specializations for regular levels and refinements

The `FarTopologyRefiner <far_overview.html>`__ class and its Factory class are
the public interface to the functionality provided here.

Common Vtr utilities
====================

Associated headers:

    * `vtr/types.h`

Synopsis
********

The style of the existing OpenSubdiv code is to nest the definitions of utility
types and classes in other classes. There are several in Vtr that are used
equally by multiple classes, and its debatable if they belong in one or
another. So currently <vtr/types.h> includes a number of such declarations for
public use within Vtr (or Far). These include:

    * declarations of types used for component indices, sharpness, etc.
    * constant and inline method for testing index validity
    * declarations of commonly used array types

Array Type
**********

With data in Vtr, `Far <far_overview.html>`__ and elsewhere stored as
collections of arrays aggregated into much larger arrays, access to these sets
of arrays within a larger context is frequent, and so making it both convenient
and efficient is of value.

A generic "array interface" class was made available to support this. This class
is nothing more than a pair that defines the extent of an array along with a
subset of the interface of std::vector for accessing its entries. It does not
own the data that it refers to, but simply allows access to its elements.

Arguably we could pass pairs of values around, either begin/end or begin/size,
but the frequency of such occurrences is considerable and it doubles the number
of function parameters in many cases (particularly those requiring more than
one array) so declaring a simple class for this purpose makes a big difference
to code readability.

The term "Vector" is used for types defined in terms of std::vector and "Array"
(or "Accessor" and "Modifier" for const/non-const interfaces. The two main Vtr
classes described below make heavy use of these arrays.


VtrLevel
========

Associated headers:

    * `vtr/level.h`

Synopsis
********

VtrLevel is a complete topological description of a subdivision level, with the
topological relations, sharpness values and component tags all stored in
vectors (literally std::vectors, but easily changed via typedefs). There are no
classes or objects for the mesh component types (i.e. faces, edges and
vertices) but simply an integer index to identify each.  It can be viewed as a
structure-of-arrays representation of the topology: any property related to a
particular component is stored in an array and accessible using the index
identifying that component. So with no classes the for the components, its
difficult to say what constitutes a "vertex" or a "face": they are each the sum
of all the fields scattered amongst the many vectors included.

VtrLevel represents a single level of a potential hierarchy and is capable of
representing the complete base mesh.  There are no members that relate data in
one level to any other, either below or above.  As such, any VtrLevel can be
used as the base level for a new subdivision hierarchy (potentially more than
one). All relationships between separate levels are maintained in the
`VtrRefinement <#vtrrefinement>`__ class.

Topological Relationships
*************************

`VtrLevel <#vtrlevel>`__ requires the definition of and associations between a
fixed set of indexable components for all three component types, i.e. an
explicit edge list in addition to the expected set of vertices and faces.
There are no explicit component objects in the representation, only an
integer index (*VtrIndex*) identifying each component within the set and data
associated with that component in the various vectors.

The topology is stored as six sets of incident relations between the components:
two each for the two other component types incident each component type, i.e.:

    * for each face, its incident vertices and incident edges
    * for each edge, its incident vertices and incident faces
    * for each vertex, its incident edges and incident faces

The collection of incidence relations is a vectorized variation of AIF (the
"Adjacency and Incidence Framework"). The set of these six incidence relations
is not minimal (only four are required, but that set excludes the most desired
face-vertex relation) but all six are kept and maintained to facilitate faster
refinement. While the sizes of several vectors are directly proportional to the
number of vertices, edges or faces to which the data is associated, the sizes
of some of the vectors for these relations is more cumulative and so additional
vectors of offsets is required (typical of the face-vertex list commonly used
as the minimal definition of mesh topology).

Vectors for the sharpness values associated with crease edges and corner
vertices are included (and so sized according to the number of edges and
vertices), along with additional tags for the components that may be helpful to
refinement (i.e. the type of subdivision Rule associated with each vertex).

A VtrLevel is really just a container for data in a subdivision level, and so
its public methods are primarily to access that data. Modification of the data
is protected and only made available to classes that are intended to construct
VtrLevels: currently the Far factory class that is responsible for building the
base level, and the `VtrRefinement <#vtrrefinement>`__ class that constructs
subsequent levels during refinement.

Memory Efficiency
*****************

One of the advantages in storing data in what is essentially a
structure-of-arrays, rather than the array-of-structures more typical of
topological representations, is that we can be more selective about memory
usage in some cases. Particularly in the case of uniform refinement, when the
data in subsequent levels is typically 4x its predecessor, we can minimize what
we either generate or keep around at each level. For instance, if only a
face-list is required at the finest level, we only need to generate one of the
six topological relations: the vertices incident each face. When we do keep
VtrLevels around in memory (as is the case with the `FarTopologyRefiner
<far_overview.html>`__) we do have do have the opportunity to prune what is not
strictly necessary after the refinement. Just as with construction, whatever
classes are privileged to construct a VtrLevel are likely those that will be
privileged to prune its contents when needed.

Since VtrLevel is purely topological, there is no template parameter required.
Other than the utility types defined in a separate header, this is true of all
Vtr source.


VtrRefinement
=============

Associated headers:
    vtr/refinement.h:

Synopsis
********

While `VtrLevel <#vtrlevel>`__ contains the topology for a subdivision level,
VtrRefinement is responsible for creating a new level via refinement of an
existing one, and for maintaining the relationships between the components in
the parent and child levels.  So a simplified view of a subdivision hierarchy
with Vtr is a set of VtrLevels with a VtrRefinement between each successive
pair.

VtrRefinement is a friend of VtrLevel and will populate a child level from a
parent given a set of refinement parameters (eventually). Feature-adaptive
refinement is just one form of selective refinement, the criteria being the
topological features of interest (creases and extra-ordinary vertices).  The
intent is to provide greater generality to facilitate the refinement of
particular regions of interest or more dynamic/adaptive needs.

Parent-child relationships
**************************

While VtrRefinement populates a new child VtrLevel as part of its refinement
operation, it also accumulates the relationships between the parent and child
level (and as with VtrLevel, this data is stored in vectors indexable by the
components). Currently the associations between components in the two levels
is uni-directional: child components are associated with incident components
of a parent component based on the parent components topology, so we have a
parent-to-child mapping (one to many). We tried to avoid having a
complementary child-to-parent mapping to reduce memory (particularly in the
case of uniform refinement) as it often was not necessary, but there is a
growing need for it in some cases once the hierarchy is constructed, so it is
likely it will be eventually added.

Data flexibility
****************

One of the advantages of the structure-of-arrays representation in both VtrLevel
and Refinement is that we can make more dynamic choices about what type of data
we choose to allocate and use based on needs. For instance, we can choose
between maintaining the parent-child or child-parent mapping in VtrRefinement,
or both if needed, and we can remove one if no longer necessary. An active
example of this is uniform refinement: if we only require the face-vertex
list at the finest subdivision level, there is no need to generate a complete
topological description of that level (as would be required of more traditional
representations), and given that level is 4x the magnitude of its parent, the
savings are considerable.

Currently there is nothing specific to a subdivision scheme in the refinement
other than the type of topological splitting to apply. The refinement does
subdivide sharpness values for creasing, but that too is independent of scheme.

Like VtrLevel, VtrRefinement is also purely topological, free of any template
parameter for a vertex type, and so its source is defined in a .cpp file
corresponding to its header.
