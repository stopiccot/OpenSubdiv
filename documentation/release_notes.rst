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


Release Notes
-------------

.. contents::
   :local:
   :backlinks: none

----

.. include:: under_development.rst


General 3.x RoadMap 
===================

Within the 3.x release cycle we would like to address first and foremost many of
the issues related to scaling the application of subdivision surfaces to large
volumes of primitives within typical graphics pipelines. 

Enabling workflows at larger scales will require improvements on several fronts:

* Handle more primitives, but with fewer overheads:

    * Reduce Compute kernel launches using stencils instead of subdivision tables
    * Reduce Draw calls by addressing the combinatorial explosion of tessellation
      shaders
    * Provide back-ends for next-gen APIs (D3D12, Mantle, Metal, GL 5.x)

* More semi-sharp creases: feature isolation needs to become much more efficient to
  allow for complete creative freedom in using the feature.
* Faster topology analysis


Release 3.0
===========

OpenSubdiv 3.0 represents a landmark release with very profound changes to the
core algorithms. While providing faster, more efficient, and more flexible
subdivision code remains our principal goal, OpenSubdiv 3.0 introduces many
improvements that constitute a fairly radical departures from our previous
code releases.

Improved performance
********************

OpenSubdiv 3.0 introduces new data structures and algorithms that greatly enhance
performance over previous versions. The 3.0 release focuses mostly on the CPU
side, and  should provide "out-of-the-box" speed-ups close to an order of
magnitude for topology refinement and analysis (both uniform and adaptive).

On the GPU side, the replacement of subdivision tables with stencils allows
us to remove several bottlenecks in the Compute area that can yield as much as
4x faster interpolation on CUDA platforms. At the same time, stencils also
reduce the dozens of kernel launches required per primitive to a single one (this
was a known issue on certain mobile platforms). Compute calls batching is now
trivial.

New topology entry-points
*************************

OpenSubdiv 3.0 introduces several new entry-points for client topology. Previous
releases force client applications to define and populate instances of an Hbr
half-edge topology representation. For many applications, this representation is
both redundant and inefficient.

OpenSubdiv 3.0 introduces a new *intermediate* topological representation, named
**Vtr** (Vectorized Topology Representation). The topological relationships held
by Vtr can populated using either a high-level interface where simplicity has
been emphasized, or a lower-level interface for enhanced efficiency.

As a result, Hbr is no longer a core API of OpenSubdiv. While the code is marked
as deprecated, it will remain in the source distribution for legacy and
regression purposes.

Stencil Tables
**************

OpenSubdiv 3.0 replaces the serialized subdivision tables with factorized stencil
tables. Subdivision tables as implemented in 2.x releases still contain a fairly
large amount of data inter-dependencies which incur pernalties for using more
fences or kernel launches. Most of these dependencies have now been factorised
away in the pre-computation stage, yielding *stencil tables* instead.

Stencils remove all data dependencies and simplify all the computations into a
single trivial kernel. This simplification results in a faster pre-computation
stage, faster execution on GPU, and fewer driver overheads. The new stencil
tables Compute back-end is supported on all the same platforms as previous
releases.

Subdivision Specification Changes
*********************************

The refactoring of OpenSubdiv 3.0 data representations presents a unique
opportunity to revisit some corners of the subdivision specification and
remove or update some legacy features.

XXXX

Hierarchical Edits
++++++++++++++++++

Currently Hierarchical Edits have been marked as "extended specification" and
support for hierarchical features has been removed from the 3.0 release. This
decision allows for great simplifications of many areas of the subdivision
algorithms. If we can identify legitimate use cases for hierarchical tags, we
will consider re-implementing them in future releases, as time and resources
allow.

Alpha Release Notes
===================

Our intentions as open-source developpers is to give as much access to our code,
as early as possible, because we value and welcome the feedback from the community.

The 'alpha' release moniker means to us that our code is still far from being
finalized. Although we are now close from being feature complete, our
public-facing interfaces are still subject to change. Therefore, we do not
recommend this version of OpenSubdiv be used in client applications until both
features and interfaces have been finalized in an official 'Beta' Release.

The following is a short list of featurs and issues that will be addressed during
the alpha cycle:

    #. Rename "FarTopologyRefiner"
    #. Refactor Interpolate<>()
    #. Refinement orientation bug + validation code
    #. Face-varying boundary interpolation rules interpolation
    #. Arbitrary-location limit stencils
    #. Limit Masks
    #. Loop / Bilinear schemes + Vtr / Far refactor
    #. Holes implementation
    #. Misc Sdc / Vtr / Far code cleanup & documentation
    #. GCD Compute back-end



Release 2.x
===========

`Previous releases <release_notes_2x.html>`_
