#include <GL/glew.h>
#include <GL/freeglut_std.h>

#include "constants.h"
#include "geography.h"

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
	const auto normal = geo.normal_at(x, y, normal_multiplier);
	glColor3f(
            static_cast<GLfloat>(1. + normal.x()) / 2,
            static_cast<GLfloat>(1. + normal.y()) / 2,
            static_cast<GLfloat>(normal.z())
    );
}

void height_color(const size_t x, const size_t y) {
	const auto g = static_cast<GLfloat>(
            (geo.height_at(x, y) - geo.min_height())
            / (geo.max_height() - geo.min_height())
    );
	glColor3f(g, g, g);
}

void render() {
	glClearColor(0., 0., 0., 1.);
	glClear(GL_COLOR_BUFFER_BIT);

    const auto color_func = mode == normal ? normal_color : height_color;

	glBegin(GL_POINTS);
	for (size_t x = 0; x < width; ++x) {
		for (size_t y = 0; y < length; ++y) {
			color_func(x, y);

			for (size_t xi = 0; xi < scale; ++xi) {
				for (size_t yi = 0; yi < scale; ++yi) {
					glVertex2f(
                            static_cast<GLfloat>(buffer + x * scale + xi),
                            static_cast<GLfloat>(buffer + y * scale + yi)
                    );
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
