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
  CheckGLError();

  GLenum res = glewInit();
  if (res != GLEW_OK) {
    cerr << "Error in glewInit()";
    throw runtime_error(
        reinterpret_cast<const char *>(glewGetErrorString(res)));
  }
  CheckGLError();

  shader_ = new Shader("phong.vert", "phong.frag");
  light_ = new PointLight(
      {kGeographyShort / 2, kGeographyLong / 2, kHeightMultiplier / 2});
  geo_ = new Geography();
  CheckGLError();

  InitGeom();
  CheckGLError();

  TimerCB(0);
  glutMainLoop();
}

Renderer::~Renderer() { delete shader_; }

void Renderer::InitGeom() {
  light_->InitGeom();
  geo_->InitGeom();
  CheckGLError();
}

void Renderer::Display() const {
  light_->GenerateCubeMaps(*geo_);

  glViewport(0, 0, viewport_width_, viewport_height_);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shader_->id());

  camera_.LoadMatrices(shader_);
  light_->LoadData(shader_);
  CheckGLError();

  // Load model transformation matrix
  if (!shader_->CopyDataToUniform(glm::identity<glm::dmat4>(), "model")) {
    cerr << "Model matrix not in shader" << endl;
  }
  if (!shader_->CopyDataToUniform(useColor_, "useColor")) {
    cerr << "Shader not considering colour toggle" << endl;
  }
  if (!shader_->CopyDataToUniform(useLight_, "useLight")) {
    cerr << "Shader not considering light toggle" << endl;
  }
  if (!shader_->CopyDataToUniform(useShadows_, "useShadows")) {
    cerr << "Shader not considering light toggle" << endl;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, light_->getDepthTexture());
  auto depthMap = glGetUniformLocation(shader_->id(), "depthMap");
  glUniform1i(depthMap, 0);

  light_->Render(shader_->id());
  geo_->Render(shader_->id());

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

void Renderer::Keyboard(const unsigned char key, const int, const int) {
  switch (key) {
    case 'x':
    case 'X':
    case 'q':
    case 'Q':
    case 27:  // ESC
      exit(0);
    case 'k':
    case 'K':
      setPointLight_ = !setPointLight_;
      break;
    case 'l':
    case 'L':
      useLight_ = !useLight_;
      break;
    case 'm':
    case 'M':
      useShadows_ = !useShadows_;
      break;
    case 'n':
    case 'N':
      useColor_ = !useColor_;
      break;
    case 'p':
    case 'P':
      simulating_ = !simulating_;
      break;
    case 'r':
    case 'R':
      geo_->Randomize(true);
    default:
      break;
  }
  HandleMovementKey(key, true);
  glutPostRedisplay();
}

void Renderer::KeyboardUp(const unsigned char key, const int, const int) {
  HandleMovementKey(key, false);
}

void Renderer::Motion(const int x, const int y) { HandleMouseMove(x, y, true); }

void Renderer::PassiveMotion(const int x, const int y) {
  HandleMouseMove(x, y, false);
}

const auto kRotateDelta = .0025f;

void Renderer::HandleMouseMove(const int x, const int y, const bool active) {
  if (active) {
    camera_.RelativeRotate(
        {0, static_cast<float>(y - last_mouse_y_) * kRotateDelta,
         static_cast<float>(last_mouse_x_ - x) * kRotateDelta});
  }
  last_mouse_x_ = x;
  last_mouse_y_ = y;
}

constexpr auto kMoveDelta = kGeographyShort * .01f;

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

void Renderer::Tick(int ticks) {
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

  if (setPointLight_) {
    light_->setPosition(camera_.getPosition());
    light_->setColors({1, 1, 1});
  } else if (simulating_) {
    auto lightAngle =
        fmod(static_cast<float>(ticks) * glm::two_pi<float>() / (kFPS * 20),
             glm::pi<float>() * 5 / 4) -
        glm::pi<float>() * 5 / 8;
    auto lightRotation =
        glm::vec3(glm::sin(lightAngle), 0, glm::cos(lightAngle)) *
        static_cast<float>(kGeographyShort);
    light_->setPosition(lightRotation +
                        glm::vec3(kGeographyShort / 2, kGeographyLong / 2, 0));
    auto baseLightColor = glm::max(0.0f, glm::cos(lightAngle * 0.9f));
    light_->setColors({baseLightColor, glm::pow(baseLightColor, 1.25),
                       glm::pow(baseLightColor, 1.25)});
  }

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
