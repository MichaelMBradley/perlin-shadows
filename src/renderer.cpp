#include "renderer.h"

#include <GL/freeglut_std.h>

#include <stdexcept>

#include "constants.h"

using namespace std;

Renderer *Renderer::window = nullptr;

Renderer::Renderer(int argc, char *argv[]) {
  if (window != nullptr) {
    throw runtime_error("Only one window may exist.");
  }
  window = this;

  glutInit(&argc, argv);
  glutInitWindowPosition(10, 10);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("Ice Simulator");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glutReshapeFunc(ReshapeCB);
  glutDisplayFunc(DisplayCB);
  glutKeyboardFunc(KeyboardCB);
  glutKeyboardUpFunc(KeyboardUpCB);
  glutMotionFunc(MotionCB);
  glutPassiveMotionFunc(PassiveMotionCB);
  TimerCB(0);

  glutMainLoop();
}

constexpr auto height_multiplier = .15;
constexpr auto normal_multiplier = 60 * height_multiplier;

void Renderer::NormalColor(const size_t x, const size_t y) const {
  const auto normal = geo_.normal_at(x, y, normal_multiplier);
  glColor3d((1. + normal.x) / 2, (1. + normal.y) / 2, normal.z);
}

void Renderer::HeightColor(const size_t x, const size_t y) const {
  const auto g = (geo_.height_at(x, y) - geo_.min_height()) /
                 (geo_.max_height() - geo_.min_height());
  glColor3d(g, g, g);
}

constexpr auto min_geography_dimension = min(kGeographyWidth, kGeographyLength);

void Renderer::Display() const {
  glClearColor(0., 0., 0., 1.);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  camera_.LoadMatrices();

  glBegin(GL_TRIANGLES);
  for (size_t x = 0; x < kGeographyWidth - 1; ++x) {
    for (size_t y = 0; y < kGeographyLength - 1; ++y) {
      switch (mode_) {
        case kNormal:
          NormalColor(x, y);
          break;
        case kHeight:
          HeightColor(x, y);
          break;
      }

      constexpr auto half_width = static_cast<GLdouble>(kGeographyWidth) / 2.;
      constexpr auto half_length = static_cast<GLdouble>(kGeographyLength) / 2.;
      const auto resize_x = [&](const size_t x) -> GLdouble {
        return (static_cast<GLdouble>(x) - half_width) /
               min_geography_dimension;
      };
      const auto resize_y = [&](const size_t y) -> GLdouble {
        return (static_cast<GLdouble>(y) - half_length) /
               min_geography_dimension;
      };

      // Drawing the 2 triangles that make up the grid cell counterclockwise
      glVertex3d(resize_x(x), resize_y(y),
                 geo_.height_at(x, y) * height_multiplier);
      glVertex3d(resize_x(x + 1), resize_y(y),
                 geo_.height_at(x + 1, y) * height_multiplier);
      glVertex3d(resize_x(x), resize_y(y + 1),
                 geo_.height_at(x, y + 1) * height_multiplier);

      glVertex3d(resize_x(x), resize_y(y + 1),
                 geo_.height_at(x, y + 1) * height_multiplier);
      glVertex3d(resize_x(x + 1), resize_y(y),
                 geo_.height_at(x + 1, y) * height_multiplier);
      glVertex3d(resize_x(x + 1), resize_y(y + 1),
                 geo_.height_at(x + 1, y + 1) * height_multiplier);
    }
  }
  glEnd();

  glutSwapBuffers();
}

void Renderer::Reshape(const int new_width, const int new_height) {
  viewport_width_ = new_width;
  viewport_height_ = new_height;

  camera_.set_aspect(new_width, new_height);
  glViewport(0, 0, viewport_width_, viewport_height_);
  glutPostRedisplay();
}

constexpr auto kMoveDelta = .05;

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
      geo_.Randomize();
    default:
      break;
  }
  HandleMovementKey(key, true);
  glutPostRedisplay();
}

void Renderer::KeyboardUp(const unsigned char key, const int, const int) {
  HandleMovementKey(key, false);
}

const auto kRotateDelta = .0025;

void Renderer::Motion(const int x, const int y) { HandleMouseMove(x, y, true); }

void Renderer::PassiveMotion(const int x, const int y) {
  HandleMouseMove(x, y, false);
}

void Renderer::HandleMouseMove(const int x, const int y, const bool active) {
  if (active) {
    camera_.RelativeRotate({0., (y - last_mouse_y_) * kRotateDelta,
                            (last_mouse_x_ - x) * kRotateDelta});
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
      {((move_forward_ ? 1 : 0) + (move_backward_ ? -1 : 0)) * kMoveDelta,
       ((move_left_ ? 1 : 0) + (move_right_ ? -1 : 0)) * kMoveDelta, 0.});
  camera_.AbsoluteMove(
      {0., 0., ((move_up_ ? 1 : 0) + (move_down_ ? -1 : 0)) * kMoveDelta});
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
  glutTimerFunc(static_cast<unsigned int>(1000. / kFPS), TimerCB, ticks + 1);
  window->Tick(ticks);
}
