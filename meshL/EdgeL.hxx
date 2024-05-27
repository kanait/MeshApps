////////////////////////////////////////////////////////////////////
//
// $Id: EdgeL.hxx 2022/05/22 18:38:59 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _EDGEL_HXX
#define _EDGEL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include <list>
using namespace std;

#include "NodeL.hxx"

class VertexL;
#include "HalfedgeL.hxx"

class MeshL;

// Edge ‚ÆŠe—v‘f‚ÌŠÖŒW
//            ev
//            |
//            |
//    lf  <-  |  -> rf
//            |
//            |
//            sv
class EdgeL : public NodeL {

public:

  EdgeL() : NodeL() { init(); };
  EdgeL( int id ) : NodeL(id) { init(); };
//    EdgeL( VertexL* sv, VertexL* ev, HalfedgeL* lhe, HalfedgeL* rhe ) {
//      setVertices( sv, ev );
//      setLHalfedge( lhe ); setRHalfedge( lhe );
//    };
  ~EdgeL(){};
  void init() {
//     iter_ = NULL;
    sv_ = NULL; ev_ = NULL;
    lhe_ = NULL; rhe_ = NULL;
  };

  // vertex
  VertexL* sv() const { return sv_; };
  VertexL* ev() const { return ev_; };
  void setSVertex( VertexL* sv ) { sv_ = sv; };
  void setEVertex( VertexL* ev ) { ev_ = ev; };
  void setVertices( VertexL* sv, VertexL* ev ) {
    setSVertex( sv ); setEVertex( ev );
  };

  // halfedge
  HalfedgeL* lhe() const { return lhe_; };
  HalfedgeL* rhe() const { return rhe_; };
  void setLHalfedge( HalfedgeL* lhe ) {
    lhe_ = lhe;
    lhe_->setEdge( this );
  };
  void setRHalfedge( HalfedgeL* rhe ) {
    rhe_ = rhe;
    rhe_->setEdge( this );
  };

  bool rhe_valid( HalfedgeL* rhe ) {
    // reverse order
    if ( (sv_ == rhe->next()->vertex()) && (ev_ == rhe->vertex()) ) return true; 
    return false;
  };
  
  bool isBoundary() const { return ( (lhe_) && (rhe_) ) ? false : true; };
  
  // iter
  void setIter( std::list<EdgeL*>::iterator iter ) { iter_ = iter; };
  std::list<EdgeL*>::iterator iter() const { return iter_; };

  // void setMesh( MeshL* );

private:

  VertexL* sv_;
  VertexL* ev_;
  HalfedgeL* lhe_;
  HalfedgeL* rhe_;

  // list iterator of MeshL
  std::list<EdgeL*>::iterator iter_;

};

class EdgeList {

  std::list<EdgeL*> l_;

public:

  EdgeList() {};
  ~EdgeList() {};

  EdgeL* findEdge( VertexL* sv, VertexL* ev ) {
    // foreach ( std::list<EdgeL*>, l_, ed )
    for (auto ed : l_) {
      if ( (ed->sv() == sv) && (ed->ev() == ev) )
        return ed;
      if ( (ed->ev() == sv) && (ed->sv() == ev) )
        return ed;
    }
    return NULL;
  };

  void push_back( EdgeL* ed ) { l_.push_back(ed); };

private:

  VertexL* sv_;
  VertexL* ev_;
  HalfedgeL* lhe_;
  HalfedgeL* rhe_;

  // list iterator of MeshL
  std::list<EdgeL*>::iterator iter_;

};

#endif // _EDGEL_HXX
