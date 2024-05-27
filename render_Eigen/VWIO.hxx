////////////////////////////////////////////////////////////////////
//
// $Id: VWIO.hxx 2021/06/13 15:17:02 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _VWIO_HXX
#define _VWIO_HXX 1

#include "envDep.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "myEigen.hxx"
// #include "Point3.h"
// #include "Quat4.h"
// #ifdef VM_INCLUDE_NAMESPACE
// using namespace kh_vecmath;
// #endif // VM_INCLUDE_NAMESPACE

#include "Arcball.hxx"

class VWIO {

public:

  VWIO() {};
  ~VWIO() {};

  bool inputFromFile( const char* const filename, Arcball& manip ) {
    std::ifstream ifs( filename );
    if ( !ifs )
      {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
      }

    std::string cline;
    char buf[BUFSIZ], dummy[BUFSIZ];

    // comment
    ifs.getline(buf, BUFSIZ, '\n');

    // seezo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    float seezo;
    sscanf( cline.c_str(), "%s%f", dummy, &seezo );
    manip.setSeezo( seezo );

    // eye
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Vector3f& o = manip.offset();
    sscanf( cline.c_str(), "%s%f%f%f", dummy, &(o.x()), &(o.y()), &(o.z()) );

    // center
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    char val[BUFSIZ];
    sscanf( cline.c_str(), "%s%s", dummy, val );
    manip.radius( atof(val) );

    //Eigen::Quaternionf dq;
  
    // center
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& dq = manip.center();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(dq.x()), &(dq.y()), &(dq.z()), &(dq.w()) );
    //manip.center().set( dq );
  
