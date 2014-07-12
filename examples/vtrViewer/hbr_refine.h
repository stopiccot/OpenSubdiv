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

#ifndef HBR_REFINE_H
#define HBR_REFINE_H


//------------------------------------------------------------------------------

template <class X> struct VertCompare {
    bool operator() (OpenSubdiv::HbrVertex<X> const * v1,
        OpenSubdiv::HbrVertex<X> const * v2 ) const {

        //return v1->GetID() < v2->GetID();
        return (void*)(v1) < (void*)(v2);
    }
};

//------------------------------------------------------------------------------
// True if the vertex can be incorporated into a B-spline patch
template <class T> bool
vertexIsBSpline(OpenSubdiv::HbrVertex<T> * v, bool next) {

    int valence = v->GetValence();

    // Boundary & corner vertices
    if (v->OnBoundary()) {
        if (valence==2) {
            // corner vertex

            OpenSubdiv::HbrFace<T> * f = v->GetFace();

            // the vertex may not need isolation depending on boundary
            // interpolation rule (sharp vs. rounded corner)
            typename OpenSubdiv::HbrMesh<T>::InterpolateBoundaryMethod method =
                f->GetMesh()->GetInterpolateBoundaryMethod();

            if (method==OpenSubdiv::HbrMesh<T>::k_InterpolateBoundaryEdgeAndCorner) {
                if (not next) {
                    // if we are checking coarse vertices (next==false),
                    // count the number of corners in the face, because we
                    // can only have 1 corner vertex in a corner patch.
                    int nsharpboundaries=0;
                    for (int i=0; i<f->GetNumVertices(); ++i) {
                        OpenSubdiv::HbrHalfedge<T> * e = f->GetEdge(i);
                        if (e->IsBoundary() and
                            e->GetSharpness()==OpenSubdiv::HbrHalfedge<T>::k_InfinitelySharp) {
                            ++nsharpboundaries;
                        }
                    }
                    return nsharpboundaries < 3 ? true: false;
                } else
                    return true;
            } else
                return false;
        } else if (valence>3) {
            // extraordinary boundary vertex (high valence)
            return false;
        }
        // regular boundary vertices have valence 3
        return true;
    }

    // Extraordinary or creased vertices that aren't corner / boundaries
    if (v->IsExtraordinary() or v->IsSharp(next))
        return false;

    return true;
}

//------------------------------------------------------------------------------
template <class T> void
refineVertexNeighbors(OpenSubdiv::HbrVertex<T> * v) {

    assert(v);

    OpenSubdiv::HbrHalfedge<T> * start = v->GetIncidentEdge(),
                               * next=start;
    do {

        OpenSubdiv::HbrFace<T> * lft = next->GetLeftFace(),
                               * rgt = next->GetRightFace();

        if (not ((lft and lft->IsHole()) and
                 (rgt and rgt->IsHole()) ) ) {

            if (rgt)
                rgt->_adaptiveFlags.isTagged=true;

            if (lft)
                lft->_adaptiveFlags.isTagged=true;

            OpenSubdiv::HbrHalfedge<T> * istart = next,
                                       * inext = istart;
            do {
                if (not inext->IsInsideHole()  )
                    inext->GetOrgVertex()->Refine();
                inext = inext->GetNext();
            } while (istart != inext);
        }
        next = v->GetNextEdge( next );
    } while (next and next!=start);
}


