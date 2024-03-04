#include "renderer.h"

#include <GL/freeglut_std.h>

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

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
  glutInitWindowSize(viewport_width, viewport_height);
  glutCreateWindow("COMP 3009 - Final Project");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glutReshapeFunc(reshapeCB);
  glutDisplayFunc(displayCB);
  glutKeyboardFunc(keyboardCB);

  glutMainLoop();
}

constexpr int normal_multiplier = 20;

void Renderer::normal_color(const size_t x, const size_t y) const {
  const auto normal = geo.normal_at(x, y, normal_multiplier);
  glColor3f(static_cast<GLfloat>(1. + normal.x) / 2,
            static_cast<GLfloat>(1. + normal.y) / 2,
            static_cast<GLfloat>(normal.z));
}

void Renderer::height_color(const size_t x, const size_t y) const {
  const auto g = static_cast<GLfloat>((geo.height_at(x, y) - geo.min_height()) /
                                      (geo.max_height() - geo.min_height()));
  glColor3f(g, g, g);
}

constexpr auto height_multiplier = .5;
constexpr auto min_geography_dimension = min(geography_width, geography_length);

void Renderer::display() const {
  glClearColor(0., 0., 0., 1.);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO: Add camera class that allows us to update these variables
  const auto viewMatrix = glm::lookAt(
      glm::dvec3(0., 0., 2.), glm::dvec3(0., 0., 0.), glm::dvec3(1., 0., 0.));
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(&viewMatrix[0][0]);

  const auto perspectiveMatrix = glm::perspective(
      glm::radians(45.), static_cast<double>(viewport_width) / viewport_height,
      .1, 100.);
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixd(&perspectiveMatrix[0][0]);

  glBegin(GL_TRIANGLES);
  for (size_t x = 0; x < geography_width - 1; ++x) {
    for (size_t y = 0; y < geography_length - 1; ++y) {
      switch (mode) {
        case normal:
          normal_color(x, y);
          break;
        case height:
          height_color(x, y);
          break;
      }

      constexpr auto half_width = static_cast<GLdouble>(geography_width) / 2.;
      constexpr auto half_length = static_cast<GLdouble>(geography_length) / 2.;
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
                 geo.height_at(x, y) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x),
                 geo.height_at(x, y + 1) * height_multiplier);
      glVertex3d(resize_y(y), resize_x(x + 1),
                 geo.height_at(x + 1, y) * height_multiplier);

      glVertex3d(resize_y(y), resize_x(x + 1),
                 geo.height_at(x + 1, y) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x),
                 geo.height_at(x, y + 1) * height_multiplier);
      glVertex3d(resize_y(y + 1), resize_x(x + 1),
                 geo.height_at(x + 1, y + 1) * height_multiplier);
    }
  }
  glEnd();

  glutSwapBuffers();
}

void Renderer::reshape(const int new_width, const int new_height) {
  viewport_width = new_width - buffer * 2;
  viewport_height = new_height - buffer * 2;

  glViewport(buffer, buffer, viewport_width, viewport_height);
  glutPostRedisplay();
}

void Renderer::keyboard(const unsigned char key, const int, const int) {
  switch (key) {
    case 'X':
    case 'x':
    case 'Q':
    case 'q':
    case 27:  // ESC
      exit(0);
    case 'm':
    case 'M':
      mode = mode == height ? normal : height;
      break;
    case 'r':
    case 'R':
      geo.randomize();
    default:
      break;
  }
  glutPostRedisplay();
}

void Renderer::displayCB() { window->display(); }

void Renderer::reshapeCB(const int w, const int h) { window->reshape(w, h); }

void Renderer::keyboardCB(const unsigned char key, const int x, const int y) {
  window->keyboard(key, x, y);
}
