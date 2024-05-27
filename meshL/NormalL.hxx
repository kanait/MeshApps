////////////////////////////////////////////////////////////////////
//
// $Id: NormalL.hxx 2022/05/22 21:10:03 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _NORMALL_HXX
#define _NORMALL_HXX 1

#include <list>
using namespace std;

#include "myEigen.hxx"

#include "NodeL.hxx"

class NormalL : public NodeL {

public:

  NormalL() : NodeL() { init(); };
  NormalL( int id ) : NodeL(id) { init(); };
  ~NormalL(){};

  void init() {};

  // vector
  Eigen::Vector3d& point() { return point_; };
  void setPoint( Eigen::Vector3d& p ) { point_ = p; };
  void setPoint( double& x, double& y, double& z ) {
    point_ << x, y, z;
  };

  // iter
  void setIter( std::list<NormalL*>::iterator iter ) { iter_ = iter; };
  std::list<NormalL*>::iterator iter() { return iter_; };

private:

  // normal vector
  Eigen::Vector3d point_;

  // list iterator of MeshL
  std::list<NormalL*>::iterator iter_;
};

#endif // _NORMALL_HXX


