////////////////////////////////////////////////////////////////////
//
// $Id: Octree.hxx 2024/08/10 14:13:36 kanai Exp $
//
// Copyright (c) 2023 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////

#ifndef _OCTREE_HXX
#define _OCTREE_HXX 1

#include <vector>
#include <limits>
using namespace std;

#include "myEigen.hxx"
#include "FaceL.hxx"

#define  MAX_LEVEL 5

#include "tribox3.h"
#include "raytri.h"

// Octree のノードのクラス
class Octree {

public:

  Octree(){ init(); };
  Octree( Eigen::Vector3d& bbmin, Eigen::Vector3d& bbmax ) { init(); setBB( bbmin, bbmax ); };

  // parent_ と child_[8] ノードの初期化
  void init() {
    parent_ = NULL;
    for ( int i = 0; i < 8; ++i ) child_[i] = NULL;
  };

  // Bounding Box (bbmin, bbmax) への値のセット
  void setBB( Eigen::Vector3d& bbmin, Eigen::Vector3d& bbmax ) {
    bbmin_ = bbmin;
    bbmax_ = bbmax;
  };

  void setParent( Octree* parent ) { parent_ = parent; };
  void setChild( int id, Octree* child ) { child_[id] = child; };
  void setLevel( int l ) { level_ = l; };

  int level() const { return level_; };
  Eigen::Vector3d& getBBmin() { return bbmin_; };
  Eigen::Vector3d& getBBmax() { return bbmax_; };

  // ノードの面リストへの面データの追加
  void addFaceList( FaceL* fc ) {
    flist_.push_back( fc );
  };

  Octree* child( int id ) { return child_[id]; };

  // Child ノードの生成
  Octree* addChild( int id ) {
    // 範囲の計算
    Eigen::Vector3d bbmin, bbmax;
    calcChildRange( id, bbmin, bbmax );

    Octree* child = new Octree( bbmin, bbmax );
    child->setParent( this );
    child->setLevel( level_ + 1 );

    child_[id] = child;

    return child;
  };

  // child ノードの Bounding Box の計算
  void calcChildRange( int id, Eigen::Vector3d& cbbmin, Eigen::Vector3d& cbbmax ) {
    if( id < 4 ) {
      cbbmin.z() = bbmin_.z();
      cbbmax.z() = (bbmin_.z() + bbmax_.z()) / 2.0;

      switch( id ) {
      case 0:
        cbbmin.x() = bbmin_.x();
        cbbmin.y() = bbmin_.y();
        cbbmax.x() = (bbmax_.x() + bbmin_.x()) / 2.0;
        cbbmax.y() = (bbmax_.y() + bbmin_.y()) / 2.0;
        break;

      case 1:
        cbbmin.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmin.y() = bbmin_.y();
        cbbmax.x() = bbmax_.x();
        cbbmax.y() = (bbmin_.y() + bbmax_.y())/ 2.0;    
        break;

      case 2:
        cbbmin.x() = bbmin_.x();
        cbbmin.y() = (bbmin_.y() + bbmax_.y())/ 2.0;
        cbbmax.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmax.y() = bbmax_.y();
        break;

      case 3:
        cbbmin.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmin.y() = (bbmin_.y() + bbmax_.y())/ 2.0;
        cbbmax.x() = bbmax_.x();
        cbbmax.y() = bbmax_.y();
        break;
      }
    } else {
      cbbmin.z() = (bbmin_.z() + bbmax_.z()) / 2.0;
      cbbmax.z() = bbmax_.z();

      switch( id ) {
      case 4:
        cbbmin.x() = bbmin_.x();
        cbbmin.y() = bbmin_.y();
        cbbmax.x() = (bbmax_.x() + bbmin_.x()) / 2.0;
        cbbmax.y() = (bbmax_.y() + bbmin_.y()) / 2.0;
        break;

      case 5:
        cbbmin.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmin.y() = bbmin_.y();
        cbbmax.x() = bbmax_.x();
        cbbmax.y() = (bbmin_.y() + bbmax_.y())/ 2.0;    
        break;

      case 6:
        cbbmin.x() = bbmin_.x();
        cbbmin.y() = (bbmin_.y() + bbmax_.y())/ 2.0;
        cbbmax.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmax.y() = bbmax_.y();
        break;

      case 7:
        cbbmin.x() = (bbmin_.x() + bbmax_.x())/ 2.0;
        cbbmin.y() = (bbmin_.y() + bbmax_.y())/ 2.0;
        cbbmax.x() = bbmax_.x();
        cbbmax.y() = bbmax_.y();
        break;
      }
    }
  };

  //
  // node->child[i] の中に fc が入っているかどうか調べ
  // 入っていれば node->child[i] を再帰的に調べる．
  // あるレベルに到達したときに face list に加える．
  //
  void addFaceToOctree( FaceL* fc ) {
    if ( level_ == MAX_LEVEL ) {
      addFaceList( fc );
      return;
    }

    for ( int i = 0; i < 8; ++i ) {
      // 面 fc が 子供の範囲内に入っているかどうかをチェック
      Eigen::Vector3d bbmin, bbmax;
      calcChildRange( i, bbmin, bbmax );
      if ( isFaceOveralapBox( fc, bbmin, bbmax ) ) { // 入っている場合
        // child[i] がなければ作成
        if ( child_[i] == NULL ) child_[i] = addChild( i );
        child_[i]->addFaceToOctree( fc );
      }
    }
  };

