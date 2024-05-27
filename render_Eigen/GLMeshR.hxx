////////////////////////////////////////////////////////////////////
//
// $Id: GLMeshR.hxx 2021/06/13 15:10:44 kanai Exp $
//
//   OpenGL MeshR draw class
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _GLMESHR_HXX
#define _GLMESHR_HXX 1

#include "envDep.h"
#include "mydef.h"

#include "myGL.hxx"

#include "MeshR.hxx"
#include "GLMesh.hxx"

class GLMeshR : public GLMesh {

public:

  GLMeshR() : mesh_(NULL) {};
  ~GLMeshR() {};

  void clear() { if ( mesh_ ) deleteMesh(); };
  void setMesh( MeshR& mesh ) {
    mesh_ = &mesh;
    if ( !mesh.colors().empty() )
      {
        setIsDrawColor( true );
        setIsDrawShading( false );
      }
    else
      {
        setIsDrawColor( false );
        setIsDrawShading( true );
      }
  };
  void setMeshP( MeshR* mesh ) { mesh_ = mesh; };
  MeshR& mesh() { return *mesh_; };
  MeshR* meshP() { return mesh_; };

  void deleteMesh() { delete mesh_; mesh_ = NULL; };
  bool empty() const { 
    if ( mesh_ == NULL ) return true;
    return ( mesh_->numPoints() ) ? false : true; 
  };

  void setIsSmoothShading( bool f ) {
    GLMesh::setIsSmoothShading( f );
    if ( mesh_ == NULL ) return;
    if ( (f == false) && (mesh_->fnormals().empty()) )
      {
        mesh_->createFaceNormals();
      }
  };

  void draw() {
    if ( empty() ) return;

    ::glPushMatrix();
    if ( isDrawColor_ ) { drawColor(); };
    if ( isDrawShading_ ) { drawShading(); }
    if ( isDrawWireframe_ ) { drawWireframe(); };
    if ( isDrawPoint_ ) { drawPoints(); };
    ::glPopMatrix();
  };
  //void draw() { draw( mesh_ ); };

  void drawColor() {
    if ( empty() ) return;
    if ( mesh().colors().empty() ) return;

    ::glShadeModel( GL_FLAT );
    ::glDisable( GL_LIGHTING );

    std::vector<float>& points = mesh().points();
    std::vector<unsigned int>& indices = mesh().indices();
    std::vector<unsigned char>& colors = mesh().colors();

    ::glBegin( GL_TRIANGLES );
    unsigned int n_id = 0;
    for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
      {
        unsigned int id0 = indices[TRIANGLE * i];
        unsigned int id1 = indices[TRIANGLE * i + 1];
        unsigned int id2 = indices[TRIANGLE * i + 2];

        ::glColor3ubv( &(colors[ nXYZ * id0 ]) );
        ::glVertex3fv( &(points[ nXYZ * id0 ]) );

        ::glColor3ubv( &(colors[ nXYZ * id1 ]) );
        ::glVertex3fv( &(points[ nXYZ * id1 ]) );

        ::glColor3ubv( &(colors[ nXYZ * id2 ]) );
        ::glVertex3fv( &(points[ nXYZ * id2 ]) );
      }
    ::glEnd();
  };

  void drawShading() {
    if ( empty() ) return;

    if ( isSmoothShading_ )
      {
        ::glShadeModel( GL_SMOOTH );
        ::glEnable( GL_LIGHTING );

        // normals
        ::glNormalPointer( GL_FLOAT, 0, 
                           (GLfloat*) &(mesh().normals()[0]) );
        ::glEnableClientState( GL_NORMAL_ARRAY );

        if ( isDrawTexture_ )
          {
            ::glEnable( GL_TEXTURE_2D );
            ::glTexCoordPointer( mesh().n_tex(), GL_FLOAT, 0, 
                                 (GLfloat*) &(mesh().texcoords()[0]) );
            ::glEnableClientState( GL_TEXTURE_COORD_ARRAY );
          }

        // vertices
        ::glVertexPointer( 3, GL_FLOAT, 0, 
                           (GLfloat*) &(mesh().points()[0]) );
        ::glEnableClientState( GL_VERTEX_ARRAY );

        // material binding
        mtl_.bind();

        ::glDrawElements( GL_TRIANGLES, mesh().indices_size(), 
                          GL_UNSIGNED_INT, &(mesh().indices()[0]) );

        if ( isDrawTexture_ )
          {
            ::glDisableClientState( GL_TEXTURE_COORD_ARRAY );
            ::glDisable( GL_TEXTURE_2D );
          }
        ::glDisableClientState( GL_NORMAL_ARRAY );
        ::glDisableClientState( GL_VERTEX_ARRAY );

        ::glDisable( GL_LIGHTING );
 
      }
    else // isSmoothShading == false 
      {
        ::glShadeModel( GL_FLAT );
        ::glEnable( GL_LIGHTING );

        if ( isDrawTexture_ )
          {
            ::glEnable( GL_TEXTURE_2D );
            ::glBindTexture( GL_TEXTURE_2D, mesh().texID() );
          }
        else
          {
            // material binding
            mtl_.bind();
          }

        std::vector<float>& points = mesh().points();
        std::vector<float>& fnormals = mesh().fnormals();
        std::vector<float>& texcoords = mesh().texcoords();
        std::vector<unsigned int>& indices = mesh().indices();
        std::vector<unsigned int>& tindices = mesh().tindices();
        int n_tex = mesh().n_tex();

#if 1 // for large model
        ::glBegin( GL_TRIANGLES );
        for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
          {
            unsigned int id0 = indices[TRIANGLE * i];
            unsigned int id1 = indices[TRIANGLE * i + 1];
            unsigned int id2 = indices[TRIANGLE * i + 2];
            unsigned int tid0, tid1, tid2;
            if ( isDrawTexture_ )
              {
                tid0 = tindices[TRIANGLE * i];
                tid1 = tindices[TRIANGLE * i + 1];
                tid2 = tindices[TRIANGLE * i + 2];
              }
            //cout << "normal " << fnormals[nXYZ*i] << " " << fnormals[nXYZ*i+1] << " " << fnormals[nXYZ*i+2] << endl;
            ::glNormal3fv( &(fnormals[ nXYZ * i ]) );
            if ( isDrawTexture_ ) ::glTexCoord2fv( &(texcoords[ n_tex * tid0 ]) );
            ::glVertex3fv( &(points[ nXYZ * id0 ]) );
            if ( isDrawTexture_ ) ::glTexCoord2fv( &(texcoords[ n_tex * tid1 ]) );
            ::glVertex3fv( &(points[ nXYZ * id1 ]) );
            if ( isDrawTexture_ ) ::glTexCoord2fv( &(texcoords[ n_tex * tid2 ]) );
            ::glVertex3fv( &(points[ nXYZ * id2 ]) );
          }
        ::glEnd();
#endif
#if 0 // for fandisk
        ::glBegin( GL_TRIANGLES );
        unsigned int n_id = 0;
        for ( int i = 0; i < mesh().numFaces(); ++i )
          {
            unsigned int id0 = indices[TRIANGLE * i];
            unsigned int id1 = indices[TRIANGLE * i + 1];
            unsigned int id2 = indices[TRIANGLE * i + 2];
            ::glNormal3fv( &(normals[ nXYZ * n_id++ ]) );
            ::glVertex3fv( &(points[ nXYZ * id0 ]) );
            ::glNormal3fv( &(normals[ nXYZ * n_id++ ]) );
            ::glVertex3fv( &(points[ nXYZ * id1 ]) );
            ::glNormal3fv( &(normals[ nXYZ * n_id++ ]) );
            ::glVertex3fv( &(points[ nXYZ * id2 ]) );
          }
        ::glEnd();
#endif
        if ( isDrawTexture_ )
          {
            ::glBindTexture( GL_TEXTURE_2D, 0 );
            ::glDisable( GL_TEXTURE_2D );
          }
        ::glDisable( GL_LIGHTING );
      }
  };

