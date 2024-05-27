////////////////////////////////////////////////////////////////////
//
// $Id: Arcball.hxx 2022/05/19 16:06:31 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _ARCBALL_HXX
#define _ARCBALL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include <cstdio>
#include <cmath>
using namespace std;

#include "myEigen.hxx"
// #include <Quat4.h>
// #include <Matrix4.h>
// #ifdef VM_INCLUDE_NAMESPACE
// using namespace kh_vecmath;
// #endif // VM_INCLUDE_NAMESPACE

class Arcball {
  
public:

  Arcball() { init(); };
  ~Arcball(){};

  void init() {
    const Eigen::Quaternionf qOne = Eigen::Quaternionf::Identity();
    //(1.0f, 0.0, 0.0, 0.0);
      center_ = qNow_ = qDown_ = qOne;
      //mNow_.set(qNow_);
      //mNow_ = qNow_;
      mNow_ = Eigen::Matrix4f::Identity();
      mag_ = 1.0f;
      magObject_ = 1.0f;
      wheelScale_ = .1f;
      seezo_ = 0.0f;
      scrn_x_ = scrn_y_ = 0;
      offset_ = Eigen::Vector3f::Zero();
  };

  Eigen::Quaternionf mouse_on_sphere( const int x, const int y, const int x0, const int y0 ) {

    Eigen::Quaternionf sphere( .0f, (x - x0) / radius_, -(y - y0) / radius_, .0f );
    double mag = sphere.x() * sphere.x() + sphere.y() * sphere.y();
    if (mag > 1.0) 
      {
        double scale = 1.0 / std::sqrt(mag);
        sphere.x() *= scale;
        sphere.y() *= scale;
      }
    else
      {
        sphere.z() = std::sqrt(1.0 - mag);
      }

    return sphere;
  };

  void setDrag( void ) {
    qDrag_ = Eigen::Quaternionf( vFrom_.x()*vTo_.x() + vFrom_.y()*vTo_.y() + vFrom_.z()*vTo_.z(),
                                 vFrom_.y()*vTo_.z() - vFrom_.z()*vTo_.y(),
                                 vFrom_.z()*vTo_.x() - vFrom_.x()*vTo_.z(),
                                 vFrom_.x()*vTo_.y() - vFrom_.y()*vTo_.x() );

  };

  void setArc( void ) {
    double s = std::sqrt(qDrag_.x()*qDrag_.x() + qDrag_.y()*qDrag_.y());
    if (s == 0.0) {
      vrFrom_ = Eigen::Quaternionf( .0f, .0f, 1.0f, .0f );
      //vrFrom_.set(0.0, 1.0, 0.0, 0.0);
    } else {
      vrFrom_ = Eigen::Quaternionf( .0f, -qDrag_.y()/s, qDrag_.x()/s, 0.0f );
      //vrFrom_.set(-qDrag_.y/s, qDrag_.x/s, 0.0, 0.0);
    }
    vrTo_ = Eigen::Quaternionf( vrTo_.w(),
                                qDrag_.w() * vrFrom_.x() - qDrag_.z() * vrFrom_.y(),
                                qDrag_.w() * vrFrom_.y() + qDrag_.z() * vrFrom_.x(),
                                qDrag_.x() * vrFrom_.y() - qDrag_.y() * vrFrom_.x() );
    if (qDrag_.w() < 0.0) {
      vrFrom_ = Eigen::Quaternionf( .0f, -vrFrom_.x(), -vrFrom_.y(), 0.0f );
      //vrFrom_.set(-vrFrom_.x, -vrFrom_.y(), 0.0, 0.0);
    }
  };

  void radius(double r) {radius_ = r;}
  double radius(void) {return radius_;}
  void vFrom(const Eigen::Quaternionf vf) {vFrom_ = vf;}
  void vTo(const Eigen::Quaternionf vt) {vTo_ = vt;}
  void qDown(const Eigen::Quaternionf& q) {qDown_ = q;}
  void mDown(const Eigen::Matrix4f& m) {mDown_ = m;}
  Eigen::Quaternionf& center(void) {return center_;}
  Eigen::Quaternionf& qNow(void) {return qNow_;}
  Eigen::Quaternionf& qDown(void) {return qDown_;}
  Eigen::Quaternionf& qDrag(void) {return qDrag_;}
  Eigen::Quaternionf& vFrom(void) {return vFrom_;}
  Eigen::Quaternionf& vTo(void) {return vTo_;}
  Eigen::Quaternionf& vrFrom(void) {return vrFrom_;}
  Eigen::Quaternionf& vrTo(void) {return vrTo_;}
  Eigen::Matrix4f& mNow(void) {return mNow_;}
  Eigen::Matrix4f& mDown(void) {return mDown_;}

  void setOffset( Eigen::Vector3f& p ) {
    offset_ = p;
    //offset_.set( p );
  };
  void setMagObject( float f ) {
    magObject_ = f / 2.0f;
    //seezo_ = magObject_;
  };

  //
  Eigen::Vector3f& offset() { return offset_; };
  float seezo() const { return seezo_; };
  void setSeezo( float z ) { seezo_ = z; };

  //
  void setScrnXY( int x, int y ) {
    scrn_x_ = x; scrn_y_ = y;
  };
  int scrn_x() const { return scrn_x_; };
  int scrn_y() const { return scrn_y_; };

  void setHalfWHL( int w, int h ) {
    halfW_ = w; halfH_ = h;
    radius_ = std::sqrt( (double) halfW_*halfW_ + halfH_*halfH_ );
  };
  int halfW() const { return halfW_; };
  int halfH() const { return halfH_; };
  
  //
  void updateRotate( int dx, int dy ) {
    setDrag();
    //qNow().mul(qDrag(), qDown());
    qNow_ = qDrag_ * qDown_;
    setArc();
    Eigen::Quaternionf qc = qNow();
    qc.conjugate();
    // Eigen::Matrix4f& mc = mNow();
    //mNow().set(qc);
    Eigen::Matrix3f mat3 = qc.toRotationMatrix();
    mNow_.block(0,0,3,3) = mat3;
  };

  void updateMove( int x, int y, int ox, int oy ) {
    double dx = ( x - ox) / 200.0 / mag_ * magObject_;
    double dy = (-y + oy) / 200.0 / mag_ * magObject_;
    Eigen::Vector3f p( mNow_(0,0) * dx + mNow_(0,1) * dy,
                       mNow_(1,0) * dx + mNow_(1,1) * dy,
                       mNow_(2,0) * dx + mNow_(2,1) * dy );
    offset_ -= p;
    //offset_.sub(p);
  };

  void updateZoom( int x, int y, int ox, int oy ) {
    seezo_ += (x - ox) * .01 / mag_ * magObject_;
  };

  void updateWheelZoom( float x ) {
    seezo_ -= wheelScale_ * x;
  };

private:

  Eigen::Quaternionf center_;
  double radius_;
  Eigen::Quaternionf qNow_, qDown_, qDrag_;
  Eigen::Quaternionf vFrom_, vTo_, vrFrom_, vrTo_;
  Eigen::Matrix4f mNow_, mDown_;

  Eigen::Vector3f offset_;
  float mag_;
  float magObject_;
  float wheelScale_;
  float seezo_;

  int scrn_x_, scrn_y_;  // 取得した画面の座標
  int halfW_, halfH_;  // half size of window

};

#endif // _ARCBALL_HXX
