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


FAR Overview
------------

.. contents::
   :local:
   :backlinks: none

Feature Adaptive Representation (Far)
=====================================

This document is written to accompany the opensubdiv/`vtr <vtr_overview.html>`__
source.  It is intended to give a high level overview of the organization of
the source, the rationale for various decisions, as well as highlight open
issues or situations where the choices made thus far warrant further
deliberation.

The Far classes package up the functionality provided in Vtr for public use,
either directly within Far or indirectly eventually though Osd. The two classes
classes are as follows:

+----------------------------------+---------------------------------------------------+
| FarTopologyRefiner               | A class encapsulating the topology of a refined   |
|                                  | mesh.                                             |
+----------------------------------+---------------------------------------------------+
| FarTopologyRefinerFactory<MESH>  | A factory class template specialized by users (in |
|                                  | terms of their mesh class) to construct           |
|                                  | FarTopologyRefiner as quickly as possible.        |
+----------------------------------+---------------------------------------------------+

These classes are the least well defined of the API, but given they provide the
public interface to all of the improvements proposed, they potentially warrant
the most attention. FarTopologyRefiner is purely topological and it is the
backbone used to construct or be associated with the other table classes in Far.

Topology Refiner
================

Associated headers:
    `far/topologyRefiner.h`

Synopsis
********

FarTopologyRefiner is the building block for many other useful classes in
OpenSubdiv, but its purpose is more specific.  It is intended to store the
topology of an arbitrarily refined subdivision hierarchy to support the
construction of `stencil tables <#patch-tables>`__, `patch tables
<#patch-tables>`__,  etc.

Aside from public access to topology, *FarTopologyRefiner::Refine(...)* is
internally where simple specifications of refinement (currently uniform or
feature-adaptive with a level argument) will be translated into refinement
operations within Vtr. Feature-adaptive refinement is a special case of
*"sparse"* or *"selective"* refinement, and so the feature-adaptive logic
exists internal to FarTopologyRefiner and translate the feature-analysis into a
simpler topological specification of refinement to Vtr.

The longer term intent is that the public Refine(...) operation eventually be
overloaded to allow clients more selective control of refinement. While
FarTopologyRefiner is a purely topological class, and so free of any
definitions of vertex data, the public inteface has been extended to include
templated functors that allow clients to interpolate primitive variable data.

Topology Refiner Factory
========================

Associated headers:
    `far/topologyRefinerFactory.h`

Synopsis
********

This is one of the most important aspects of the Far API, as it is the
entry-point to OpenSubdiv. Its task is to map/convert data in a client's mesh
into the internal `Vtr <vtr_overview.html>`__ representation as quickly as
possible. Since a client's boundary-rep knows best how to identify the data it
wants, it seems best to leave it to the client to gather that data and simply
provide a location to store it, rather than trying to define a suite of mesh
traversal utilities that the client would need to define to do so.

So the approach taken was to follow a common pattern in OpenSubdiv, and use
Factory classes to construct instances of FarTopologyRefiner and provide some
kind of derivation or specialization by the client to optimize the process for
their representation. The FarTopologyRefiner's Factory class will construct its
instances from a client's mesh (of unknown type). Given the need to manage an
instance of a client's mesh type in the Factory, the simplest solution is for
client-code to specialize a templated interface in the Factory.

The FarTopologyRefinerFactory class is a class template parameterized by and
specialized for the client's mesh class, i.e. FarTopologyRefinerFactory<MESH>.
The template provides the high-level construction of the tables, with the
requirement that two methods will be specialized.  These two methods serve the
following two purposes:

    * specify the sizes of topological data so that tables can be pre-allocated
    * assign the topological data to the newly allocated tables

As noted above, the assumption here is that the client's boundary-rep knows best
how to retrieve the data that we require most efficiently, so that interference
with it is kept to a minimum. After the factory class gathers sizing
information and allocates appropriate memory, the factory provides the client
with locations of the appropriate tables to be populated (using the same `Array
<vtr_overview.html#arry-type>`__ classes and interface used to access the
tables).  The client is expected to load a complete topological description and
can optionally load other tables, i.e.:

    * the six topological relations required by Vtr, oriented when manifold
    * sharpness values for edges and/or vertices (optional)
    * additional tags related to the components, e.g. holes (optional)