  void drawWireframe() {
    if ( empty() ) return;

    ::glDisable( GL_LIGHTING );

    ::glColor3fv( wireColor() );
    ::glLineWidth( wireSize() );

    ::glEnableClientState( GL_VERTEX_ARRAY );
    ::glVertexPointer( 3, GL_FLOAT, 0, 
                       (GLfloat*) &(mesh().points()[0]) );

    for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
      {
        ::glDrawElements( GL_LINE_LOOP, 3, GL_UNSIGNED_INT, 
                          &(mesh().indices()[3*i]) );
      }

    ::glDisableClientState( GL_VERTEX_ARRAY );
  };

  void drawPoints() {
    if ( empty() ) return;

    ::glDisable( GL_LIGHTING );

    ::glPointSize( pointSize() );

    if ( mesh().isSelected() != SEL_VERTEX )
      {
        ::glColor3fv( pointColor() );

        ::glEnableClientState( GL_VERTEX_ARRAY );
        ::glVertexPointer( 3, GL_FLOAT, 0,
                           (GLfloat*) &(mesh().points()[0]) );

        ::glDrawArrays( GL_POINTS, 0, mesh().numPoints() );

        ::glDisableClientState( GL_VERTEX_ARRAY );
      }
    else
      {
        std::vector<float>& points = mesh().points();
        ::glBegin( GL_POINTS );
        for ( unsigned int i = 0; i < mesh().numPoints(); ++i )
          {
            if ( mesh().vertexSelected( i ) == false )
              ::glColor3fv( pointColor() );
            else
              ::glColor3fv( selectedColor() );

            ::glVertex3fv( &(points[ nXYZ * i ]) );
          }
        ::glEnd();
      }
  };
  //   void drawColor();

  void drawPointsSelect() {

    if ( empty() ) return;

    std::vector<float>& points = mesh().points();

    // draw points
    for ( unsigned int i = 0; i < mesh().numPoints(); ++i )
      {
        ::glLoadName( (GLuint ) i );
        ::glBegin( GL_POINTS );
        ::glVertex3fv( &(points[ nXYZ * i ]) );
        ::glEnd();
      }

  };

  void drawFacesSelect() {

    if ( empty() ) return;

    std::vector<float>& points = mesh().points();
    std::vector<float>& fnormals = mesh().fnormals();
    std::vector<unsigned int>& indices = mesh().indices();

    // draw faces
    unsigned int n_id = 0;
    for ( unsigned int i = 0; i < mesh().numFaces(); ++i )
      {
        ::glLoadName( (GLuint ) i );
        unsigned int id0 = indices[TRIANGLE * i];
        unsigned int id1 = indices[TRIANGLE * i + 1];
        unsigned int id2 = indices[TRIANGLE * i + 2];
        ::glBegin( GL_TRIANGLES );
        ::glNormal3fv( &(fnormals[ nXYZ * n_id++ ]) );
        ::glVertex3fv( &(points[ nXYZ * id0 ]) );
        ::glVertex3fv( &(points[ nXYZ * id1 ]) );
        ::glVertex3fv( &(points[ nXYZ * id2 ]) );
        ::glEnd();
      }
  };

private:

  MeshR* mesh_;

};

#endif // _GLMESHR_HXX
