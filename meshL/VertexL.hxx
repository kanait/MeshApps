////////////////////////////////////////////////////////////////////
//
// $Id: VertexL.hxx 2022/05/22 20:54:26 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _VERTEXL_HXX
#define _VERTEXL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include <list>
using namespace std;

#include "myEigen.hxx"

#include "NodeL.hxx"
class HalfedgeL;

class VertexL : public NodeL {

public:

  VertexL() : NodeL(), halfedge_(NULL) {};
  VertexL( int id ) : NodeL(id), halfedge_(NULL) {};
  ~VertexL(){};

  // point
  Eigen::Vector3d& point() { return point_; };
  void setPoint( Eigen::Vector3d& point ) { point_ = point; };
  void setPoint( double x, double y, double z ) {
    point_ << x, y, z;
  };

  // iter
  void setIter( std::list<VertexL*>::iterator iter ) { iter_ = iter; };
  std::list<VertexL*>::iterator iter() const { return iter_; };

  //
  // halfedge
  //
  HalfedgeL* halfedge() const { return halfedge_; };
  void setHalfedge( HalfedgeL* halfedge ) { halfedge_ = halfedge; };

private:

  // 3D coord
  Eigen::Vector3d point_;

  // one of halfedges
  HalfedgeL* halfedge_;

  // list iterator of MeshL
  std::list<VertexL*>::iterator iter_;

};

#endif // _VERTEX_HXX
