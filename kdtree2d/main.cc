////////////////////////////////////////////////////////////////////////////////////
//
// $Id: main.cc 2024/08/10 14:12:14 kanai Exp $
//
// Copyright (c) 2024 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>

#include "envDep.h"
#include "mydef.h"
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLPanel.hxx"
GLPanel pane;

#include "KdTree.hxx"

// kD-Tree 2D のインスタンス
KdTree<2> kdtree;

// k-近傍探索の点群のインデックスを格納
std::vector<int> knn;
// 近傍点の数
static int n_neighbors = 10;

////////////////////////////////////////////////////////////////////////////////////

// keyboard
bool shift_key_pressed = false;
bool control_key_pressed = false;
// mouse
bool left_button_pressed = false;
bool right_button_pressed = false;

int width = 800;
int height = 800;

#include "GLKdTree.hxx"
GLKdTree<2> glkdtree(width, height);

bool isDisplayKdTree = false;

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

  // w
  else if ((key == GLFW_KEY_W) && (action == GLFW_PRESS)) {
    // if (isDisplayKdTree == false) isDisplayKdTree = true;
    // else isDisplayKdTree = false;
    isDisplayKdTree ^= 1;
    return;
  }

  // q
  else if ((key == GLFW_KEY_Q) && (action == GLFW_PRESS)) {
    glfwSetWindowShouldClose(window, GL_TRUE);
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

  // マウスをクリックした点 query からの k個の近傍点を探索
  if ((button == GLFW_MOUSE_BUTTON_1) && (action == GLFW_PRESS)) {
    Eigen::Vector2d q(xd, height-yd);
    knn.clear();
    knn = kdtree.knnSearch(q, n_neighbors);
  }

#if 0
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
#endif
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

int main( int argc, char* argv[] )
{
  // 乱数生成器のシードを設定します
  std::random_device rd;
  std::mt19937 gen(rd());

  // 一様分布の範囲を設定します
  std::uniform_real_distribution<double> get_rand_uni_real(0.0, 1.0);

  // 乱数を生成して点を生成
  int num_of_points = 10000;
  std::vector<Eigen::Vector2d> points;
  for (int i = 0; i < num_of_points; ++i) {
    double x = get_rand_uni_real(gen) * (double) width;
    double y = get_rand_uni_real(gen) * (double) height;
    Eigen::Vector2d p(x,y);
    points.push_back(p);
  }

  // kD-Tree の構築
  kdtree.construct(points);

  // ここからウインドウの初期化処理
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) return EXIT_FAILURE;

  GLFWwindow* window = glfwCreateWindow(width, height, "GLFW Window", NULL, NULL);
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

  pane.init( width, height );
  pane.initGL2d();
  pane.initGLEW();
  pane.setIsGradientBackground( false );

  glfwSwapInterval(0);
  // ここまでウインドウの初期化処理

  // kD-Tree 表示用クラスへの登録
  glkdtree.setKdTree( kdtree );

  // 描画ループ処理
  while (!glfwWindowShouldClose(window)) {
    pane.clear2d();
    pane.setView2d();

    // kD-Tree の表示
    // 点群の表示
    glkdtree.drawPoints2d();
    // kD-Tree 境界の表示
    if (isDisplayKdTree) glkdtree.drawTreeRanges2d();
    // k-近傍点の表示
    glkdtree.drawClosePointSets2d( knn );

    pane.finish2d();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // ウインドウ終了処理
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