  //
  // 面がボックスに少しでも入っているかどうかをチェック
  //
  bool isFaceOveralapBox( FaceL *face, Eigen::Vector3d& bbmin, Eigen::Vector3d& bbmax ) {

    float boxcenter[3], boxhalfsize[3];
    boxcenter[0] = (float) ( bbmax.x() + bbmin.x() ) / 2.0f;
    boxcenter[1] = (float) ( bbmax.y() + bbmin.y() ) / 2.0f;
    boxcenter[2] = (float) ( bbmax.z() + bbmin.z() ) / 2.0f;
    boxhalfsize[0] = (float) ( bbmax.x() - bbmin.x() ) / 2.0f;
    boxhalfsize[1] = (float) ( bbmax.y() - bbmin.y() ) / 2.0f;
    boxhalfsize[2] = (float) ( bbmax.z() - bbmin.z() ) / 2.0f;

    // bool isFaceInRange( Face *face, Point3d *bbmin, Point3d *bbmax ) {
    float triverts[3][3];
    int i = 0;
    for (auto he : face->halfedges()) {
      Eigen::Vector3d& p = he->vertex()->point();
      triverts[i][0] = (float) p.x();
      triverts[i][1] = (float) p.y();
      triverts[i][2] = (float) p.z();
      ++i;
    }

    return triBoxOverlap( boxcenter, boxhalfsize, triverts );
  };

  // 直線とボックスの交差判定
  // http://marupeke296.com/COL_3D_No18_LineAndAABB.html
  bool isRayIntersect( Eigen::Vector3d& pos, Eigen::Vector3d& dir ) {

    double t_max = std::numeric_limits<double>::max();   // AABB からレイが外に出る時刻
    double t_min = -std::numeric_limits<double>::max();  // AABB にレイが侵入する時刻

    for (int i=0; i<3; i++) {
      double t1, t2;
      if ( i==0 ) {
        t1 = (bbmin_.x() - pos.x())/dir.x();
        t2 = (bbmax_.x() - pos.x())/dir.x();
      } else if (i == 1) {
        t1 = (bbmin_.y() - pos.y())/dir.y();
        t2 = (bbmax_.y() - pos.y())/dir.y();
      } else if (i == 2) {
        t1 = (bbmin_.z() - pos.z())/dir.z();
        t2 = (bbmax_.z() - pos.z())/dir.z();
      }
      double t_near = std::min(t1, t2);
      double t_far = std::max(t1, t2);
      t_max = std::min(t_max, t_far);
      t_min = std::max(t_min, t_near);

      //
      // レイが外に出る時刻と侵入する時刻が逆転している => 交差していない
      //
      if (t_min > t_max) return false;
    }

    return true;
  };

  // ray と flist_ に入っている面との交差判定
  // 複数入っている場合は，pos に一番近い点を出力する
  FaceL* intersectRayFaces( Eigen::Vector3d& pos, Eigen::Vector3d& dir,
                            Eigen::Vector3d& near_p ) {

    FaceL* near_fc = NULL;
    for (int i = 0; i < flist_.size(); ++i ) {
      FaceL* fc = flist_[i];
      double orig[3], ddir[3], vert0[3], vert1[3], vert2[3];
      orig[0] = pos.x(); orig[1] = pos.y(); orig[2] = pos.z();
      ddir[0] = dir.x(); ddir[1] = dir.y(); ddir[2] = dir.z();
      int j = 0;
      Eigen::Vector3d p0, p1, p2;
      for ( auto he : fc->halfedges() ) {
        if (j == 0) {
          p0 = he->vertex()->point();
          vert0[0] = p0.x();
          vert0[1] = p0.y();
          vert0[2] = p0.z();
        } else if (j == 1) {
          p1 = he->vertex()->point();
          vert1[0] = p1.x();
          vert1[1] = p1.y();
          vert1[2] = p1.z();
        } else if (j == 2) {
          p2 = he->vertex()->point();
          vert2[0] = p2.x();
          vert2[1] = p2.y();
          vert2[2] = p2.z();
        }
        ++j;
      }
      double t,u,v;
      if ( intersect_triangle2(orig, ddir, vert0, vert1, vert2,
                               &t, &u, &v) ) {
        Eigen::Vector3d p = (1.0-u-v) * p0 + u * p1 + v * p2;
        if ( near_fc != NULL ) {
          if ( (pos - near_p).norm() > (pos - p).norm() ) {
            near_fc = fc;
            near_p = p;
          }
        } else {
          near_fc = fc;
          near_p = p;
        }
      }
    }

    return near_fc;
  };

private:

  int level_;
  Eigen::Vector3d bbmin_, bbmax_;
  Octree* parent_;
  Octree* child_[8];
  std::vector<FaceL*> flist_;
};

#endif // _OCTREE_HXX
