#include <iostream>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_std.h>

#include "constants.h"
#include "geography.h"
#include "vec.h"

using namespace std;

constexpr int buffer = 20;
constexpr int scale = 2;

enum Render { height, normal };
Render mode = normal;

Geography geo;
void resize(int, int);
void render();
void keyPressed(unsigned char, int, int);

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(width * scale + buffer * 2, length * scale + buffer * 2);
	glutCreateWindow("COMP 3009 - Final Project");
	glutReshapeFunc(resize);
	glutDisplayFunc(render);
	glutKeyboardFunc(keyPressed);

	glutMainLoop();

	return 0;
}

constexpr int normal_multiplier = 20;

void normal_color(const size_t x, const size_t y) {
	const auto low_x = x == 0 ? 0 : x - 1;
	const auto high_x = x == width - 1 ? x : x + 1;
	const auto low_y = y == 0 ? 0 : y - 1;
	const auto high_y = y == length - 1 ? y : y + 1;

	const auto x_diff = Vec3(
		static_cast<double>(high_x - low_x),
		0.,
		normal_multiplier * (geo.height_at(high_x, y) - geo.height_at(low_x, y))
	);
	const auto y_diff = Vec3(
		0.,
		static_cast<double>(high_y - low_y),
		normal_multiplier * (geo.height_at(x, high_y) - geo.height_at(x, low_y))
	);
	const auto normal = x_diff.cross(y_diff).norm();
	glColor3f((1. + normal.x()) / 2, (1. + normal.y()) / 2, normal.z());
}

void height_color(const size_t x, const size_t y) {
	const auto g = (geo.height_at(x, y) - geo.min_height()) / (geo.max_height() - geo.min_height());
	glColor3f(g, g, g);
}

void render() {
	glClearColor(0., 0., 0., 1.);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (size_t x = 0; x < width; ++x) {
		for (size_t y = 0; y < length; ++y) {
			switch (mode) {
				case normal:
					normal_color(x, y);
					break;
				case height:
					height_color(x, y);
					break;
			}

			for (size_t xi = 0; xi < scale; ++xi) {
				for (size_t yi = 0; yi < scale; ++yi) {
					glVertex2f(buffer + x * scale + xi, buffer + y * scale + yi);
				}
			}
		}
	}
	glEnd();

	glutSwapBuffers();
}

void resize(const int new_width, const int new_height) {
    // Use the full view port
    glViewport(0, 0, new_width, new_height);

	// Use 2D  orthographic parallel projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluOrtho2D(0.0, new_width, 0.0, new_height);
}

void keyPressed(const unsigned char key, const int, const int) {
	switch (key) {
		case 'X':
		case 'x':
		case 'Q':
		case 'q':
		case 27: // ESC
			exit(0);
			break;
		case 'm':
		case 'M':
			mode = mode == height ? normal : height;
			glutPostRedisplay();
			break;
	}
}