While there is plenty of opportunity for user error here, that is no different
from any other conversion process.  Given that Far controls the construction
process through the Factory class, we do have ample opportunity to insert
runtime validation, and to vary that level of validation at any time on an
instance of the Factory.

A common base class has been created for the factory class, i.e.:

.. code:: c++

    template <class MESH>
    class FarTopologyRefinerFactory : public FarTopologyRefinerFactoryBase

both to provide common code independent of <MESH> and also potentially to
protect core code from unwanted specialization.

XXXX <insert blurb about Descriptor>

Refining Topology
*****************

XXXX <insert blurb about uniform / adaptive refinement>

.. image:: images/topology_refiner.png
   :align: center


Patch Tables
============

.. include:: under_development.rst


Stencil Tables
==============

.. include:: under_development.rst

The base container for stencil data is the FarStencilTables class. As with most
other Far entities, it has an associated FarStencilTablesFactory that requires
a FarTopologyRefiner:

.. image:: images/far_stencil5.png
   :align: center

Principles
**********

Iterative subdivision algorithms such as the one used in `FarSubdivisionTables
<#subdivision-tables>`__ converge towards the limit surface by sucessively
refining the vertices of the coarse control cage.

.. image:: images/far_stencil4.png
   :align: center

Each step is dependent upon the previous subidivion step being completed, and a
substantial number of steps may be required in order approximate the limit. Since
each subdivision step incurs an O(4 :superscript:`n`) growing amount of
computations, the accrued number of interpolations can be quite large.

However, every intermediate subdivided vertex can be expressed as a linear
interpolation of vertice from the previous step. So, eventually, every point at
on the limit surface can be expressed as a weighted average of the set of coarse
control vertices from the one-ring surrounding the face that the point is in:

.. image:: images/far_stencil3.png
   :align: center

Where:

.. image:: images/far_stencil2.png
   :align: center

Stencils are created by combining the list of control vertices of the 1-ring
to a set of interpolation weights obtained by successive accumulation of
subdivision interpolation weights.

The weight accumulation process is made efficient by adaptively subdividing the
control cage only around extraordinary locations, and otherwise reverting to fast
bi-cubic bspline patch evaluation. The use of bi-cubic patches also allows the
accumulation of analytical derivatives.

Limit Stencils
**************

Limit stencils are the most direct method of evaluation of specific locations on
the limit of a subdivision surface starting from the coarse vertices of the
control cage.

.. image:: images/far_stencil0.png
   :align: center


Sample Location
***************

Each stencil is associated with a singular parametric location on the coarse
mesh. The paramatric location is defined as face location and local [0.0 - 1.0]
(u,v) triplet:

In the case of a non-coarse quad face, the parametric sub-face quadrant needs to
be identified. This can be done either explicitly or implicitly by using the
unique ptex face indices for instance.

.. image:: images/far_stencil6.png
   :align: center

Code example
************

When the control vertices (controlPoints) move in space, the limit locations can
be very efficiently recomputed simply by applying the blending weights to the
series of coarse control vertices:

.. code:: c++

    class StencilType {
    public:

        void Clear() {
            memset( &x, 0, sizeof(StencilType));
        }

        void AddWithWeight( StencilType const & cv, float weight  ) {
            x += cv.x * weight;
            y += cv.y * weight;
            z += cv.z * weight;
        }

        float x,y,z;
    };

    std::vector<StencilType> controlPoints,
                             points,
                             utan,
                             vtan;

    // Uppdate points by applying stencils
    controlStencils.UpdateValues<StencilType>( reinterpret_cast<StencilType const *>(
        &controlPoints[0]), &points[0] );

    // Uppdate tangents by applying derivative stencils
    controlStencils.UpdateDerivs<StencilType>( reinterpret_cast<StencilType const *>(
        &controlPoints[0]), &utan[0], &vtan[0] );

