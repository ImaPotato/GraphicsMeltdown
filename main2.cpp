#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include "arcball.h"
#include "world.h"
#include <iostream>
#include <fstream>

static float aspect_ratio = 1.0f;
static int width, height;

// scene parameters
const vec eye(0.0f, 0.0f, -2.0f);
const vec centre(0.0f, 0.0f, 0.0f);
const vec up(0.0f, 1.0f, 0.0f);

static GLfloat spotX = 0;
static GLfloat spotY = 5;
static GLfloat spotZ = 0;

static GLfloat spotRotX = 0;
static GLfloat spotRotY = -1;
static GLfloat spotRotZ = 0;

static GLfloat spotRad = 24.0f;

static GLfloat spotBright = 1.0f;
GLuint p, f, v;
World world;

const float PI = 3.141592654f;

static void reset_view(int w, int h) {
	width = w;
	height = h;
	aspect_ratio = (float) width / (float) height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, aspect_ratio, 1.0f, 50.0f);
	gluLookAt(eye.x, eye.y, eye.z, centre.x, centre.y, centre.z, up.x, up.y,
			up.z);
	// set up the arcball using the current projection matrix
	arcball_setzoom(1.0f, eye, up);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

inline float randf() {
	return ((1.0f / 127.f) * (((float) (rand() & 255)) - 128.0f));
}

inline vec rotate_x(vec v, float sin_ang, float cos_ang) {
	return vec(v.x, (v.y * cos_ang) + (v.z * sin_ang),
			(v.z * cos_ang) - (v.y * sin_ang));
}

inline vec rotate_y(vec v, float sin_ang, float cos_ang) {
	return vec((v.x * cos_ang) + (v.z * sin_ang), v.y,
			(v.z * cos_ang) - (v.x * sin_ang));
}

static void click_scene(int x, int y) {
	int invert_y = (height - y) - 1; // OpenGL viewport coordinates are Cartesian
	arcball_start(x, invert_y);
}

static void drag_scene(int x, int y) {
	int invert_y = (height - y) - 1;
	arcball_move(x, invert_y);
}

static void draw_scene() {
	arcball_rotate();
	world.Draw();
}

static void resize(int w, int h) {
	reset_view(w, h);
}

static void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);

	float position[] = { 2.0f, 1.0f, 5.0f, 1.0f };
	float diffintensity[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	float lightDiffuse[] = { spotBright, spotBright, spotBright, spotBright };
	float lightSpecular[] = { 0.5f, 0.5f, 0.5f, 0.3f };
	float lightPosition[] = { spotX, spotY, spotZ, 1.0f };
	float lightDirection[] = { spotRotX, spotRotY, spotRotZ, 1 };
	float noAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection);
	glLightfv(GL_LIGHT1, GL_AMBIENT, noAmbient);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotRad);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
	glEnable(GL_LIGHT1);

	float diffuse[] = { 0.3, 0.3, 0.3, 0.05 };
	float pos[] = { 3, 3, 3, 1 };
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, pos);
	glEnable(GL_LIGHT2);

	glUseProgram(p);
	draw_scene();
	glUseProgram(-1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_NORMALIZE);
	glDisable(GL_AUTO_NORMAL);

	glutSwapBuffers();
}

static void key(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static void idle() {
	glutPostRedisplay();
}

static void mouse_button(int button, int state, int x, int y) {
	if (state == GLUT_DOWN)
		click_scene(x, y);
}

static void mouse_motion(int x, int y) {
	drag_scene(x, y);
}

char *textFileRead(char *fn) {
	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {
		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *) malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}

	if (content == NULL) {
		fprintf(stderr, "ERROR: could not load in file %s\n", fn);
		exit(1);
	}
	return content;
}
/** I don't think this is working, but at least it compiles :) */
void setShaders() {

	char *vs, *fs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("phong.vert");
	fs = textFileRead("phong.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);

}

int main(int argc, char ** argv) {
	arcball_reset();

	glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);

	glutCreateWindow("Graphics Meltdown");

	world = World();

	if (argc > 1)
		world.LoadModel(argv[1]);
	else
		world.LoadDefaultModel();

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
//	glShadeModel(GL_SMOOTH);
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);

//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);

	glewInit();

	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

	glutMainLoop();

	return 0; // never reached
}

// end of file
