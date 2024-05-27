////////////////////////////////////////////////////////////////////
//
// $Id: MeshL.hxx 2022/05/22 20:50:55 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _MESHL_HXX
#define _MESHL_HXX 1

#include "envDep.h"

#include <list>
#include <vector>
using namespace std;

#include "myEigen.hxx"

#include "VertexL.hxx"
#include "mydef.h"
#include "NormalL.hxx"
#include "TexcoordL.hxx"
#include "HalfedgeL.hxx"
#include "FaceL.hxx"
#include "EdgeL.hxx"
#include "LoopL.hxx"
#include "BLoopL.hxx"
#include "VertexLCirculator.hxx"
#include "MeshUtiL.hxx"

class MeshL {

 public:

  MeshL() { init(); };
  ~MeshL() { deleteAll(); };

  void init() {
    v_id_ = 0;
    f_id_ = 0;
    h_id_ = 0;
    t_id_ = 0;
    e_id_ = 0;
    n_id_ = 0;
    bl_id_ = 0;
    l_id_ = 0;
    n_vt_ = 0;
    isConnectivity_ = false;
    isNormalized_ = false;
    texID_ = 0;
  };

  // elements
  std::list<VertexL*>& vertices() { return vertices_; };
  int vertices_size() const { return n_vt_; };
  std::list<NormalL*>& normals() { return normals_; };
  std::list<TexcoordL*>& texcoords() { return texcoords_; };
  std::list<FaceL*>& faces() { return faces_; };
  int faces_size() const { return (int)faces_.size(); };
  std::list<EdgeL*>& edges() { return edges_; };

#if 1
  std::list<LoopL*>& loops() { return loops_; };
#endif
  std::list<BLoopL*>& bloops() { return bloops_; };
  //    Material& material() { return material_; };

  // vertex
  VertexL* vertex(int id) {
    for (auto vt : vertices_)
      if (vt->id() == id) return vt;
    return NULL;
  };

  VertexL* addVertex(Eigen::Vector3d& p) {
    VertexL* vt = new VertexL(v_id_++);
    ++n_vt_;
    vt->setPoint(p);
    vt->setIter(vertices_.insert(vertices_.end(), vt));
    return vt;
  };

  void deleteVertex(VertexL* vt) {
    vertices_.erase(vt->iter());
    --n_vt_;
    delete vt;
  };

  // normal
  NormalL* normal(int id) {
    for (auto nm : normals_)
      if (nm->id() == id) return nm;
    return NULL;
  };

  NormalL* addNormal(Eigen::Vector3d& p) {
    NormalL* nm = new NormalL(n_id_++);
    nm->setPoint(p);
    nm->setIter(normals_.insert(normals_.end(), nm));
    return nm;
  };

  void deleteNormal(NormalL* nm) {
    normals_.erase(nm->iter());
    delete nm;
  }

  // texcoord
  TexcoordL* texcoord(int id) {
    for (auto tc : texcoords_)
      if (tc->id() == id) return tc;
    return NULL;
  };

  TexcoordL* addTexcoord(Eigen::Vector3d& p) {
    TexcoordL* tc = new TexcoordL(t_id_++);
    tc->setPoint(p);
    tc->setIter(texcoords_.insert(texcoords_.end(), tc));
    return tc;
  };

  void deleteTexcoord(TexcoordL* tc) {
    texcoords_.erase(tc->iter());
    delete tc;
  };

  // halfedge
  int halfedge_size() const { return h_id_; };

  HalfedgeL* createHalfedge() {
    HalfedgeL* he = new HalfedgeL(h_id_++);
    return he;
  };

  HalfedgeL* addHalfedge(FaceL* fc) {
    HalfedgeL* he = createHalfedge();
    fc->addHalfedge(he);
    return he;
  };

#if 0
  HalfedgeL* addHalfedge(FaceL* fc, VertexL* vt, NormalL* nm) {
    HalfedgeL* he = createHalfedge();
    fc->addHalfedge(he, vt, nm);
    return he;
  };
#endif

