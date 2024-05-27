////////////////////////////////////////////////////////////////////
//
// $Id: HalfedgeL.hxx 2022/05/22 21:06:48 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _HALFEDGEL_HXX
#define _HALFEDGEL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include "myEigen.hxx"

#include <list>
using namespace std;

#include "VMProc.hxx"

#include "NodeL.hxx"
#include "VertexL.hxx"
#include "NormalL.hxx"
#include "TexcoordL.hxx"
class EdgeL;
class FaceL;

class HalfedgeL : public NodeL {

public:

  HalfedgeL() : NodeL() {init();};
  HalfedgeL( int id ) : NodeL(id) { init(); };
  ~HalfedgeL(){};

  void init() {
    vertex_ = NULL;
    normal_ = NULL;
    texcoord_ = NULL;
    mate_ = NULL;
    edge_ = NULL;
    face_ = NULL;
  };

#if 1
  std::list<HalfedgeL*>::iterator f_begin() const { return f_hlist_->begin(); };
  std::list<HalfedgeL*>::iterator f_end() const { return f_hlist_->end(); };
#endif

  HalfedgeL* next() const {
    auto h_iter = f_iter_;
    h_iter++;
    return ( h_iter != f_end() ) ? *h_iter : *(f_begin());
  };

  HalfedgeL* prev() const {
    auto h_iter = f_iter_;
    if ( h_iter != f_begin() ) --h_iter;
    else {
      h_iter = f_end();
      --h_iter;
    }
    return *(h_iter);
  };

  // iterator of FaceL
  std::list<HalfedgeL*>::iterator f_iter() const { return f_iter_; };
  void setFIter( std::list<HalfedgeL*>::iterator iter ) { f_iter_ = iter; };
  void setFHalfedges( std::list<HalfedgeL*>& f_hlist ) { 
    f_hlist_ = &(f_hlist);
  };

  // vertex
  VertexL* vertex() const { return vertex_; };
  void setVertex( VertexL* vt ) { vertex_ = vt; };

  VertexL* next_vertex() const { return next()->vertex(); };
  VertexL* prev_vertex() const { return prev()->vertex(); };
  VertexL* opposite_vertex() const { return prev_vertex(); };

  // mate
  HalfedgeL* mate() const { return mate_; };
  bool mate_valid( HalfedgeL* he ) const {
    return ( (vertex() == he->next_vertex()) && (next_vertex() == he->vertex()) );
  };
  void setMate( HalfedgeL* he ) { mate_ = he; };
  void setBothMate( HalfedgeL* he ) { setMate(he); he->setMate(this); };
  //boundary
  bool isBoundary() const { return ( mate_ != NULL ) ? false : true; };

  // normal
  bool isNormal() const { return ( normal_ != NULL ) ? true : false; };
  NormalL* normal() const { return normal_; };
  void setNormal( NormalL* nm ) { normal_ = nm; };

  // texcoord
  bool isTexcoord() const { return ( texcoord_ != NULL ) ? true : false; };
  TexcoordL* texcoord() const { return texcoord_; };
  void setTexcoord( TexcoordL* tc ) { texcoord_ = tc; };

  // face
  FaceL* face() const { return face_; };
  void setFace( FaceL* fc ) {
    face_ = fc;
    f_iter_ = f_end();
  };
  void setFaceandFIter( FaceL* fc,
                        std::list<HalfedgeL*>::iterator iter,
                        std::list<HalfedgeL*>& f_hlist ) {
    face_ = fc;
    f_iter_ = iter;
    f_hlist_ = &(f_hlist);
  };

  // edge
  void setEdge( EdgeL* edge ) { edge_ = edge; };
  EdgeL* edge() const { return edge_; };

  // length
  double length() const {
    return Eigen::Vector3d(vertex()->point() - next()->vertex()->point()).norm();
  };

  // length
  double param_length() const {
    return Eigen::Vector3d(texcoord()->point() - next()->texcoord()->point()).norm();
  };

  // for texcoord
  HalfedgeL* findNextHalfedge( Eigen::Vector2d& v0, Eigen::Vector2d& v1 ) {
    HalfedgeL* he = this->next();
    do {
      Eigen::Vector3d& sv0 = he->texcoord()->point();
      Eigen::Vector3d& ev0 = he->next()->texcoord()->point();
      Eigen::Vector2d sv( sv0.x(), sv0.y() );
      Eigen::Vector2d ev( ev0.x(), ev0.y() );
      if ( isLineSegmentCrossing2d( sv, ev, v0, v1 ) == true ) {
        return he->mate();
      }
      he = he->next();
    } while ( he != this );

    return NULL;
  };

  // re-attaching vertex's halfedge to the end.
  HalfedgeL* reset() {
    HalfedgeL* he = this;
    if ( he->mate() == NULL ) return this;
    do {
      he = he->mate()->next();
    } while ( (he->mate() != NULL) && ( he != this) );

    return he;
  };


private:

  // vertex
  VertexL* vertex_;

  // normal
  NormalL* normal_;

  // texture coordinate
  TexcoordL* texcoord_;

  HalfedgeL* mate_;
  FaceL*   face_; // back face

  EdgeL* edge_;

  // list iterator of FaceL
  std::list<HalfedgeL*>::iterator f_iter_;
  std::list<HalfedgeL*>* f_hlist_;

};

#endif // _HALFEDGEL_HXX
