#include "renderer.h"

#include <GL/freeglut_std.h>

#include <chrono>
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
  Grid::RandomizeBase();

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
  light_ = new PointLight({kGeographyShort * kGeographyCountShort / 2,
                           kGeographyLong * kGeographyCountLong / 2,
                           kHeightMultiplier * kGeographyCountShort / 2});

  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  auto start_time = chrono::high_resolution_clock::now();
  for (auto x = 0; x < kGeographyCountShort; ++x) {
    for (auto y = 0; y < kGeographyCountLong; ++y) {
      objects_.push_back(new Geography(x, y));
    }
  }

  auto end_time = chrono::high_resolution_clock::now();
  cout << "Generation time: "
       << duration_cast<milliseconds>(end_time - start_time).count() << "ms\n";
  cout << "       vertices: "
       << kGeographyCountShort * kGeographyCountLong * kGeographyShort *
              kGeographyLong
       << "\n";
  CheckGLError();

  InitGeom();
  CheckGLError();

  PrintKeyMap();
  TimerCB(0);
  glutMainLoop();
}

Renderer::~Renderer() { delete shader_; }

void Renderer::InitGeom() {
  light_->InitGeom();
  for (const auto geo : objects_) {
    geo->InitGeom();
  }
  CheckGLError();
}

void Renderer::Display() const {
  if (useShadows_ && shadowsChanged_) {
    light_->GenerateCubeMaps(objects_);
  }

  glViewport(0, 0, viewport_width_, viewport_height_);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shader_->id());

  camera_.LoadMatrices(shader_);
  light_->LoadData(shader_);
  CheckGLError();

  if (!shader_->CopyDataToUniform(useColor_, "useColor")) {
    cerr << "Shader not considering colour toggle" << endl;
  }
  if (!shader_->CopyDataToUniform(useLight_, "useLight")) {
    cerr << "Shader not considering light toggle" << endl;
  }
  if (!shader_->CopyDataToUniform(useShadows_, "useShadows")) {
    cerr << "Shader not considering light toggle" << endl;
  }
  float minHeight = numeric_limits<float>::infinity();
  float maxHeight = -minHeight;
  for (const auto object : objects_) {
    const auto geo = dynamic_cast<Geography *>(object);
    if (geo != nullptr) {
      float min = geo->min();
      if (min < minHeight) {
        minHeight = min;
      }
      float max = geo->max();
      if (max > maxHeight) {
        maxHeight = max;
      }
    }
  }
  if (!shader_->CopyDataToUniform(minHeight, "minHeight")) {
    cerr << "Shader not considering minimum height" << endl;
  }
  if (!shader_->CopyDataToUniform(maxHeight, "maxHeight")) {
    cerr << "Shader not considering maximum height" << endl;
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, light_->getDepthTexture());
  auto depthMap = glGetUniformLocation(shader_->id(), "depthMap");
  glUniform1i(depthMap, 0);

  light_->Render(shader_);
  for (const auto geo : objects_) {
    geo->Render(shader_);
  }

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
      Grid::RandomizeBase();
      for (const auto object : objects_) {
        const auto geo = dynamic_cast<Geography *>(object);
        if (geo != nullptr) {
          geo->Randomize(true);
        }
      }
      shadowsChanged_ = true;
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

void Renderer::HandleMouseMove(const int x, const int y, const bool active) {
  if (active) {
    camera_.RelativeRotate(
        {0, static_cast<float>(y - last_mouse_y_) * kRotateDelta,
         static_cast<float>(last_mouse_x_ - x) * kRotateDelta});
    glutPostRedisplay();
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

void Renderer::Tick(int ticks) {
  bool doneSomething = false;
  if (last_mouse_x_ < 0 || last_mouse_x_ > viewport_width_ ||
      last_mouse_y_ < 0 || last_mouse_y_ > viewport_height_) {
    move_forward_ = false;
    move_backward_ = false;
    move_left_ = false;
    move_right_ = false;
    move_up_ = false;
    move_down_ = false;
  }
  if (move_forward_ || move_backward_ || move_left_ || move_right_) {
    camera_.RelativeMove(
        {static_cast<float>((move_forward_ ? 1 : 0) +
                            (move_backward_ ? -1 : 0)) *
             kMoveDelta,
         static_cast<float>((move_left_ ? 1 : 0) + (move_right_ ? -1 : 0)) *
             kMoveDelta,
         0.});
    doneSomething = true;
  }
  if (move_up_ || move_down_) {
    camera_.AbsoluteMove(
        {0., 0.,
         static_cast<float>((move_up_ ? 1 : 0) + (move_down_ ? -1 : 0)) *
             kMoveDelta});
    doneSomething = true;
  }

  if (setPointLight_) {
    light_->setPosition(camera_.getPosition());
    light_->setColors({1, 1, 1});
    doneSomething = true;
    shadowsChanged_ = true;
  } else if (simulating_) {
    auto lightAngle =
        fmod(static_cast<float>(ticks) * glm::two_pi<float>() / (kFPS * 20),
             glm::pi<float>() * 5 / 4) -
        glm::pi<float>() * 5 / 8;
    auto lightRotation =
        glm::vec3(0, glm::sin(lightAngle), glm::cos(lightAngle) / 2) *
        static_cast<float>(kGeographyLong * kGeographyCountLong);
    light_->setPosition(lightRotation +
                        glm::vec3(kGeographyShort * kGeographyCountShort / 2,
                                  kGeographyLong + kGeographyCountShort / 2,
                                  0));
    auto baseLightColor = glm::max(0.0f, glm::cos(lightAngle));
    light_->setColors(
        {glm::pow(baseLightColor, 0.8), baseLightColor, baseLightColor});
    doneSomething = true;
    shadowsChanged_ = true;
  }

  if (doneSomething) {
    glutPostRedisplay();
  }
}

void Renderer::DisplayCB() {
  window->Display();
  window->shadowsChanged_ = false;
}

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
  auto tickIncrement = window->simulating_ ? 1 : 0;
  glutTimerFunc(static_cast<unsigned int>(1000 / kFPS), TimerCB,
                ticks + tickIncrement);
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

void Renderer::PrintKeyMap() {
  cout << "\n";
  cout << "Mouse control:\n";
  cout << "\tClick and drag to rotate camera\n";
  cout << "\n";
  cout << "Keyboard control (case-insensitive):\n";
  cout << "\tx, q, [ESC]: Quit program\n";
  cout << "\tr: Regenerate terrain\n";
  cout << "\n";
  cout << "\twasd: Move forward/left/backward/right relative to the camera\n";
  cout << "\tcz: Move up/down relative to the world\n";
  cout << "\n";
  cout << "\tk: Toggle point light following camera\n";
  cout << "\tl: Toggle phong light simulation\n";
  cout << "\tm: Toggle shadows\n";
  cout << "\tn: Toggle ground/normal colour\n";
  cout << "\tp: Toggle day/night cycle\n";
  cout << endl;
}