  HalfedgeL* addHalfedge(FaceL* fc, VertexL* vt,
                         NormalL* nm = NULL, TexcoordL* tc = NULL) {
    HalfedgeL* he = createHalfedge();
    fc->addHalfedge(he, vt, nm, tc);
    return he;
  };

  // face
  FaceL* face(int id) {
    for (auto fc : faces_)
      if (fc->id() == id) return fc;
    return NULL;
  };

  FaceL* addFace() {
    FaceL* fc = new FaceL(f_id_++);
    fc->setIter(faces_.insert(faces_.end(), fc));
    return fc;
  };

  void deleteFace(FaceL* fc) {
    if (fc == NULL) return;
    fc->deleteHalfedges();
    faces_.erase(fc->iter());
    delete fc;
  };

  // edge
  EdgeL* addEdge() {
    EdgeL* ed = new EdgeL(e_id_++);
    ed->setIter(edges_.insert(edges_.end(), ed));
    return ed;
  };

  void deleteEdge(EdgeL* ed) {
    edges_.erase(ed->iter());
    delete ed;
  };

  // loop
  LoopL* addLoop() {
    LoopL* lp = new LoopL(l_id_++);
    lp->setIter(loops_.insert(loops_.end(), lp));
    return lp;
  };
  // void deleteLoop( LoopL* );
  void deleteLoop(LoopL* lp) {
    loops_.erase(lp->iter());
    delete lp;
  };

  // boundary loop
  BLoopL* addBLoop() {
    BLoopL* blp = new BLoopL(bl_id_++);
    blp->setBIter(bloops_.insert(bloops_.end(), blp));
    return blp;
  };

  void deleteBLoop(BLoopL* blp) {
    bloops_.erase(blp->biter());
    delete blp;
  };
  bool emptyBLoop() const { return (!(bloops_.size())) ? true : false; };
  BLoopL* bloop() { return *(bloops_.begin()); };

  // delete all
  void deleteAllVertices() {
    for (auto vt : vertices_) {
      delete vt;
      --n_vt_;
    }
    vertices_.clear();
  };

  void deleteAllNormals() {
    for (auto nm : normals_) { delete nm; }
    normals_.clear();
  };

  void deleteAllTexcoords() {
    for (auto tc : texcoords_) { delete tc; }
    texcoords_.clear();
  };

  void deleteAllFaces() {
    for (auto fc : faces_) { delete fc; }
    faces_.clear();
  };

  void deleteAllEdges() {
    for (auto ed : edges_) {
      if (ed->lhe() != NULL) ed->lhe()->setEdge(NULL);
      if (ed->rhe() != NULL) ed->rhe()->setEdge(NULL);
      delete ed;
    }
    edges_.clear();
    e_id_ = 0;
  };

  void deleteAllLoops() {
    for (auto lp : loops_) { delete lp; }
    loops_.clear();
  };

  void deleteAllBLoops() {
    for (auto blp : bloops_) { delete blp; }
    bloops_.clear();
  };

  void deleteAll() {
    deleteAllBLoops();
    deleteAllLoops();
    deleteAllEdges();
    deleteAllFaces();
    deleteAllTexcoords();
    deleteAllNormals();
    deleteAllVertices();
  };

