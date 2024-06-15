#ifndef _GLOCTREE_HXX
#define _GLOCTREE_HXX 1

#include "Octree.hxx"

class GLOctree {

public:

  void drawOctree( Octree* node ) {

    if ( node == NULL ) return;

    // 最大レベルを超えたら描画しない
    if ( node->level() > MAX_LEVEL ) return;

    glLineWidth( 1.0f );
    glColor3f( 0.0f, 1.0f, 0.0f );

    Eigen::Vector3d& bbmin = node->getBBmin();
    Eigen::Vector3d& bbmax = node->getBBmax();

    glBegin( GL_LINE_LOOP );
    glVertex3d( bbmin.x(), bbmin.y(), bbmin.z() );
    glVertex3d( bbmax.x(), bbmin.y(), bbmin.z() );
    glVertex3d( bbmax.x(), bbmax.y(), bbmin.z() );
    glVertex3d( bbmin.x(), bbmax.y(), bbmin.z() );
    glEnd();

    glBegin( GL_LINE_LOOP );
    glVertex3d( bbmin.x(), bbmin.y(), bbmax.z() );
    glVertex3d( bbmax.x(), bbmin.y(), bbmax.z() );
    glVertex3d( bbmax.x(), bbmax.y(), bbmax.z() );
    glVertex3d( bbmin.x(), bbmax.y(), bbmax.z() );
    glEnd();

    glBegin( GL_LINES );
    glVertex3d( bbmin.x(), bbmin.y(), bbmin.z() );
    glVertex3d( bbmin.x(), bbmin.y(), bbmax.z() );
    glEnd();
  
    glBegin( GL_LINES );
    glVertex3d( bbmin.x(), bbmax.y(), bbmin.z() );
    glVertex3d( bbmin.x(), bbmax.y(), bbmax.z() );
    glEnd();
  
    glBegin( GL_LINES );
    glVertex3d( bbmax.x(), bbmax.y(), bbmin.z() );
    glVertex3d( bbmax.x(), bbmax.y(), bbmax.z() );
    glEnd();
  
    glBegin( GL_LINES );
    glVertex3d( bbmax.x(), bbmin.y(), bbmin.z() );
    glVertex3d( bbmax.x(), bbmin.y(), bbmax.z() );
    glEnd();
  
    for ( int i = 0; i < 8; ++i ) drawOctree( node->child(i) );
  };

};

#endif // _GLOCTREE_HXX
