#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

#include "world.h"

// Global Variables
GLuint g_mainWnd;
GLuint WinWidth = 800;
GLuint WinHeight = 800;

World world;

// UI Parameters
#define ZOOM_RATE 1.0
#define PAN_RATE 1.0

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
//float camX = 0.0;
//float camY = 0.0;
//float camZ = 100.0;
float camX = 0.0;
float camY = 10.0;
float camZ = 5.0;
// Function Declarations
void SetLight();
void SetCamera();
void Display();
void SetShaders();
void Mouse(int button, int state, int x, int y);
void KeyPressed(unsigned char key, int x, int y);

void Zoom(int in);
void Pan(int dir);

// MAIN
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WinWidth, WinHeight);
	g_mainWnd = glutCreateWindow("Graphic Meltdown");

	// Create Default World Object
	world = World();
	if (argc > 1)
		world.LoadModel(argv[1]);
	else
		world.LoadDefaultModel();

	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(KeyPressed);

	SetLight();
	SetCamera();
	/*
	glewInit();
			if (glewIsSupported("GL_VERSION_2_0"))
				printf("Ready for OpenGL 2.0\n");
			else {
				printf("OpenGL 2.0 not supported\n");
				exit(1);
			}
			SetShaders();
			*/
	glutMainLoop();

	return 0;
}

void SetShaders() {
/*
		char *vs,*fs;

		GLuint v = glCreateShader(GL_VERTEX_SHADER);
		GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

		vs = textFileRead("toon.vert");
		fs = textFileRead("toon.frag");

		const char * vv = vs;
		const char * ff = fs;

		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		free(vs);free(fs);

		glCompileShader(v);
		glCompileShader(f);

		GLuint p = glCreateProgram();

		glAttachShader(p,v);
		glAttachShader(p,f);

		glLinkProgram(p);
		glUseProgram(p); */

	}


void Mouse(int button, int state, int x, int y) {

}

void KeyPressed(unsigned char key, int x, int y) {
	switch (key) {
	case '+':
		Zoom(1); // Zoom 1 = in
		break;
	case '-':
		Zoom(0); // Zoom 0 = out
		break;
	case 'w':
		Pan(UP);
		break;
	case 'a':
		Pan(LEFT);
		break;
	case 's':
		Pan(DOWN);
		break;
	case 'd':
		Pan(RIGHT);
		break;
	}
}

void SetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, (double) WinWidth / (double) WinHeight, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camX, camY, camZ, 0.0, 2.5, 0.0, 0.0, 1.0, 0.0);

}

// Set Light
void SetLight() {
	float direction[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	float diffintensity[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

// Display function
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.0f, 1.0f, 1.0f); /* set object color as cyan */

	world.Draw();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

// UI Controls
void Pan(int dir) {
	switch (dir) {
	case (UP):
		camY += PAN_RATE;
		break;
	case (DOWN):
		camY -= PAN_RATE;
		break;
	case (LEFT):
		camX -= PAN_RATE;
		break;
	case (RIGHT):
		camX += PAN_RATE;
		break;
	}
	SetCamera();
	glutPostRedisplay();
}

void Zoom(int in) {
	if (in) {
		camZ -= ZOOM_RATE;
	} else {
		camZ += ZOOM_RATE;
	}

	SetCamera();
	glutPostRedisplay();
}

