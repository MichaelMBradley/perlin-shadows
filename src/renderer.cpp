#include "renderer.h"

#include <GL/freeglut_std.h>

#include <iostream>
#include <stdexcept>

#include "constants.h"

using namespace std;

Renderer *Renderer::window = nullptr;

Renderer::Renderer(int argc, char *argv[]) {
  if (window != nullptr) {
    throw runtime_error("Only one window may exist.");
  }
  window = this;
  CheckGLError();

  glutInit(&argc, argv);
  glutInitWindowPosition(10, 10);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("Ice Simulator");
  CheckGLError();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  CheckGLError();

  glutReshapeFunc(ReshapeCB);
  glutDisplayFunc(DisplayCB);
  glutKeyboardFunc(KeyboardCB);
  glutKeyboardUpFunc(KeyboardUpCB);
  glutMotionFunc(MotionCB);
  glutPassiveMotionFunc(PassiveMotionCB);
  TimerCB(0);
  CheckGLError();

  GLenum res = glewInit();
  if (res != GLEW_OK) {
    cerr << "Error in glewInit()";
    throw runtime_error(
        reinterpret_cast<const char *>(glewGetErrorString(res)));
  }
  CheckGLError();

  shader_ = new Shader("vertShader.glsl", "fragShader.glsl");
  CheckGLError();

  InitGeom();
  CheckGLError();

  glutMainLoop();
}

Renderer::~Renderer() { delete shader_; }

void Renderer::InitGeom() {
  geo_.InitGeom(shader_->id());
  CheckGLError();
}

void Renderer::Display() const {
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera_.LoadMatrices(shader_);

  // Load model transformation matrix
  if (!shader_->CopyDataToUniform(glm::identity<glm::dmat4>(), "model")) {
    cerr << "Model matrix not in shader" << endl;
  }

  geo_.Draw();

  glutSwapBuffers();
  CheckGLError();
}

void Renderer::Reshape(const int new_width, const int new_height) {
  viewport_width_ = new_width;
  viewport_height_ = new_height;

  camera_.set_aspect(new_width, new_height);
  glViewport(0, 0, viewport_width_, viewport_height_);
  glutPostRedisplay();
}

constexpr auto kMoveDelta = .05f;

void Renderer::Keyboard(const unsigned char key, const int, const int) {
  switch (key) {
    case 'x':
    case 'X':
    case 'q':
    case 'Q':
    case 27:  // ESC
      exit(0);
    case 'm':
    case 'M':
      mode_ = mode_ == kHeight ? kNormal : kHeight;
      break;
    case 'r':
    case 'R':
      geo_.Randomize(shader_->id());
    default:
      break;
  }
  HandleMovementKey(key, true);
  glutPostRedisplay();
}

void Renderer::KeyboardUp(const unsigned char key, const int, const int) {
  HandleMovementKey(key, false);
}

const auto kRotateDelta = .0025f;

void Renderer::Motion(const int x, const int y) { HandleMouseMove(x, y, true); }

void Renderer::PassiveMotion(const int x, const int y) {
  HandleMouseMove(x, y, false);
}

void Renderer::HandleMouseMove(const int x, const int y, const bool active) {
  if (active) {
    camera_.RelativeRotate(
        {0, static_cast<float>(y - last_mouse_y_) * kRotateDelta,
         static_cast<float>(last_mouse_x_ - x) * kRotateDelta});
  }
  last_mouse_x_ = x;
  last_mouse_y_ = y;
}

void Renderer::HandleMovementKey(const unsigned char key, const bool down) {
  switch (key) {
    case 'w':
    case 'W':
      move_forward_ = down;
      break;
    case 's':
    case 'S':
      move_backward_ = down;
      break;
    case 'a':
    case 'A':
      move_left_ = down;
      break;
    case 'd':
    case 'D':
      move_right_ = down;
      break;
    case 'z':
    case 'Z':
      move_down_ = down;
      break;
    case 'c':
    case 'C':
      move_up_ = down;
      break;
    default:
      break;
  }
}

void Renderer::Tick(int) {
  if (last_mouse_x_ < 0 || last_mouse_x_ > viewport_width_ ||
      last_mouse_y_ < 0 || last_mouse_y_ > viewport_height_) {
    move_forward_ = false;
    move_backward_ = false;
    move_left_ = false;
    move_right_ = false;
    move_up_ = false;
    move_down_ = false;
  }
  camera_.RelativeMove(
      {static_cast<float>((move_forward_ ? 1 : 0) + (move_backward_ ? -1 : 0)) *
           kMoveDelta,
       static_cast<float>((move_left_ ? 1 : 0) + (move_right_ ? -1 : 0)) *
           kMoveDelta,
       0.});
  camera_.AbsoluteMove(
      {0., 0.,
       static_cast<float>((move_up_ ? 1 : 0) + (move_down_ ? -1 : 0)) *
           kMoveDelta});
  glutPostRedisplay();
}

void Renderer::DisplayCB() { window->Display(); }

void Renderer::ReshapeCB(const int w, const int h) { window->Reshape(w, h); }

void Renderer::KeyboardCB(const unsigned char key, const int x, const int y) {
  window->Keyboard(key, x, y);
}

void Renderer::KeyboardUpCB(const unsigned char key, const int x, const int y) {
  window->KeyboardUp(key, x, y);
}

void Renderer::MotionCB(const int w, const int h) { window->Motion(w, h); }

void Renderer::PassiveMotionCB(const int w, const int h) {
  window->PassiveMotion(w, h);
}

constexpr auto kFPS = 60;

void Renderer::TimerCB(const int ticks) {
  glutTimerFunc(static_cast<unsigned int>(1000 / kFPS), TimerCB, ticks + 1);
  window->Tick(ticks);
}

void Renderer::CheckGLError() {
  for (auto rc = glGetError(); rc != GL_NO_ERROR; rc = glGetError()) {
    PrintOpenGLError(rc);
  }
}

void Renderer::PrintOpenGLError(GLenum errorCode) {
  switch (errorCode) {
    case GL_INVALID_VALUE:
      cerr << "GL_INVALID_VALUE" << endl;
      break;
    case GL_INVALID_OPERATION:
      cerr << "GL_INVALID_OPERATION" << endl;
      break;
    default:
      cerr << "Unknown OpenGL error" << endl;
  }
}
