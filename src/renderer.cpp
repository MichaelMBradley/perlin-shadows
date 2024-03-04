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

  glutInit(&argc, argv);
  glutInitWindowPosition(10, 10);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(viewport_width_, viewport_height_);
  glutCreateWindow("Ice Simulator");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glutReshapeFunc(ReshapeCB);
  glutDisplayFunc(DisplayCB);
  glutKeyboardFunc(KeyboardCB);
  glutMotionFunc(MotionCB);
  glutPassiveMotionFunc(PassiveMotionCB);

  glutMainLoop();
}

constexpr int normal_multiplier = 20;

void Renderer::NormalColor(const size_t x, const size_t y) const {
  const auto normal = geo_.normal_at(x, y, normal_multiplier);
  glColor3f(static_cast<GLfloat>(1. + normal.x) / 2,
            static_cast<GLfloat>(1. + normal.y) / 2,
            static_cast<GLfloat>(normal.z));
}

void Renderer::HeightColor(const size_t x, const size_t y) const {
  const auto g =
      static_cast<GLfloat>((geo_.height_at(x, y) - geo_.min_height()) /
                           (geo_.max_height() - geo_.min_height()));
  glColor3f(g, g, g);
}

constexpr auto height_multiplier = .5;
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
      // TODO: Find out why it fits the image better when I swap x and y
      glVertex3d(resize_y(y), resize_x(x),
                 geo_.height_at(x, y) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x),
                 geo_.height_at(x, y + 1) * height_multiplier);
      glVertex3d(resize_y(y), resize_x(x + 1),
                 geo_.height_at(x + 1, y) * height_multiplier);

      glVertex3d(resize_y(y), resize_x(x + 1),
                 geo_.height_at(x + 1, y) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x),
                 geo_.height_at(x, y + 1) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x + 1),
                 geo_.height_at(x + 1, y + 1) * height_multiplier);
    }
  }
  glEnd();

  glutSwapBuffers();
}

void Renderer::Reshape(const int new_width, const int new_height) {
  viewport_width_ = new_width;
  viewport_height_ = new_height;

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
    case 'w':
    case 'W':
      camera_.RelativeMove({kMoveDelta, 0., 0.});
      break;
    case 's':
    case 'S':
      camera_.RelativeMove({-kMoveDelta, 0., 0.});
      break;
    case 'a':
    case 'A':
      camera_.RelativeMove({0., kMoveDelta, 0.});
      break;
    case 'd':
    case 'D':
      camera_.RelativeMove({0., -kMoveDelta, 0.});
      break;
    case 'z':
    case 'Z':
      camera_.RelativeMove({0., 0, kMoveDelta});
      break;
    case 'c':
    case 'C':
      camera_.RelativeMove({0., 0, -kMoveDelta});
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

const auto kRotateDelta = .0075;

void Renderer::Motion(const int x, const int y) {
  camera_.RelativeRotate({0., (y - last_mouse_y_) * kRotateDelta,
                          (x - last_mouse_x_) * kRotateDelta});
  last_mouse_x_ = x;
  last_mouse_y_ = y;
  glutPostRedisplay();
}

void Renderer::PassiveMotion(const int x, const int y) {
  last_mouse_x_ = x;
  last_mouse_y_ = y;
}

void Renderer::DisplayCB() { window->Display(); }

void Renderer::ReshapeCB(const int w, const int h) { window->Reshape(w, h); }

void Renderer::KeyboardCB(const unsigned char key, const int x, const int y) {
  window->Keyboard(key, x, y);
}

void Renderer::MotionCB(const int w, const int h) { window->Motion(w, h); }

void Renderer::PassiveMotionCB(const int w, const int h) {
  window->PassiveMotion(w, h);
}