  // find
  HalfedgeL* findHalfedge(VertexL* sv, VertexL* ev) {
    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        if ((he->vertex() == sv) && (he->next()->vertex() == ev)) return he;
        if ((he->vertex() == ev) && (he->next()->vertex() == sv)) return he;
      }
    }
    return NULL;
  };

  VertexL* findSelectedVertex() {
    for (auto vt : vertices_) {
      if (vt->isSelected() == true) return vt;
    }
    return NULL;
  };

  VertexL* findBoundaryVertex() {
    for (auto vt : vertices_) {
      if (isBoundary(vt) == true) return vt;
    }
    return NULL;
  };

  // center, max length for normalization
  void setCenter(Eigen::Vector3d& cen) { center_ = cen; };
  Eigen::Vector3d& center() { return center_; };
  void setMaxLength(double maxlen) { max_length_ = maxlen; };
  double maxLength() const { return max_length_; };
  bool isNormalized() const { return isNormalized_; };
  void setIsNormalized(bool f) { isNormalized_ = f; };

  // create
  void createConnectivity(bool isDeleteEdges=true) {
    // already defined
    if (isConnectivity()) {
      if (!(edges_.empty())) deleteAllEdges();
      deleteConnectivity();
    }

    std::vector<EdgeList> edge_list(vertices_size());
    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        VertexL* sv = he->vertex();
        VertexL* ev = he->next()->vertex();

        // set a halfedge to sv
        sv->setHalfedge(he);

        // find an edge
        EdgeL* ed = NULL;
        if ((ed = edge_list[sv->id()].findEdge(sv, ev)) == NULL) {
          // new edge
          ed = addEdge();
          ed->setSVertex(sv);
          ed->setEVertex(ev);
          ed->setLHalfedge(he);
          edge_list[ed->sv()->id()].push_back(ed);
          edge_list[ed->ev()->id()].push_back(ed);
        } else {
          if (ed->rhe() != NULL) {
            std::cerr << "Warning: More than three halfedges. Edge No."
                      << ed->id() << std::endl;
            std::cerr << "fc " << fc->id() << " he " << he->id()
                      << std::endl;
          }

          // assign right halfedge
          HalfedgeL* lhe = ed->lhe();
          if (lhe->mate_valid(he)) {
            lhe->setMate(he);
            he->setMate(lhe);
          } else {
            std::cerr << "Warning: invalid halfedge pair. Edge No." << ed->id()
                      << std::endl;
          }

          if (ed->rhe_valid(he)) {
            ed->setRHalfedge(he);
          }
        }
      }
    }

    // move vt's halfedge to the end. (efficient for boundary vertex)
    for (auto vt : vertices_) {
      HalfedgeL* he = vt->halfedge();
      if (he != NULL) vt->setHalfedge(he->reset());
    }

    if (isDeleteEdges) deleteAllEdges();

    setConnectivity(true);
  };

  void deleteConnectivity() {
    for (auto vt : vertices_) {
      vt->setHalfedge(NULL);
    }

    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        he->setMate(NULL);
      }
    }

    setConnectivity(false);
  };

  void checkConnectivity() {
    // face connectivity
    deleteIsolateFaces();
    // vertex in face check
    deleteIsolateVertices();
  };

  // delete isoleted faces
  void deleteIsolateFaces() {
    unsigned int i;
    std::vector<FaceL*> fcount;
    for (auto fc : faces_) {
      int count = 0;
      for (auto he : fc->halfedges()) {
        if (he->isBoundary()) count++;
      }
      if (count >= 2) {
        std::cout << "Warning: Face No." << fc->id()
                  << " is isolated. deleted..." << std::endl;
        fcount.push_back(fc);
      }
    }
    for (i = 0; i < fcount.size(); ++i) deleteFace(fcount[i]);
  };

  // delete unused vertices
  void deleteIsolateVertices() {
    std::vector<int> vcount;
    vcount.resize(vertices_size());

    unsigned int v_size = vertices_size();
    unsigned int i;
    for (i = 0; i < v_size; ++i) vcount[i] = 0;

    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        vcount[he->vertex()->id()]++;
      }
    }

    bool isRecalculateBLoop = false;
    for (i = 0; i < v_size; ++i)
      if (!(vcount[i])) {
        std::cout << "Warning: Vertex No." << i
                  << " was not used in all faces. deleted..." << std::endl;
        VertexL* vt = vertex(i);

        if (!emptyBLoop()) {
          if (bloop()->isVertex(vt) == true) isRecalculateBLoop = true;
        }
        deleteVertex(vt);
      }

    if (isRecalculateBLoop) createBLoop(bloop()->vertex(0));
  };

  bool isConnectivity() const { return isConnectivity_; };
  void setConnectivity(bool f) { isConnectivity_ = f; };

  //
  // 折れ線を考慮していないスムースシェーディング用
  // (ハーフエッジを使わない例)
  //
  void calcSmoothVertexNormal() {
    normals_.clear();

    // 面の数 保存用
    std::vector<int> n_vf;
    n_vf.resize(vertices_size());

    // 面積計算用
    std::vector<double> area(vertices_size());
    // normal 計算用
    std::vector<Eigen::Vector3d> nmvec(vertices_size());

    // 初期化
    // id の付け直し
    for (int i = 0; i < vertices_size(); ++i) {
      n_vf[i] = 0;
      nmvec[i] = Eigen::Vector3d::Zero();
    }

    for (auto fc : faces_) {
      double a = fc->area();
      for (auto he : fc->halfedges()) {
        n_vf[he->vertex()->id()]++;
        area[he->vertex()->id()] += a;
        Eigen::Vector3d nrm = fc->normal();
        nrm *= a;
        nmvec[he->vertex()->id()] += nrm;
      }
    }

    std::vector<NormalL*> nm_array;
    nm_array.resize(vertices_size());
    for (int i = 0; i < vertices_size(); ++i) {
      double f = (double)n_vf[i];
      nmvec[i] /= (f * area[i]);
      nmvec[i].normalize();
      NormalL* nm = addNormal(nmvec[i]);
      nm_array[i] = nm;
    }

    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        he->setNormal(nm_array[he->vertex()->id()]);
      }
    }
  };

  void normalize(Eigen::Vector3d& center, double maxlen) {
    for (auto vt : vertices_) {
      Eigen::Vector3d p1 = vt->point() - center;
      p1 /= maxlen;
      vt->setPoint(p1);
    }
  };

  void normalize() {
    if (isNormalized()) return;

    std::cout << "normalize ... ";
    Eigen::Vector3d vmax, vmin;
    int i = 0;
    for (auto vt : vertices_) {
      Eigen::Vector3d& p = vt->point();
      if (i) {
        if (p.x() > vmax.x()) vmax.x() = p.x();
        if (p.x() < vmin.x()) vmin.x() = p.x();
        if (p.y() > vmax.y()) vmax.y() = p.y();
        if (p.y() < vmin.y()) vmin.y() = p.y();
        if (p.z() > vmax.z()) vmax.z() = p.z();
        if (p.z() < vmin.z()) vmin.z() = p.z();
      } else {
        vmax = p;
        vmin = p;
      }
      ++i;
    }

    center_ = (vmax + vmin) * .5;

    Eigen::Vector3d len(vmax - vmin);
    double maxl = (std::fabs(len.x()) > std::fabs(len.y())) ? std::fabs(len.x())
      : std::fabs(len.y());
    maxl = (maxl > std::fabs(len.z())) ? maxl : std::fabs(len.z());
    setMaxLength(maxl);

    normalize(center_, maxl);

    setIsNormalized(true);

    std::cout << "done." << std::endl;
  };

  // void unnormalize();
  void unnormalize() {
    if (!isNormalized()) return;

    std::cout << "unnormalize ... ";

    for (auto vt : vertices_) {
      Eigen::Vector3d p(vt->point());
      p *= maxLength();
      p += center_;
      vt->setPoint(p);
    }

    setIsNormalized(false);

    std::cout << "done." << std::endl;
  };

  // void normalizeTexcoord();
  void normalizeTexcoord() {
    Eigen::Vector2d vmax, vmin;
    int i = 0;
    for (auto tc : texcoords_) {
      Eigen::Vector3d& p0 = tc->point();
      Eigen::Vector2d p(p0.x(), p0.y());
      if (i) {
        if (p.x() > vmax.x()) vmax.x() = p.x();
        if (p.x() < vmin.x()) vmin.x() = p.x();
        if (p.y() > vmax.y()) vmax.y() = p.y();
        if (p.y() < vmin.y()) vmin.y() = p.y();
      } else {
        vmax = p;
        vmin = p;
      }
      ++i;
    }

    double xlen = vmax.x() - vmin.x();
    double ylen = vmax.y() - vmin.y();

    for (auto tc : texcoords_) {
      Eigen::Vector3d& p = tc->point();
      Eigen::Vector3d q((p.x() - vmin.x()) / xlen, (p.y() - vmin.y()) / ylen, .0);
      tc->setPoint(q);
    }
  };

  void calcAllFaceNormals() {
    for (auto fc : faces_)
      fc->calcNormal();
  };

  void createBLoop() {
    // needs connectivity
    createConnectivity(true);
    VertexL* vt = findBoundaryVertex();
    if (vt == NULL) return;
    createBLoop(vt);
  };

  //void createBLoop(VertexL*);
  //
  // create boundary loop
  // sv: starting vertex
  void createBLoop(VertexL* sv) {
    // needs connectivity
    createConnectivity(true);

    if (isBoundary(sv) == false) return;

    if (!(emptyBLoop())) deleteAllBLoops();

    BLoopL* bl = addBLoop();

    VertexL* vt = sv;    // current vertex
    VertexL* pv = NULL;  // previous vertex
    do {
      std::cout << *vt << std::endl;

      bl->addVertex(vt);
      bl->addIsCorner(false);  // default

      // next boundary vertex
      VertexLCirculator vc(vt);
      VertexL* vv = vc.beginVertexL();
      do {
        vv = vc.nextVertexL();
        if ((isBoundary(vv)) && (vv != pv)) break;
      } while ((vv != vc.firstVertexL()) && (vv != NULL));

      pv = vt;
      vt = vv;

    } while (vt != sv);

    // set first vertex to corner.
    bl->setCorner(0, true);

    // optimize boundary corner
    bl->optimize(4);

    std::cout << "BLoop created." << std::endl;
  };

  bool isVerticesSelected() {
    for (auto vt : vertices_)
      if (vt->isSelected()) return true;
    return false;
  };

  bool isFacesSelected() {
    for (auto fc : faces_)
      if (fc->isSelected()) return true;
    return false;
  };

  // select
  void clearAllVerticesSelected() {
    for (auto vt : vertices_) vt->setSelected(false);
  };

  void setAllVerticesSelected() {
    for (auto vt : vertices_) vt->setSelected(true);
  };

  void clearAllFacesSelected() {
    for (auto fc : faces_) fc->setSelected(false);
  };

  void setAllFacesSelected() {
    for (auto fc : faces_) fc->setSelected(true);
  };

  //
  // create a new mesh with reordering vertices according to an array "order"
  //
  void reorderVertices(std::vector<int>& order) {
    int n_vt = vertices_.size();

    // store old vertices
    std::vector<VertexL*> old_vt;
    int id = 0;
    for (auto vt : vertices_) {
      old_vt.push_back(vt);
      vt->setID(id++);
    }

    // create vertices according to the order array and add vertex lists
    std::vector<VertexL*> new_vt;
    id = 0;
    std::vector<int> new_id(n_vt);
    for (int i = 0; i < n_vt; ++i) {
      VertexL* nvt = addVertex(old_vt[order[i]]->point());
      new_id[order[i]] = id;
      nvt->setID(id++);
      new_vt.push_back(nvt);
    }

    // reconnect the halfedge's vertices
    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        he->setVertex(new_vt[new_id[he->vertex()->id()]]);
      }
    }

    // delete former vertices
    auto vt_iter = vertices_.begin();
    for (int i = 0; i < n_vt; ++i) {
      auto nvt = vt_iter;
      ++vt_iter;
      deleteVertex(*nvt);
    }
  };

  //
  // reordering new faces according to an array "indices"
  //
  void reorderIndices(unsigned int n_indices, unsigned int* indices) {
    int n_fc = faces_.size();

    // store vertex pointers
    std::vector<VertexL*> old_vt;
    int id = 0;
    for (auto vt : vertices_) {
      old_vt.push_back(vt);
      vt->setID(id++);
    }

    // create new faces
    id = 0;
    unsigned int i = 0;
    while (i < n_indices) {
      FaceL* fc = addFace();
      fc->setID(id++);

      VertexL* vt;
      // HalfedgeL* he;
      vt = old_vt[indices[i]];
      ++i;
      (void)addHalfedge(fc, vt, NULL, NULL);
      vt = old_vt[indices[i]];
      ++i;
      (void)addHalfedge(fc, vt, NULL, NULL);
      vt = old_vt[indices[i]];
      ++i;
      (void)addHalfedge(fc, vt, NULL, NULL);
    }

    // delete former faces
    auto fc_iter = faces_.begin();
    for (int i = 0; i < n_fc; ++i) {
      auto nfc = fc_iter;
      ++fc_iter;
      deleteFace(*nfc);
    }
  };

  unsigned int texID() const { return texID_; };
  //
  // set Texture id to all selected faces
  //
  // selected:
  //   - true: set only selected faces
  //   - false: set all faces
  //
  void setTexIDToFaces(int id, bool selected) {
    for (auto fc : faces_) {
      if (selected) {
        if (fc->isSelected()) fc->setTexID(id);
      } else
        fc->setTexID(id);
    }
  };

  void changeTexID(int id0, int id1) {
    texID_ = id1;
    for (auto fc : faces_) {
      if (fc->texID() == id0) fc->setTexID(id1);
    }
  };

  void copyTexcoordToVertex(std::vector<Eigen::Vector3d>& p) {
    auto tc = texcoords_.begin();
    int i = 0;
    for (auto vt : vertices_) {
      p[i] = vt->point();
      vt->setPoint((Eigen::Vector3d&)(*tc)->point());
      tc++;
      ++i;
    }
  };

  void copyVertex(std::vector<Eigen::Vector3d>& p) {
    int i = 0;
    for (auto vt : vertices_) {
      vt->setPoint(p[i]);
      ++i;
    }
  };

  void resetVertexID() {
    int i = 0;
    for (auto vt : vertices_) {
      vt->setID(i);
      ++i;
    }
  };

  void resetHalfedgeID() {
    int i = 0;
    for (auto fc : faces_) {
      for (auto he : fc->halfedges()) {
        he->setID(i);
        ++i;
      }
    }
    assert(halfedge_size() == i);
  };

  void resetFaceID() {
    int i = 0;
    for (auto fc : faces_) {
      fc->setID(i);
      ++i;
    }
  };

  void print() {
    for (auto fc : faces_) fc->print();
  };

  void printInfo() {
    std::cout << "mesh "
              << " ";
    if (vertices_.size())
      std::cout << " v " << (unsigned int)vertices().size() << " ";
    if (normals_.size())
      std::cout << " n " << (unsigned int)normals().size() << " ";
    if (texcoords_.size())
      std::cout << " t " << (unsigned int)texcoords().size() << " ";
    if (faces_.size()) std::cout << " f " << (unsigned int)faces_.size() << " ";
    if (bloops_.size())
      std::cout << " bl " << (unsigned int)bloops_.size() << " ";
    std::cout << std::endl;
  };

private:

  // vertices
  int v_id_;
  int n_vt_;
  std::list<VertexL*> vertices_;

  // normals (for smooth shading)
  int n_id_;
  std::list<NormalL*> normals_;

  // texcoords
  int t_id_;
  std::list<TexcoordL*> texcoords_;

  // halfedges
  int h_id_;

  // faces
  int f_id_;
  std::list<FaceL*> faces_;

  // edges (define if needed)
  int e_id_;
  std::list<EdgeL*> edges_;

  // loops (define if needed)
  int l_id_;
  std::list<LoopL*> loops_;

  // boundary loops (define if needed)
  int bl_id_;
  std::list<BLoopL*> bloops_;

  bool isConnectivity_;

  bool isNormalized_;
  Eigen::Vector3d center_;
  double max_length_;

  // texture
  unsigned int texID_;

};

#endif  // _MESHL_HXX
