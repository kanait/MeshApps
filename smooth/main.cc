////////////////////////////////////////////////////////////////////
//
// $Id: main.cc 2024/06/17 17:36:31 kanai Exp $
//
// Copyright (c) 2024 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "envDep.h"
#include "mydef.h"
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "MeshL.hxx"
#include "SMFLIO.hxx"

MeshL mesh; // メッシュ
SMFLIO smflio; // メッシュのIO

#include "GLMeshL.hxx"
#include "GLPanel.hxx"

GLPanel pane;
GLMeshL glmeshl;

////////////////////////////////////////////////////////////////////////////////////

// keyboard
bool shift_key_pressed = false;
bool control_key_pressed = false;
// mouse
bool left_button_pressed = false;
bool right_button_pressed = false;

int width = 800;
int height = 800;

////////////////////////////////////////////////////////////////////////////////////

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

// キーボードイベント処理関数
static void key_callback(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  // ESC
  if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    return;
  }

  // q
  else if ((key == GLFW_KEY_Q) && (action == GLFW_PRESS)) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    return;
  }

  // w
  else if ((key == GLFW_KEY_W) && (action == GLFW_PRESS)) {
    if (glmeshl.isDrawWireframe() == false) {
      glmeshl.setIsDrawWireframe(true);
    } else {
      glmeshl.setIsDrawWireframe(false);
    }
    return;
  }

  // s
  else if ((key == GLFW_KEY_S) && (action == GLFW_PRESS)) {
    if (glmeshl.isSmoothShading() == false) {
      glmeshl.setIsSmoothShading(true);
    } else {
      glmeshl.setIsSmoothShading(false);
    }
    return;
  }

  // shift
  else if ((key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_PRESS)) {
    shift_key_pressed = true;
    return;
  } else if ((key == GLFW_KEY_LEFT_SHIFT) && (action == GLFW_RELEASE)) {
    shift_key_pressed = false;
    return;
  }

  // control
  else if ((key == GLFW_KEY_LEFT_CONTROL) && (action == GLFW_PRESS)) {
    control_key_pressed = true;
    return;
  } else if ((key == GLFW_KEY_LEFT_CONTROL) && (action == GLFW_RELEASE)) {
    control_key_pressed = false;
    return;
  }
}

// マウスイベント処理関数
static void mousebutton_callback(GLFWwindow* window, int button, int action,
                                 int mods) {
  double xd, yd;
  glfwGetCursorPos(window, &xd, &yd);
  pane.setScreenXY((int)xd, (int)yd);

  if ((button == GLFW_MOUSE_BUTTON_1) && (action == GLFW_PRESS)) {
    left_button_pressed = true;
    pane.startRotate();
    pane.startZoom();
    pane.startMove();
  } else if ((button == GLFW_MOUSE_BUTTON_1) && (action == GLFW_RELEASE)) {
    left_button_pressed = false;
    pane.finishRMZ();
  } else if ((button == GLFW_MOUSE_BUTTON_2) && (action == GLFW_PRESS)) {
    right_button_pressed = true;
  } else if ((button == GLFW_MOUSE_BUTTON_2) && (action == GLFW_RELEASE)) {
    right_button_pressed = false;
  }
}

// カーソルイベント処理関数
static void cursorpos_callback(GLFWwindow* window, double xd, double yd) {
  int x = (int)xd;
  int y = (int)yd;

  if (left_button_pressed && !shift_key_pressed && !control_key_pressed) {
    pane.updateRotate(x, y);
  } else if (left_button_pressed && shift_key_pressed && !control_key_pressed) {
    pane.updateZoom(x, y);
  } else if (left_button_pressed && !shift_key_pressed && control_key_pressed) {
    pane.updateMove(x, y);
  }
}

// mouse wheel
static void scroll_callback(GLFWwindow* window, double xoffset,
                            double yoffset) {
  pane.updateWheelZoom(yoffset);
}

// window resize
static void windowsize_callback(GLFWwindow* window, int w, int h) {
  width = w;
  height = h;
  pane.changeSize(w, h);
}

////////////////////////////////////////////////////////////////////////////////////

// 角度（ラジアン）
#define DEG45 0.785398     // M_PI/4.0
#define DEG30 0.523599     // M_PI/6.0
#define DEG20 0.349066     // M_PI/9.0
#define DEG10 0.174533     // M_PI/18.0

// crease の判定
bool isCrease( HalfedgeL* he ) {
  if (he->mate() == NULL) return false;
  Eigen::Vector3d& lnm = he->face()->normal();
  Eigen::Vector3d& rnm = he->mate()->face()->normal();
  if (V3AngleBetweenVectors( lnm, rnm ) > DEG30) return true;
  return false;
}

void calcSmoothVertexNormalWithCrease( MeshL& mesh ) {

  // ハーフエッジデータ構造の作成
  mesh.createConnectivity(true);

  std::list<VertexL*>& vertices = mesh.vertices();
  std::list<NormalL*>& normals = mesh.normals();

  // エッジが crease のときそのエッジを頂点からリンクする
  for ( auto vt : vertices ) {

    // 頂点 vt を持つハーフエッジを辿る
    VertexLCirculator vc(vt);
    HalfedgeL* vh = vc.beginHalfedgeL();
    do {
      if ( (vh->mate() != NULL) && (isCrease(vh) == true) ) {
        // 頂点からハーフエッジのリンクの付け替え
        vc.setfirstHalfedge(vh);
        break;
      }
      vh = vc.prevHalfedgeL();

    } while ((vh != vc.firstHalfedgeL()) && (vh != NULL));
  }

  // NormalL（頂点法線）のインスタンスの作成
  // Crease がある場合，新しい Normal を作成する
  ////////////////////// ここから //////////////////////////


  /////////////////// ここまでを埋める //////////////////////
}

////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " in.obj" << std::endl;
    return EXIT_FAILURE;
  }

  // メッシュデータの読み込み
  smflio.setMesh(mesh);
  if (smflio.inputFromFile(argv[1]) == false) {
    return EXIT_FAILURE;
  }

  // Smooth Shading 用法線ベクトルの生成
  calcSmoothVertexNormalWithCrease(mesh);

  // ここからウインドウの初期化処理
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) return EXIT_FAILURE;

  GLFWwindow* window =
    glfwCreateWindow(width, height, "GLFW Window", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mousebutton_callback);
  glfwSetCursorPosCallback(window, cursorpos_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowSizeCallback(window, windowsize_callback);

  pane.init(width, height);
  pane.initGL();
  pane.initGLEW();

  glfwSwapInterval(0);
  // ここまでウインドウの初期化処理

  // メッシュ表示用 に mesh をセット
  glmeshl.setMesh(mesh);

  // 描画ループ処理
  while (!glfwWindowShouldClose(window)) {
    pane.clear(width, height);
    pane.setView();
    pane.setLight();

    glmeshl.draw();

    pane.finish();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // ウインドウ終了処理
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
