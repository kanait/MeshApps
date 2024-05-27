////////////////////////////////////////////////////////////////////
//
// $Id: FaceL.hxx 2022/05/22 21:03:58 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _FACEL_HXX
#define _FACEL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include <vector>
#include <list>
using namespace std;

#include "myEigen.hxx"

#include "VertexL.hxx"
#include "HalfedgeL.hxx"
#include "NormalL.hxx"
#include "TexcoordL.hxx"
#include "NodeL.hxx"

class MeshL;

////////////////////////////////////////////////////////////////////////
//
// Halfedge への（巡回リストとしての）アクセス方法
//
//   HalfedgeL* he = begin();
//   do {
//
//     .....
//
//     he = he->next();
//   }
//   while ( he != begin() );
//
////////////////////////////////////////////////////////////////////////

class FaceL : public NodeL {

public:

  FaceL() : NodeL() { init(); };
  FaceL( int id ) : NodeL(id) { init(); };
  ~FaceL(){ deleteHalfedges(); };

  void init() {
    texid_ = 0;
  };

  // face normal
  Eigen::Vector3d& normal() { return normal_; };
  void setNormal( Eigen::Vector3d& norm ) { normal_ = norm; };
  void calcNormal( Eigen::Vector3d& norm ) { calcNormal(); norm = normal_; };
  void calcNormal() {
    auto he_iter = halfedges_.begin();
    Eigen::Vector3d& p0 = (*he_iter)->vertex()->point(); he_iter++;
    Eigen::Vector3d& p1 = (*he_iter)->vertex()->point(); he_iter++;
    Eigen::Vector3d& p2 = (*he_iter)->vertex()->point();
    Eigen::Vector3d v1(p1 - p0);
    Eigen::Vector3d v2(p2 - p0);
    normal_ = v1.cross(v2);
    normal_.normalize();
  };

  void calcParamNormal( Eigen::Vector3d& nm ) {
    auto he_iter = halfedges_.begin();
    Eigen::Vector3d& p0 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p1 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p2 = (*he_iter)->texcoord()->point();
    Eigen::Vector3d v1(p1-p0);
    Eigen::Vector3d v2(p2-p0);
    nm = v1.cross(v2);
    nm.normalize();
  };

  int size() const { return halfedges_.size(); };
  std::list<HalfedgeL*>& halfedges() { return halfedges_; };
  void addHalfedge( HalfedgeL* he ) {
    he->setFaceandFIter( this,
                         halfedges_.insert( halfedges_.end(), he ),
                         halfedges_ );
  };
  void addHalfedge( HalfedgeL* he, VertexL* vt, NormalL* nm ) {
    addHalfedge( he );
    he->setVertex( vt );
    he->setNormal( nm );
  };
  void addHalfedge( HalfedgeL* he, VertexL* vt, NormalL* nm, TexcoordL* tc ) {
    addHalfedge( he );
    he->setVertex( vt );
    he->setNormal( nm );
    he->setTexcoord( tc );
  };
  HalfedgeL* begin() { return *(halfedges_.begin()); };
  HalfedgeL* halfedge( int n ) {
    std::list<HalfedgeL*>::iterator hiter = halfedges_.begin();
    for ( int i = 0; i < n; ++i ) hiter++;
    return *hiter;
  };
  void deleteHalfedges() {
    for (auto he : halfedges_) {
      delete he;
    }
    halfedges_.clear();
  };

  // iter
  void setIter( std::list<FaceL*>::iterator iter ) { iter_ = iter; };
  std::list<FaceL*>::iterator iter() const { return iter_; };

  // texture id
  void setTexID( int i ) { texid_ = i; };
  int texID() const { return texid_; };

  // check
  // true if a face has both vertices a and b
  bool checkVertex( VertexL* a, VertexL* b ) {
    bool flag = false;
    for (auto he : halfedges_)
      if ( he->vertex() == a ) flag = true;
    if ( flag == false ) return false;

    flag = false;
    for (auto he : halfedges_)
      if ( he->vertex() == b ) flag = true;
    if ( flag == false ) return false;

    return true;
  };

  double area() {
    auto he_iter = halfedges_.begin();
    Eigen::Vector3d& p0 = (*he_iter)->vertex()->point(); he_iter++;
    Eigen::Vector3d& p1 = (*he_iter)->vertex()->point(); he_iter++;
    Eigen::Vector3d& p2 = (*he_iter)->vertex()->point();
    Eigen::Vector3d v1(p1-p0);
    Eigen::Vector3d v2(p2-p0);
    return .5 * v1.cross(v2).norm();
  };

  double areaTexcoord() {
    auto he_iter = halfedges_.begin();
    Eigen::Vector3d& p0 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p1 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p2 = (*he_iter)->texcoord()->point();
    Eigen::Vector2d q0( p0.x(), p0.y() );
    Eigen::Vector2d q1( p1.x(), p1.y() );
    Eigen::Vector2d q2( p2.x(), p2.y() );
    return triArea2d( q0, q1, q2 );
  };

  double areaTexcoord3d() {
    auto he_iter = halfedges_.begin();
    Eigen::Vector3d& p0 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p1 = (*he_iter)->texcoord()->point(); he_iter++;
    Eigen::Vector3d& p2 = (*he_iter)->texcoord()->point();
    Eigen::Vector3d v1(p1-p0);
    Eigen::Vector3d v2(p2-p0);
    return .5 * v1.cross(v2).norm();
}
  double areaScale() { return std::sqrt(areaTexcoord3d() / area()); };

