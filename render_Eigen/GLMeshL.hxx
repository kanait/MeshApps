////////////////////////////////////////////////////////////////////
//
// $Id: GLMeshL.hxx 2022/05/20 14:28:52 kanai Exp $
//
//   OpenGL MeshL draw class
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _GLMESHL_HXX
#define _GLMESHL_HXX 1

#include "envDep.h"
#include "mydef.h"

#include "myGL.hxx"
#include "myEigen.hxx"

// #include <GL/gl.h>

#include "MeshL.hxx"
#include "FaceL.hxx"
#include "HalfedgeL.hxx"
#include "TexcoordL.hxx"
#include "VertexL.hxx"
#include "NormalL.hxx"
#include "BLoopL.hxx"
#include "GLMesh.hxx"

class GLMeshL : public GLMesh {

public:

  GLMeshL() : mesh_(NULL), isDrawWireframeBLoop_(true) {};
  ~GLMeshL() {
    //if ( mesh_ ) deleteMesh();
  };

  void setMesh( MeshL& mesh ) { mesh_ = &mesh; };
  MeshL& mesh() { return *mesh_; };
  void deleteMesh() { delete mesh_; mesh_ = NULL; };
  bool empty() const { return ( mesh_ != NULL ) ? false : true; };

  void setIsDrawWireframeBLoop( bool f ) { isDrawWireframeBLoop_ = f; };
  bool isDrawWireframeBLoop() const { return isDrawWireframeBLoop_; };

  void draw() {
    ::glPushMatrix();
    if ( isDrawShading_ ) { drawShading(); }
    if ( isDrawPoint_ ) { drawPoint(); }
    if ( isDrawWireframe_ ) { drawWireframe(); }

    if ( isDrawWireframeBLoop_ ) { drawWireframeBLoop(); }

    ::glPopMatrix();
  };

  void drawShading() {
    if ( empty() ) return;

    if ( isSmoothShading_ == false )
      ::glShadeModel( GL_FLAT );
    else
      ::glShadeModel( GL_SMOOTH );

    ::glEnable( GL_LIGHTING );
    //mtl_.set(0);
    // mtl_.bind();

    for ( auto fc : mesh().faces() ) {
#if 0
      if ( fc->isReversed() == false ) {
        mtl_.bind();
      } else {
        mtl_selected_.bind();
      }
#endif
      mtl_.bind();

      ::glBegin( GL_POLYGON );

      if ( isSmoothShading_ == false ) {
        Eigen::Vector3d& n = fc->normal();
        ::glNormal3d( n.x(), n.y(), n.z() );
      }

      for ( auto he : fc->halfedges() ) {

        if ( isSmoothShading_ && he->isNormal() ) {
          Eigen::Vector3d& n = he->normal()->point();
          ::glNormal3d( n.x(), n.y(), n.z() );
        }

        Eigen::Vector3d& p = he->vertex()->point();
        ::glVertex3d( p.x(), p.y(), p.z() );
      }

      ::glEnd();
    }

    ::glDisable( GL_LIGHTING );

  };

  void drawWireframe() {
    if ( empty() ) return;

    ::glColor3fv( wireColor() );
    ::glLineWidth( wireSize() );

    for ( auto fc : mesh().faces() ) {
      for ( auto he : fc->halfedges() ) {
        ::glBegin( GL_LINES );
        Eigen::Vector3d& p1 = he->vertex()->point();
        ::glVertex3d( p1.x(), p1.y(), p1.z() );
        Eigen::Vector3d& p2 = he->next()->vertex()->point();
        ::glVertex3d( p2.x(), p2.y(), p2.z() );
        ::glEnd();
      }
    }

#if 0
    // draw Last Point
    ::glColor3f( 1.0f, .0f, .0f );
    ::glPointSize( 5.0f );
    std::list<VertexL*>::iterator vi = mesh().vertices().end(); --vi;
    Eigen::Vector3d& p = (*vi)->point();
    ::glBegin( GL_POINTS );
    ::glVertex3d( p.x(), p.y(), p.z() );
    ::glEnd();
#endif

  };

  void drawPoint() {
    if ( empty() ) return;

    if ( isSmoothShading_ == false )
      {
        ::glPointSize( pointSize() );

        ::glBegin( GL_POINTS );
        // foreach ( std::list<VertexL*>, mesh().vertices(), vt )
        for ( auto vt : mesh().vertices() ) {
#if 1
          if ( vt->isSelected() == false ) {
            ::glColor3fv( pointColor() );
          } else {
            ::glColor3fv( selectedColor() );
          }
#endif
          Eigen::Vector3d& p = vt->point();
          ::glVertex3d( p.x(), p.y(), p.z() );
        }
        ::glEnd();

      }
    else
      {
        ::glShadeModel( GL_SMOOTH );
        ::glEnable( GL_LIGHTING );
        mtl_point_.bind();

        ::glBegin( GL_POINTS );
        // foreach ( std::list<FaceL*>, mesh().faces(), fc ) {
        for ( auto fc : mesh().faces() ) {
          // foreach ( std::list<HalfedgeL*>,(*fc)->halfedges(), he ) {
          for ( auto he : fc->halfedges() ) {
            // foreach ( std::list<HalfedgeL*>, mesh().halfedges(), he )
            //   {
            Eigen::Vector3d& n = he->normal()->point();
            ::glNormal3d( n.x(), n.y(), n.z() );
            Eigen::Vector3d& p = he->vertex()->point();
            ::glVertex3d( p.x(), p.y(), p.z() );
          }
        }
        ::glEnd();

        ::glDisable( GL_LIGHTING );
      }

  };

  void drawWireframeBLoop() {
    if ( empty() ) return;
    if ( mesh().emptyBLoop() ) return;

    ::glColor3fv( boundaryColor() );
    ::glLineWidth( boundarySize() );

    BLoopL* bl = mesh().bloop();
    ::glBegin( GL_LINE_LOOP );
    // foreach ( std::vector<VertexL*>, bl->vertices(), vt ) {
    for ( auto vt : bl->vertices() ) {
      Eigen::Vector3d& p = vt->point();
      ::glVertex3d( p.x(), p.y(), p.z() );
    }
    ::glEnd();

    ::glColor3f( 1.0f, 1.0f, 1.0f );

  };

  void drawPointsSelect() {
    // foreach ( std::list<VertexL*>, mesh().vertices(), vt ) {
    for ( auto vt : mesh().vertices() ) {
      //cout << (*vt)->id() << endl;
      ::glLoadName( (GLuint ) vt->id() );
      ::glBegin( GL_POINTS );
      Eigen::Vector3d& p = vt->point();
      ::glVertex3d( p.x(), p.y(), p.z() );
      ::glEnd();
    }
  };

  void drawFacesSelect() {
    // foreach ( std::list<FaceL*>, mesh().faces(), fc ) {
    for ( auto fc : mesh().faces() ) {
      ::glLoadName( (GLuint) fc->id() );
      //        ::glBegin( GL_TRIANGLES );
      ::glBegin( GL_POLYGON );
      // foreach ( std::list<HalfedgeL*>, (*fc)->halfedges(), he ) {
      for ( auto he : fc->halfedges() ) {
        Eigen::Vector3d& p = he->vertex()->point();
        ::glVertex3d( p.x(), p.y(), p.z() );
      }
      ::glEnd();
    }
  };

private:

  MeshL* mesh_;

  bool isDrawWireframeBLoop_;

};

#endif // _GLMESHL_HXX
