////////////////////////////////////////////////////////////////////
//
// $Id: BBox2.hxx 2022/04/02 15:29:32 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _BBOX2_HXX
#define _BBOX2_HXX 1

#include <cmath>
using namespace std;

#include "myEigen.hxx"
// #include <Point2.h>
// #ifdef VM_INCLUDE_NAMESPACE
// using namespace kh_vecmath;
// #endif // VM_INCLUDE_NAMESPACE

class BBox2 {

  Eigen::Vector2i start;
  Eigen::Vector2i end;

public:

  BBox2() {};
  ~BBox2() {};

  void clear() {
    start = Eigen::Vector2i::Zero();
    end = Eigen::Vector2i::Zero();
  };
  void setStart( int x, int y ) { start = Eigen::Vector2i( x, y ); };
  void setEnd( int x, int y ) { end = Eigen::Vector2i( x, y ); };

  int& sx() { return start.x(); };
  int& sy() { return start.y(); };
  int& ex() { return end.x(); };
  int& ey() { return end.y(); };

  bool isRectangle() {
    return ( (std::abs(start.x() - end.x()) > 4) && (std::abs(start.y() - end.y()) > 4) );
  };
};
  
#endif // _BBOX2_HXX