  void calcBarycentricPoint( Eigen::Vector3d& p ) {
    p = Eigen::Vector3d::Zero();
    for (auto he : halfedges_)
      p += he->vertex()->point();
    p *= (1/(double)halfedges_.size());
    // p.scale( 1.0/3.0 );
  };

  void findBarycentricCoordinate2d( Eigen::Vector2d& p, Eigen::Vector3d& bc ) {
    auto he = halfedges_.begin();
    Eigen::Vector3d& p1 = (*he)->texcoord()->point(); he++;
    Eigen::Vector3d& p2 = (*he)->texcoord()->point(); he++;
    Eigen::Vector3d& p3 = (*he)->texcoord()->point();
    Eigen::Vector2d vec1( p1.x(), p1.y() );
    Eigen::Vector2d vec2( p2.x(), p2.y() );
    Eigen::Vector2d vec3( p3.x(), p3.y() );
    double area = triArea2d( vec1, vec2, vec3 );
    bc << (triArea2d( p, vec2, vec3 ) / area), (triArea2d( p, vec3, vec1 ) / area), (triArea2d( p, vec1, vec2 ) / area);
  };

  void barycentricCoordinate( Eigen::Vector3d& bc, Eigen::Vector3d& p ) {
    auto he = halfedges_.begin();
    Eigen::Vector3d& vec1 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d& vec2 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d& vec3 = (*he)->vertex()->point();
    // p ではなく bc では？ check
    // p.set( p.x * vec1 + p.y * vec2 + p.z * vec3 );
    //p = p.x() * vec1 + p.y() * vec2 + p.z() * vec3;
    p = bc.x() * vec1 + bc.y() * vec2 + bc.z() * vec3;
  };

  bool isTexcoordInFace( Eigen::Vector2d& p ) {
    HalfedgeL* he = begin(); //     flc.beginHalfedgeL();
    do {
      Eigen::Vector3d& sv0 = he->texcoord()->point(); //he = flc.nextHalfedgeL();
      Eigen::Vector3d& ev0 = he->next()->texcoord()->point();
      Eigen::Vector2d sv( sv0.x(), sv0.y() );
      Eigen::Vector2d ev( ev0.x(), ev0.y() );
      if ( isLeftSide2d( sv, ev, p ) == false ) return false;
      he = he->next();
    } while ( he != begin() );

    return true;
  };

  bool isVertexInFace( VertexL* vt ) {
    for (auto he : halfedges_) {
      if ( he->vertex() == vt ) return true;
    }
    return false;
  };

  bool isReversed() {
    for (auto he : halfedges_) {
      Eigen::Vector3d& fn1 = normal_;
      Eigen::Vector3d& fn2 = he->mate()->face()->normal();
      double ang = V3AngleBetweenVectors(fn1,fn2);
      // if ( ang > DEG150 )
      if ( (ang > DEG150) &&
           (std::fabs(he->face()->area()) > ZEROEPS) &&
           (std::fabs(he->mate()->face()->area()) > ZEROEPS) ) {
        // std::cerr << "Angle is more than 90 degree: face no. " << (*fc)->id()
        //           << " angle " << ang * 180 / M_PI << std::endl;
        return true;
      }
    }
    return false;
  };

  // R = abc/4A (a,b,c: edge lengths, A: area)
  double circumRadius() {
    auto he = halfedges_.begin();
    Eigen::Vector3d p1 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d p2 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d p3 = (*he)->vertex()->point();

    double a = (p1-p2).norm();
    double b = (p2-p3).norm();
    double c = (p3-p1).norm();
    double A = area();
    return a*b*c/(4.0*A);
  };

  // r = abc/4Rs (a,b,c:edge lengths, R:circumradius, s: (a+b+c)/2)
  void radii(double& cR, double& iR) {
    auto he = halfedges_.begin();
    Eigen::Vector3d& p1 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d& p2 = (*he)->vertex()->point(); he++;
    Eigen::Vector3d& p3 = (*he)->vertex()->point();

    double a = (p1-p2).norm();
    double b = (p2-p3).norm();
    double c = (p3-p1).norm();
    double s = (a+b+c)/2.0;
    //double AA = area();
    double A = std::sqrt(s*(s-a)*(s-b)*(s-c)); // Herons' formula

    // circumradius
    cR = a*b*c/(4.0*A);
    iR = a*b*c/(4.0*cR*s);
  };

  void print() {
    std::cout << "Face: " << id() << " vertex: ";
    for (auto he : halfedges_) {
      std::cout << he->vertex()->id() << " ";
    }
    std::cout << std::endl;
    for (auto he : halfedges_) {
      std::cout << he->vertex()->point() << " " << std::endl;
    }
    std::cout << " halfedges: ";
    for (auto he : halfedges_) {
      std::cout << he->id() << " ";
    }
    std::cout << std::endl;
  };

private:

  // face normal
  Eigen::Vector3d normal_;

  // texture object id
  int texid_;

  // list of halfedges
  std::list<HalfedgeL*> halfedges_;

  // list iterator of MeshL
  std::list<FaceL*>::iterator iter_;
};

#endif // _FACE_H
