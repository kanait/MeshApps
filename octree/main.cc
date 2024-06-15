////////////////////////////////////////////////////////////////////
//
// $Id: main.cc 2023/07/02 23:35:41 kanai Exp $
//
// Copyright (c) 2022 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////

#include "envDep.h"
#include "mydef.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
using namespace std;

#include <GL/glew.h>
#if defined(_WINDOWS)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

#include "MeshL.hxx"
#include "SMFLIO.hxx"

MeshL mesh;
SMFLIO smflio;

#include "Octree.hxx"
#include "GLOctree.hxx"

Octree octree;
GLOctree gloctree;

// ray の始点
Eigen::Vector3d pos(-0.2,0,1.0);
// ray の方向
Eigen::Vector3d dir(0,0,-1.0);

// 最も近い点
Eigen::Vector3d np;
// 最も近い面のid
int nfid = -1;

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

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " in.obj" << std::endl;
    return EXIT_FAILURE;
  }

  // mesh の読み込み
  smflio.setMesh(mesh);
  if (smflio.inputFromFile(argv[1]) == false) {
    return EXIT_FAILURE;
  }

  //
  // 表示用設定 （ここから先は特に触らなくても良い）
  //

  // GLGW initialization
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

  glmeshl.setMesh(mesh);

  glfwSwapInterval(0);

  // GLFW rendering process
  while (!glfwWindowShouldClose(window)) {
    pane.clear(width, height);
    pane.setView();
    pane.setLight();

    glmeshl.draw();
    gloctree.drawOctree( &octree );

    // intersection point
    ::glPointSize( 5.0f );
    ::glColor3f( 1.0f, .0f, .0f );
    glBegin(GL_POINTS);
    glVertex3d( np.x(), np.y(), np.z() );
    glEnd();

    // ray
    ::glColor3f( .0f, .0f, .0f );
    glBegin( GL_LINES );
    glVertex3d( pos.x(), pos.y(), pos.z() );
    glVertex3d( pos.x()+2.0*dir.x(), pos.y()+2.0*dir.y(), pos.z()+2.0*dir.z() );
    glEnd();
    
    pane.finish();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
