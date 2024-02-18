#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_std.h>

#include "geography.h"

using namespace std;

constexpr int buffer = 20;
constexpr int scale = 1;

Geography geo;
void resize(int, int);
void render();
void keyPressed(unsigned char, int, int);

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(geo.get_width() * scale + buffer * 2, geo.get_length() * scale + buffer * 2);
	glutCreateWindow("COMP 3009 - Final Project");
	glutReshapeFunc(resize);
	glutDisplayFunc(render);
	glutKeyboardFunc(keyPressed);

	glutMainLoop();

	return 0;
}

void render() {
	glClearColor(0., 0., 0., 1.);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (size_t x = 0; x < geo.get_width(); ++x) {
		for (size_t y = 0; y < geo.get_length(); ++y) {
			const auto g = (geo.height_at(x, y) + 1.) / 2.;
			glColor3f(g, g, g);
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

void resize(int width, int height) {
    // Use the full view port
    glViewport(0, 0, width, height);

	// Use 2D  orthographic parallel projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
}

void keyPressed(unsigned char key, int, int) {
	switch (key) {
		case 'X':
		case 'x':
		case 'Q':
		case 'q':
		case 27: // ESC
			exit(0);
			break;
	}
}

