////////////////////////////////////////////////////////////////////////////////////
//
// $Id: GLKdTree.hxx 2024/06/22 23:25:43 kanai Exp $
//
// Copyright (c) 2024 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////

#ifndef _GLKDTREE_HXX
#define _GLKDTREE_HXX 1

#include <iostream>
using namespace std;

#include "KdTree.hxx"

template<int N>
class GLKdTree {

public:

  GLKdTree( int width, int height ) : kdtree_(NULL) {
    width_ = (float) width;
    height_ = (float) height;
  };
  ~GLKdTree() {};

  void setKdTree( KdTree<N>& kdtree ) { kdtree_ = &kdtree; };
  KdTree<N>& kdtree() { return *kdtree_; };

  bool empty() const { return ( kdtree_ != NULL ) ? false : true; };

  void draw2d() {
    if ( empty() ) return;

    drawTreeRanges2d();
    drawPoints2d();
  };

  void drawPoints2d() {
    if ( empty() ) return;

    ::glPointSize( 2.0f );
    ::glColor3f( .0f, .0f, .0f );

    std::vector<Eigen::Vector<double,N> >& points = kdtree().points();
    ::glBegin( GL_POINTS );
    for ( int i = 0; i < points.size() ; ++i ) {
      Eigen::Vector<double,N>& p = points[i];
      ::glVertex2f( p[0], p[1] );
    }
    ::glEnd();
  };

  void drawClosePointSets2d(std::vector<int>& idx ) {

    if ( empty() ) return;

    ::glPointSize( 3.0f );
    ::glColor3f( 1.0f, .0f, .0f );

    ::glBegin( GL_POINTS );
    for ( int i = 0; i < idx.size() ; ++i ) {
      Eigen::Vector<double,N>& p = kdtree().points()[idx[i]];
      ::glVertex2f( p[0], p[1] );
    }
    ::glEnd();
  };

  void drawTreeRanges2d() {

    if ( empty() ) return;

    ::glLineWidth( 1.0f );
    ::glColor3f( .0f, .0f, 1.0f );

    std::vector<Eigen::Vector<double,N> >& points = kdtree().points();

    float bbMin[2], bbMax[2];
    bbMin[0] = .0f;
    bbMin[1] = .0f;
    bbMax[0] = width_;
    bbMax[1] = height_;

    drawTreeRange2d( points, kdtree().root(), 0, points.size()-1, 0, bbMin, bbMax );
  };

  void drawTreeRange2d( std::vector<Eigen::Vector<double,N> >& points, // myPoint* p,
                        KdNode* node,
                        int left,
                        int right,
                        int splitDimension,
                        float bbMin[],
                        float bbMax[] ) {

    if ( node == nullptr ) return;

    // Calculate the index of this node:
    int mid=(left+right)>>1;

    // Calculate this node's bounding box:
    Eigen::Vector<double,N>& p = points[node->idx()];

    ::glBegin( GL_LINES );
    if ( splitDimension == 1 ) { // x
      ::glVertex2f( bbMin[0], p[1] );
      ::glVertex2f( bbMax[0], p[1] );
    } else {
      ::glVertex2f( p[0], bbMin[1] );
      ::glVertex2f( p[0], bbMax[1] );
    }
    ::glEnd();

    int childSplitDimension = (splitDimension + 1) % N;

    float childMin[2], childMax[2];
    if ( left < mid ) {
      if ( childSplitDimension == 0 ) {
        childMin[0] = bbMin[0];
        childMax[0] = bbMax[0];
        childMin[1] = bbMin[1];
        childMax[1] = p[1];
      } else {
        childMin[0] = bbMin[0];
        childMax[0] = p[0];
        childMin[1] = bbMin[1];
        childMax[1] = bbMax[1];
      }
      drawTreeRange2d( points, node->child(0), left, mid-1, childSplitDimension, childMin, childMax );
    }

    if ( right > mid ) {
      if ( childSplitDimension == 0 ) {
        childMin[0] = bbMin[0];
        childMax[0] = bbMax[0];
        childMin[1] = p[1];
        childMax[1] = bbMax[1];
      } else {
        childMin[0] = p[0];
        childMax[0] = bbMax[0];
        childMin[1] = bbMin[1];
        childMax[1] = bbMax[1];
      }
      drawTreeRange2d( points, node->child(1), mid+1, right, childSplitDimension, childMin, childMax );
    }

  };

private:

  KdTree<N>* kdtree_;
  float width_;
  float height_;

};

#endif // _GLKDTREE_HXX
