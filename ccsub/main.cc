////////////////////////////////////////////////////////////////////
//
// $Id: main.cc 2022/06/05 23:47:56 kanai Exp $
//
// Copyright (c) 2022 by Takashi Kanai. All rights reserved.
//
////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "envDep.h"
#include "mydef.h"
using namespace std;

#include <GL/glew.h>
#if defined(_WINDOWS)
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

#include "CCSubL.hxx"
#include "MeshL.hxx"
#include "SMFLIO.hxx"

std::vector<MeshL*> mesh;
int mno = 0;
SMFLIO smflio;

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

  // m
  else if ((key == GLFW_KEY_M) && (action == GLFW_PRESS)) {
    mno++;
    if (mno == mesh.size()) {
      MeshL* mesh1 = new MeshL;
      mesh.push_back( mesh1 );
      CCSubL ccsub( *(mesh[mno-1]), *(mesh[mno]) );
      ccsub.apply();
    }
    glmeshl.setMesh( *(mesh[mno]) );
    return;
  }
  // n
  else if ((key == GLFW_KEY_N) && (action == GLFW_PRESS)) {
    if (mno == 0) return;
    mno--;
    glmeshl.setMesh( *(mesh[mno]) );
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

  MeshL* mesh0 = new MeshL;
  smflio.setMesh(*mesh0);
  if (smflio.inputFromFile(argv[1]) == false) {
     return EXIT_FAILURE;
  }
  mesh.push_back(mesh0);

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

  glmeshl.setMesh(*(mesh[0]));

  glfwSwapInterval(0);

  // GLFW rendering process
  while (!glfwWindowShouldClose(window)) {
    pane.clear(width, height);
    pane.setView();
    pane.setLight();

    glmeshl.draw();

    pane.finish();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
