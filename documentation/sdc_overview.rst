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


Sdc Overview
------------

.. contents::
   :local:
   :backlinks: none

.. image:: images/api_layers_3_0.png
   :width: 100px
   :target: images/api_layers_3_0.png

Subdivision Core (Sdc)
======================

This document is written to accompany the opensubdiv/sdc source. It is intended
to give a high level overview of the organization of the source, the rationale
for various decisions, as well as highlight open issues or situations where the
choices made thus far warrant further deliberation.

The primary goal of Sdc is to separate the core subdivision logic in order to
facilate other classes internal (and possibly external) to OpenSubdiv
generating consistent and compliant results. For the purposes of this overview
Sdc is divided into three sections, with varying numbers of header files for
each:

    * types, traits and options for the supported subdivision schemes
    * computations required to support semi-sharp creasing
    * computations of mask weights for subdivided vertices for all schemes

Each section will reference a specific subset of a small number of headers.
Comments within the headers may elaborate on the details provided in the
synopses, but enough context will be provided here to raise the main issues, and
the source should then serve as an illustration.

Synopsis
********

Overall the approach taken was to extract the functionality at as low a level as
possible.  In some cases they are not far from being simple global functions.
The intent was to start at a low level and build any higher level functionality
as needed.  What exists now is functional for ongoing development and
anticipated needs within OpenSubdiv for the near future.

Types, Traits and Options
=========================

Associated headers:

    * `sdc/type.h`
    * `sdc/options.h`



Type
****

The most basic addition here is an enumerated type that identifies the fixed set
of subdivision schemes supported by OpenSubdiv:  Bilinear, Catmark and Loop.

With the definition of this enumerated type, we can avoid all of the dynamic
casting issues related to the scheme by simply adding members to the associated
subclasses for inspection.  In addition to the type enum itself, a class
defining a set of TypeTraits<TYPE> for each scheme is provided, which is to be
specialized for each scheme.

Options
*******

The second contribution is the collection of all variations in one place that
can be applied to the subdivision schemes, i.e. the boundary interpolation
rules, creasing method, edge subdivision choices, etc. The fact that these are
all declared in one place alone should help clients see the full set of
variations that are possible.

A simple Options struct (a set of bitfields) aggregates all of these variations
into a single object (an integer in this case) that are passed around to other
`Sdc <sdc_overview.html>`__ classes and/or methods and are expected to be used
at a higher level both within OpenSubdiv and externally. By aggregating the
options and passing them around as a group, it allows us to extend the set
easily in future without the need to rewire a lot of interfaces to accomodate
the new choice.  Clients can enables new choices at the highest level and be
assured that they will propogate to the lowest level where they are relevant.
