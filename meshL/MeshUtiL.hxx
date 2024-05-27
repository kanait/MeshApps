////////////////////////////////////////////////////////////////////
//
// $Id: MeshUtiL.hxx 2022/05/22 21:33:25 kanai Exp $
//
// Copyright (c) 2022 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _MESHUTIL_HXX
#define _MESHUTIL_HXX 1

#include "envDep.h"

#include "VertexL.hxx"
#include "VertexLCirculator.hxx"

//
// VertexL utility
//

inline HalfedgeL* findHalfedge( VertexL* o, VertexL *vt ) {
  VertexLCirculator vc( o );
  HalfedgeL* he = vc.beginHalfedgeL();
  do {
    if ( he->next()->vertex() == vt ) return he;

    HalfedgeL* mate = he->mate();
    if ( mate )
      {
        if ( mate->vertex() == vt ) return mate;
      }
    he = vc.nextHalfedgeL();
  } while ( (he != NULL) && (he != vc.firstHalfedgeL()) );

  return NULL;
}

inline void reset_halfedge( VertexL* vt ) {
  if ( vt->halfedge() == NULL ) return;
  HalfedgeL* he = vt->halfedge();
  if ( he->mate() == NULL ) return;
  do {
    he = he->mate()->next();
  } while ( (he->mate() != NULL) && ( he != vt->halfedge()) );
  vt->setHalfedge(he);
}

inline bool isBoundary( VertexL* vt ) {
  VertexLCirculator vc( vt );
  HalfedgeL* he = vc.beginHalfedgeL();
  do {
    he = vc.nextHalfedgeL();
    if ( he == NULL ) return true;
  } while ( he != vc.firstHalfedgeL() );

  return false;
}

inline int valence( VertexL* ovt ) {
  int count = 0;
  VertexLCirculator vc( ovt );
  VertexL* vt = vc.beginVertexL();
  do {
    count++;
    vt = vc.nextVertexL();
  } while ( (vt != vc.firstVertexL()) && (vt != NULL) );

  return count;
}

inline void calcVertexNormal( VertexL* vt, Eigen::Vector3d& nv ) {
  nv = Eigen::Vector3d::Zero();

  int i = 0;
  VertexLCirculator vc( vt );
  FaceL* fc = vc.beginFaceL();
  if ( fc == NULL ) return;
  do {
    // add face normal
    ++i;
    nv += fc->normal();
    fc = vc.nextFaceL();
  } while ( (fc != vc.firstFaceL()) && (fc != NULL) );

  // divided by its valence
  nv /= (double)i;
  nv.normalize();
}

inline void printNeighborFaces(VertexL* vt) {
  VertexLCirculator vc( vt );
  cout << "(nf) center vt " << vt->id() << " boundary " << isBoundary(vt) << endl;
  FaceL* fc = vc.beginFaceL();
  do {
    cout << "\t"; fc->print();
    fc = vc.nextFaceL();
  } while ( (fc != vc.firstFaceL()) && (fc != NULL) );
  cout << endl;
}

inline void printNeighborVertices(VertexL* ovt) {
  VertexLCirculator vc( ovt );
  cout << "(nv) center vt " << ovt->id() << " boundary " << isBoundary(ovt) << endl;
  VertexL* vt = vc.beginVertexL();
  do {
    cout << "\t Vertex: " << vt->id() << endl;
    vt = vc.nextVertexL();
  } while ( (vt != vc.firstVertexL()) && (vt != NULL) );
  cout << endl;
}

#endif // _MESHUTIL_HXX
