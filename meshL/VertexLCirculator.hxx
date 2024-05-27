////////////////////////////////////////////////////////////////////
//
// $Id: VertexLCirculator.hxx 2021/06/05 14:20:31 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _VERTEXLCIRCULATOR_HXX
#define _VERTEXLCIRCULATOR_HXX

#include "VertexL.hxx"
#include "HalfedgeL.hxx"
#include "FaceL.hxx"

class VertexLCirculator {

  VertexL* vt_;

protected:
  
  HalfedgeL* temp_halfedge_;

public:

  VertexLCirculator() { clear(); };
  VertexLCirculator( VertexL* vt ) { clear(); setVertex( vt ); };
  ~VertexLCirculator() {};

  void clear() { vt_ = NULL; temp_halfedge_ = NULL; };
  void setVertex( VertexL* vt ) { vt_ = vt; };

  //
  // vertex -> face
  //
  FaceL* beginFaceL() {
    temp_halfedge_ = vt_->halfedge();
    assert( temp_halfedge_ );
    return temp_halfedge_->face();
  };
  
  FaceL* nextFaceL() {
    HalfedgeL* he = temp_halfedge_;
    temp_halfedge_ = he->prev()->mate();
    if ( temp_halfedge_ == NULL ) return NULL;
    //cout << "\t\t next face " << temp_halfedge_->face()->id() << endl;
    return temp_halfedge_->face();
  };

  FaceL* firstFaceL() {
    return vt_->halfedge()->face();
  };

  FaceL* lastFaceL() {
    if ( vt_->halfedge()->mate() == NULL ) return NULL;
    return vt_->halfedge()->mate()->face();
  };
  
  //
  // vertex -> vertex
  //
  VertexL* beginVertexL() {
    temp_halfedge_ = vt_->halfedge();
    assert( temp_halfedge_ );
    return temp_halfedge_->next()->vertex();
  };

  VertexL* nextVertexL() {
    HalfedgeL* he = temp_halfedge_;
    if ( he == NULL ) return NULL;
    temp_halfedge_ = he->prev()->mate();
    if ( temp_halfedge_ == NULL )
      {
        // last vertex
        return he->prev()->vertex();
      }
    //return NULL;
    //cout << "\t\t next face " << temp_halfedge_->face()->id() << endl;
    return temp_halfedge_->next()->vertex();
  };

  VertexL* firstVertexL() {
    return vt_->halfedge()->next()->vertex();
  };
  
  VertexL* lastVertexL() {
    if ( vt_->halfedge()->mate() == NULL ) return NULL;
    return vt_->halfedge()->mate()->next()->vertex();
  };
  
  //
  // vertex -> halfedge
  //
  HalfedgeL* beginHalfedgeL() {
    temp_halfedge_ = vt_->halfedge();
    assert( temp_halfedge_ );
    return temp_halfedge_;
  };

  HalfedgeL* nextHalfedgeL() {
    HalfedgeL* he = temp_halfedge_;
    temp_halfedge_ = he->prev()->mate();
    if ( temp_halfedge_ == NULL ) return NULL;
    return temp_halfedge_;
  };

  HalfedgeL* firstHalfedgeL() {
    return vt_->halfedge();
  }
  
  HalfedgeL* lastHalfedgeL() {
    if ( vt_->halfedge()->mate() == NULL ) return NULL;
    return vt_->halfedge()->mate()->next();
  };
  
  //
  //  vertex -> reverse halfedge
  //
  HalfedgeL* beginRevHalfedgeL() {
    assert( vt_->halfedge() );
    temp_halfedge_ = vt_->halfedge()->prev();
    return temp_halfedge_;
  };

  HalfedgeL* nextRevHalfedgeL() {
    if ( temp_halfedge_->mate() == NULL ) return NULL;
    HalfedgeL* he = temp_halfedge_;
    temp_halfedge_ = he->mate()->prev();
    return temp_halfedge_;
  };

  HalfedgeL* firstRevHalfedgeL() {
    return vt_->halfedge()->prev();
  };
  
  HalfedgeL* lastRevHalfedgeL() {
    if ( vt_->halfedge()->mate() == NULL ) return NULL;
    return vt_->halfedge()->mate();
  };

  int num_vertices() {
    int count=0;
    VertexL* vt = beginVertexL();
//      cout << "(vv) vt " << vt_->id() << " ";
    do { count++; cout << vt->id() << " "; vt = nextVertexL();} while ( (vt != firstVertexL()) && (vt != NULL) );
//      cout << endl;
    return count;
  };
  
  int num_faces() {
    int count=0;
    FaceL* fc = beginFaceL();
//      cout << "(vf) vt " << vt_->id() << " ";
    do { count++; cout << fc->id() << " "; fc = nextFaceL(); } while ( (fc != firstFaceL()) && (fc != NULL) );
//      cout << endl;
    return count;
  };
  
  
};
  
  
#endif // _VERTEXLCIRCULATOR_HXX