    // qNow
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& qn = manip.qNow();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(qn.x()), &(qn.y()), &(qn.z()), &(qn.w()) );
    //manip.qNow().set( dq );
  
    // qDown
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& qd = manip.qDown();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(qd.x()), &(qd.y()), &(qd.z()), &(qd.w()) );
    //manip.qDown().set( dq );
  
    // qDrag
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& qdr = manip.qDrag();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(qdr.x()), &(qdr.y()), &(qdr.z()), &(qdr.w()) );
    //manip.qDrag().set( dq );
  
    // vFrom
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& vf = manip.vFrom();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(vf.x()), &(vf.y()), &(vf.z()), &(vf.w()) );
    //manip.vFrom().set( dq );
  
    // vTo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& vt = manip.vTo();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(vt.x()), &(vt.y()), &(vt.z()), &(vt.w()) );
    //manip.vTo().set( dq );
  
    // vrFrom
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& vrf = manip.vrFrom();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(vrf.x()), &(vrf.y()), &(vrf.z()), &(vrf.w()) );
    //manip.vrFrom().set( dq );
  
    // vrTo
    ifs.getline(buf, BUFSIZ, '\n');
    cline.assign( buf );
    Eigen::Quaternionf& vrt = manip.vrTo();
    sscanf( cline.c_str(), "%s%f%f%f%f", dummy, &(vrt.x()), &(vrt.y()), &(vrt.z()), &(vrt.w()) );
    //manip.vrTo().set( dq );
  
    // mNow
    ifs.getline(buf, BUFSIZ, '\n');
    float m[4][4];
    unsigned int i;
    for ( i = 0; i < 4; ++i )
      {
        ifs.getline(buf, BUFSIZ, '\n');
        cline.assign( buf );
        sscanf( cline.c_str(), "%f%f%f%f", &(m[i][0]), &(m[i][1]), &(m[i][2]), &(m[i][3]) );
      }
    Eigen::Matrix4f& mn = manip.mNow();
    mn << m[0][0], m[0][1], m[0][2], m[0][3],
      m[1][0], m[1][1], m[1][2], m[1][3],
      m[2][0], m[2][1], m[2][2], m[2][3],
      m[3][0], m[3][1], m[3][2], m[3][3];

    // mDown
    ifs.getline(buf, BUFSIZ, '\n');
    for ( i = 0; i < 4; ++i )
      {
        ifs.getline(buf, BUFSIZ, '\n');
        cline.assign( buf );
        sscanf( cline.c_str(), "%f%f%f%f", &(m[i][0]), &(m[i][1]), &(m[i][2]), &(m[i][3]) );
      }
    Eigen::Matrix4f& md = manip.mDown();
    md <<  m[0][0], m[0][1], m[0][2], m[0][3],
      m[1][0], m[1][1], m[1][2], m[1][3],
      m[2][0], m[2][1], m[2][2], m[2][3],
      m[3][0], m[3][1], m[3][2], m[3][3];

    ifs.close();

    return true;
  };

  bool outputToFile( const char* const filename, Arcball& manip ) {
    std::ofstream ofs( filename );
    if ( !ofs )
      {
        std::cerr << "Cannot open " << filename << std::endl;
        return false;
      }

  
  
    ofs << "#VW Version 2.0" << std::endl;

    ofs << "seezo " << manip.seezo() << std::endl;
  
    Eigen::Vector3f o = manip.offset();
    ofs << "eye " << o.x() << " " << o.y() << " " << o.z() << std::endl;

    ofs << "radius " << manip.radius() << std::endl;

    ofs << "center ";
    ofs << manip.center().x() << " " << manip.center().y() << " ";
    ofs << manip.center().z() << " " << manip.center().w() << std::endl;
  
    ofs << "qNow ";
    ofs << manip.qNow().x() << " " << manip.qNow().y() << " ";
    ofs << manip.qNow().z() << " " << manip.qNow().w() << std::endl;
  
    ofs << "qDown ";
    ofs << manip.qDown().x() << " " << manip.qDown().y() << " ";
    ofs << manip.qDown().z() << " " << manip.qDown().w() << std::endl;
  
    ofs << "qDrag ";
    ofs << manip.qDrag().x() << " " << manip.qDrag().y() << " ";
    ofs << manip.qDrag().z() << " " << manip.qDrag().w() << std::endl;
  
    ofs << "vFrom ";
    ofs << manip.vFrom().x() << " " << manip.vFrom().y() << " ";
    ofs << manip.vFrom().z() << " " << manip.vFrom().w() << std::endl;
  
    ofs << "vTo ";
    ofs << manip.vTo().x() << " " << manip.vTo().y() << " ";
    ofs << manip.vTo().z() << " " << manip.vTo().w() << std::endl;
  
    ofs << "vrFrom ";
    ofs << manip.vrFrom().x() << " " << manip.vrFrom().y() << " ";
    ofs << manip.vrFrom().z() << " " << manip.vrFrom().w() << std::endl;
  
    ofs << "vrTo ";
    ofs << manip.vrTo().x() << " " << manip.vrTo().y() << " ";
    ofs << manip.vrTo().z() << " " << manip.vrTo().w() << std::endl;
  
    ofs << "mNow" << std::endl;
    Eigen::Matrix4f& mn = manip.mNow();
    ofs << mn(0,0) << " " << mn(0,1) << " ";
    ofs << mn(0,2) << " " << mn(0,3) << std::endl;
    ofs << mn(1,0) << " " << mn(1,1) << " ";
    ofs << mn(1,2) << " " << mn(1,3) << std::endl;
    ofs << mn(2,0) << " " << mn(2,1) << " ";
    ofs << mn(2,2) << " " << mn(2,3) << std::endl;
    ofs << mn(3,0) << " " << mn(3,1) << " ";
    ofs << mn(3,2) << " " << mn(3,3) << std::endl;

    ofs << "mDown" << std::endl;
    Eigen::Matrix4f& md = manip.mDown();
    ofs << md(0,0) << " " << md(0,1) << " ";
    ofs << md(0,2) << " " << md(0,3) << std::endl;
    ofs << md(1,0) << " " << md(1,1) << " ";
    ofs << md(1,2) << " " << md(1,3) << std::endl;
    ofs << md(2,0) << " " << md(2,1) << " ";
    ofs << md(2,2) << " " << md(2,3) << std::endl;
    ofs << md(3,0) << " " << md(3,1) << " ";
    ofs << md(3,2) << " " << md(3,3) << std::endl;
  
    ofs.close();

    return true;
  };

};

#endif // _VWIO_HXX

