////////////////////////////////////////////////////////////////////
//
// $Id: TexcoordL.hxx 2022/05/22 21:09:35 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _TEXCOORDL_HXX
#define _TEXCOORDL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include <list>
using namespace std;

#include "myEigen.hxx"

#include "NodeL.hxx"

class TexcoordL : public NodeL {

public:

  TexcoordL() : NodeL() { init(); };
  TexcoordL( int id ) : NodeL(id) { init(); };
  ~TexcoordL(){};

  void init() {};

  // vector
  Eigen::Vector3d& point() { return point_; };
  void setPoint( Eigen::Vector3d& p ) { point_ = p; };
  void setPoint( double x, double y, double z ) {
    point_ << x, y, z;
  };

  // iter
  void setIter( std::list<TexcoordL*>::iterator iter ) { iter_ = iter; };
  std::list<TexcoordL*>::iterator iter() { return iter_; };

private:

  // texture coordinate
  Eigen::Vector3d point_;

  // list iterator of MeshL
  std::list<TexcoordL*>::iterator iter_;

};

#endif // _TEXCOORDL_HXX