//------------------------------------------------------------------------------
//
// refine the Hbr mesh adaptively
//
template <class T>
void RefineAdaptive(OpenSubdiv::HbrMesh<T> & mesh, int maxlevel,
    std::vector<OpenSubdiv::HbrFace<T> const *> & refinedFaces) {

    int ncoarsefaces = mesh.GetNumCoarseFaces(),
        ncoarseverts = mesh.GetNumVertices(),
        maxValence=0;

    // First pass : tag coarse vertices & faces that need refinement

    typedef std::set<OpenSubdiv::HbrVertex<T> *,VertCompare<T> > VertSet;
    VertSet verts, nextverts;

    for (int i=0; i<ncoarseverts; ++i) {

        OpenSubdiv::HbrVertex<T> * v = mesh.GetVertex(i);

        // Non manifold topology may leave un-connected vertices that need to be skipped
        if (not v->IsConnected()) {
            continue;
        }

        // Tag non-BSpline vertices for refinement
        if (not vertexIsBSpline(v, false)) {
            v->_adaptiveFlags.isTagged=true;
            nextverts.insert(v);
        }
    }

    for (int i=0; i<ncoarsefaces; ++i) {
        OpenSubdiv::HbrFace<T> * f = mesh.GetFace(i);

        if (f->IsHole())
            continue;

        bool extraordinary = mesh.GetSubdivision()->FaceIsExtraordinary(&mesh,f);

        int nv = f->GetNumVertices();
        for (int j=0; j<nv; ++j) {

            OpenSubdiv::HbrHalfedge<T> * e = f->GetEdge(j);
            assert(e);

            // Tag sharp edges for refinement
            if (e->IsSharp(true) and (not e->IsBoundary())) {
                nextverts.insert(e->GetOrgVertex());
                nextverts.insert(e->GetDestVertex());

                e->GetOrgVertex()->_adaptiveFlags.isTagged=true;
                e->GetDestVertex()->_adaptiveFlags.isTagged=true;
            }

            // Tag extraordinary (non-quad) faces for refinement
            if (extraordinary or f->HasVertexEdits()) {
                OpenSubdiv::HbrVertex<T> * v = f->GetVertex(j);
                v->_adaptiveFlags.isTagged=true;
                nextverts.insert(v);
            }

            // Quad-faces with 2 non-consecutive boundaries need to be flagged
            // for refinement as boundary patches.
            //
            //  o ........ o ........ o ........ o
            //  .          |          |          .     ... boundary edge
            //  .          |   needs  |          .
            //  .          |   flag   |          .     --- regular edge
            //  .          |          |          .
            //  o ........ o ........ o ........ o
            //
            if ( e->IsBoundary() and (not f->_adaptiveFlags.isTagged) and nv==4 ) {

                if (e->GetPrev() and (not e->GetPrev()->IsBoundary()) and
                    e->GetNext() and (not e->GetNext()->IsBoundary()) and
                    e->GetNext() and e->GetNext()->GetNext() and e->GetNext()->GetNext()->IsBoundary()) {

                    // Tag the face so that we don't check for this again
                    f->_adaptiveFlags.isTagged=true;

                    // Tag all 4 vertices of the face to make sure 4 boundary
                    // sub-patches are generated
                    for (int k=0; k<4; ++k) {
                        OpenSubdiv::HbrVertex<T> * v = f->GetVertex(k);
                        v->_adaptiveFlags.isTagged=true;
                        nextverts.insert(v);
                    }
                }
            }
        }
        maxValence = std::max(maxValence, nv);
    }


    // Second pass : refine adaptively around singularities
    for (int level=0; level<maxlevel; ++level) {

        verts = nextverts;
        nextverts.clear();

        // Refine vertices
        for (typename VertSet::iterator i=verts.begin(); i!=verts.end(); ++i) {

            OpenSubdiv::HbrVertex<T> * v = *i;
            assert(v);

            if (level>0)
                v->_adaptiveFlags.isTagged=true;
            else
                v->_adaptiveFlags.wasTagged=true;

            refineVertexNeighbors(v);

            // Tag non-BSpline vertices for refinement
            if (not vertexIsBSpline(v, true))
                nextverts.insert(v->Subdivide());

            // Refine edges with creases or edits
            int valence = v->GetValence();
            maxValence = std::max(maxValence, valence);

            OpenSubdiv::HbrHalfedge<T> * e = v->GetIncidentEdge();
            for (int j=0; j<valence; ++j) {

                // Skip edges that have already been processed (HasChild())
                if ((not e->HasChild()) and e->IsSharp(false) and (not e->IsBoundary())) {

                    if (not e->IsInsideHole()) {
                        nextverts.insert( e->Subdivide() );
                        nextverts.insert( e->GetOrgVertex()->Subdivide() );
                        nextverts.insert( e->GetDestVertex()->Subdivide() );
                    }
                }
                OpenSubdiv::HbrHalfedge<T> * next = v->GetNextEdge(e);
                e = next ? next : e->GetPrev();
            }

            // Flag verts with hierarchical edits for neighbor refinement at the next level
            OpenSubdiv::HbrVertex<T> * childvert = v->Subdivide();
            OpenSubdiv::HbrHalfedge<T> * childedge = childvert->GetIncidentEdge();
            assert( childvert->GetValence()==valence);
            for (int j=0; j<valence; ++j) {
                OpenSubdiv::HbrFace<T> * f = childedge->GetFace();
                if (f->HasVertexEdits()) {
                    int nv = f->GetNumVertices();
                    for (int k=0; k<nv; ++k)
                        nextverts.insert( f->GetVertex(k) );
                }
                if ((childedge = childvert->GetNextEdge(childedge)) == NULL)
                    break;
            }
        }

        // Add coarse verts from extraordinary faces
        if (level==0) {
            for (int i=0; i<ncoarsefaces; ++i) {
                OpenSubdiv::HbrFace<T> * f = mesh.GetFace(i);
                assert (f->IsCoarse());

                if (mesh.GetSubdivision()->FaceIsExtraordinary(&mesh,f))
                    nextverts.insert( f->Subdivide() );
            }
        }
    }
    
    int nfaces = mesh.GetNumFaces();
    refinedFaces.reserve(nfaces - ncoarsefaces);
    for (int i=ncoarsefaces; i<nfaces; ++i) {

        OpenSubdiv::HbrFace<T> const * f = mesh.GetFace(i);

        if (f->_adaptiveFlags.isTagged) {
            continue;
        }
        
        refinedFaces.push_back(mesh.GetFace(i));
    }
}

//------------------------------------------------------------------------------
//
// refine the Hbr mesh uniformly
//
template <class T>
void RefineUniform(OpenSubdiv::HbrMesh<T> & mesh, int maxlevel,
    std::vector<OpenSubdiv::HbrFace<T> const *> & refinedFaces) {

    int nfaces = mesh.GetNumFaces();

    for (int level=0, firstface=0; level<=maxlevel; ++level ) {

        if (level==maxlevel) {

            refinedFaces.resize(nfaces-firstface);
            for (int i=firstface, ofs=0; i<nfaces; ++i) {
                refinedFaces[ofs++]=mesh.GetFace(i);
            }
        } else {

            for (int i=firstface; i<nfaces; ++i) {
                OpenSubdiv::HbrFace<T> * f = mesh.GetFace(i);
                assert(f->GetDepth()==level);
                if (not f->IsHole()) {
                    f->Refine();
                }
            }
        }

        // Hbr allocates faces sequentially, skip faces that have
        // already been refined.
        firstface = nfaces;
        nfaces = mesh.GetNumFaces();
    }
}

//------------------------------------------------------------------------------

#endif // HBR_REFINE_H
