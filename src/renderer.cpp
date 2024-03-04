#include "renderer.h"

#include <GL/freeglut_std.h>
#include <GL/glew.h>

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

  glutReshapeFunc(reshapeCB);
  glutDisplayFunc(displayCB);
  glutKeyboardFunc(keyboardCB);

  glutMainLoop();
}

constexpr int normal_multiplier = 20;

void Renderer::normal_color(const size_t x, const size_t y) const {
  const auto normal = geo.normal_at(x, y, normal_multiplier);
  glColor3f(static_cast<GLfloat>(1. + normal.x()) / 2,
            static_cast<GLfloat>(1. + normal.y()) / 2,
            static_cast<GLfloat>(normal.z()));
}

void Renderer::height_color(const size_t x, const size_t y) const {
  const auto g = static_cast<GLfloat>((geo.height_at(x, y) - geo.min_height()) /
                                      (geo.max_height() - geo.min_height()));
  glColor3f(g, g, g);
}

void Renderer::display() const {
  glClearColor(0., 0., 0., 1.);
  glClear(GL_COLOR_BUFFER_BIT);

  const auto x_scale = static_cast<GLfloat>(viewport_width) / geography_width;
  const auto y_scale = static_cast<GLfloat>(viewport_height) / geography_length;

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
      glVertex2f(static_cast<GLfloat>(x) * x_scale,
                 static_cast<GLfloat>(y) * y_scale);
      glVertex2f(static_cast<GLfloat>(x) * x_scale,
                 static_cast<GLfloat>(y + 1) * y_scale);
      glVertex2f(static_cast<GLfloat>(x + 1) * x_scale,
                 static_cast<GLfloat>(y) * y_scale);
      glVertex2f(static_cast<GLfloat>(x + 1) * x_scale,
                 static_cast<GLfloat>(y) * y_scale);
      glVertex2f(static_cast<GLfloat>(x) * x_scale,
                 static_cast<GLfloat>(y + 1) * y_scale);
      glVertex2f(static_cast<GLfloat>(x + 1) * x_scale,
                 static_cast<GLfloat>(y + 1) * y_scale);
    }
  }
  glEnd();

  glutSwapBuffers();
}

void Renderer::reshape(const int new_width, const int new_height) {
  viewport_width = new_width - buffer * 2;
  viewport_height = new_height - buffer * 2;

  glViewport(buffer, buffer, viewport_width, viewport_height);

  // Use 2D  orthographic parallel projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, viewport_width, 0.0, viewport_height);
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
    case 's':
    case 'S':
      geo.simulate_random_drop();
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
