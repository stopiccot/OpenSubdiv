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

Release 3.0
===========

OpenSubdiv 3.0 represents a landmark release with very profound changes to the
core algorithms. While providing faster, more efficient and flexible subdivision
code remains our principal goal, OpenSubdiv 3.0 introduces many improvements
that are fairly radical departures from previous versions.

Improved performance
********************

OpenSubdiv 3.0 introduces new data structures and algorithms that greatly enhance
performance over previous versions. This 3.0 releases focuse mostly on the CPU
side, and provides speed-ups in the range of an order of magnitude for topology
refinement and analysis (uniform and adaptive). On the GPU side, we have removed
several bottlenecks in the Compute area that yield up to 4x faster interpolation
on CUDA platforms. We have also significantly reduced the number of kernel
launches (now 1 per primitive), which was a known issue on certain mobile platforms.

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

Hierarchical Edits
++++++++++++++++++

Currently Hierarchical Edits have been marked as "extended specification" and
support for hierarchical features has been removed from the 3.0 release. This
decision allows for great simplifications of many areas of the subdivision
algorithms. If we can identify legitimate use cases for hierarchical tags, we
will consider re-implementing them in future releases, as time and resources
allow.

General RoadMap
***************

Known Issues
************

* Maximum vertex valence is still limited (XXXX todo)

.. container:: notebox

    Content under development....
    
    .. image:: images/construction.png
       :align: center
       :height: 100



Release 2.x
===========

`Previous releases <release_notes_2x.html>`_
