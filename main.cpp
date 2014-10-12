#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "world.h"
#include "main.h"
#include "quaternion.h"

// Global Variables
GLuint g_mainWnd;
GLuint WinWidth = 800;
GLuint WinHeight = 800;

World world;

// UI Parameters
#define ZOOM_RATE 1.0
#define PAN_RATE 1.0

const point2d origin = {0.0 , 0.0};

int leftMouseState = GLUT_UP;
int middleMouseState = GLUT_UP;
int rightMouseState = GLUT_UP;
point2d leftMouseDownPt = origin;
point2d middleMouseDownPt = origin;
point2d rightMouseDownPt = origin;

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// Panning/ Zooming/ Rotating Controls
float downModOriginX = 0.0,  downModOriginY = 0.0,  downModOriginZ = 0.0;
float leftSelectX = 0.0, leftSelectY = 0.0, leftSelectZ = 0.0;

// Model Positioning
float modOriginX = 0.0,  modOriginY = 0.0,  modOriginZ = 0.0; // Translation of model
float xRot = 0.0, yRot = 0.0, zRot = 0.0; // Rotation of Model
quaternion modRotation;

// Laser
float lasOriginX = 10.0, lasOriginY = -10.0, lasOriginZ = 100.0;
float lasTargetX = 0.0, lasTargetY = 0.0, lasTargetZ = 0.0;

// Function Declarations
void SetLight() ;
void SetCamera() ;
void Display() ;

void Mouse(int button, int state, int x, int y);
void MouseMoved(int x, int y);
void KeyPressed (unsigned char key, int x, int y);
void reshape(int w, int h);

void Zoom(int in);
void Pan(int dir);
void ArcRotate(float x, float y);
void ArcPan(int x, int y);

//Laser
void DrawLaser();

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

	glutReshapeFunc(reshape);

	glutMouseFunc(Mouse);

	glutMotionFunc(MouseMoved);
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

void pickPoint(int button, int state, int x, int y){
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN ) {

		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
		glGetDoublev( GL_PROJECTION_MATRIX, projection );
		glGetIntegerv( GL_VIEWPORT, viewport );

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

		gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		printf("Picking: %f %f %f\n", posX, posY, posZ);
	}
}

void Mouse(int button, int state, int x, int y)
{
	pickPoint(button, state, x, y);
	if(button == GLUT_LEFT_BUTTON){
		leftMouseState = state;
		if(state==GLUT_DOWN){
			GLint viewport[4];
			GLdouble modelview[16];
			GLdouble projection[16];
			GLfloat winX, winY, winZ;
			GLdouble posX, posY, posZ;

			glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
			glGetDoublev( GL_PROJECTION_MATRIX, projection );
			glGetIntegerv( GL_VIEWPORT, viewport );

			winX = (float)x;
			winY = (float)viewport[3] - (float)y;
			glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

			gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

			printf("Picking: %f %f %f\n", posX, posY, posZ);

			if(posZ > -800) {
				leftSelectX = posX;
				leftSelectY = posY;
				leftSelectZ = winZ;

				downModOriginX = modOriginX;
				downModOriginY = modOriginY;
			} else
				leftMouseState = GLUT_UP;
		}
	} else if(button == GLUT_RIGHT_BUTTON){
		rightMouseState = state;
		if(state==GLUT_DOWN){
			rightMouseDownPt.x = x;
			rightMouseDownPt.y = y;
		}
	} else if(button == GLUT_MIDDLE_BUTTON){
		middleMouseState = state;
		if(state==GLUT_DOWN){
			middleMouseDownPt.x = x;
			middleMouseDownPt.y = y;
		}
	}
}



void MouseMoved(int x, int y){
	if(leftMouseState==GLUT_DOWN){
		ArcPan(x,y);
	} else if(rightMouseState==GLUT_DOWN){
		ArcRotate(float(x-rightMouseDownPt.x), float(y-rightMouseDownPt.y));
	}
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


	gluLookAt(1.0, 1.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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

void reshape(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio = 1.0 * w / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

// Display function
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.0f, 1.0f, 1.0f); /* set object color as cyan */

	glPushMatrix();
		glTranslatef(modOriginX,modOriginY, modOriginZ);
		glRotatef(xRot, 0.0, 1.0, 0.0);
		glRotatef(yRot, 1.0, 0.0, 0.0);
		glRotatef(zRot, 0.0, 0.0, 1.0);
		world.Draw();
	glPopMatrix();

	DrawLaser();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}



// UI Controls
void ArcPan(int x, int y){

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	winZ = (float)leftSelectZ;
	//glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);


	printf("PAN:\nPicking: %f %f %f\n", posX, posY, posZ);
	printf("lef Select: %f %f\n", leftSelectX, leftSelectY);
	printf("Compare: %f %f\n", leftSelectZ, winZ);

	modOriginX = downModOriginX + posX - leftSelectX;
	modOriginY = downModOriginY + posY - leftSelectY;

	glutPostRedisplay();
}

void ArcRotate(float x, float y){
	printf("Arc rotate\n");
	if(xRot > 135)
		yRot -= y/90.0;
	else if(xRot>45)
		zRot += y/90.0;
	else if(xRot>-45)
		yRot += y/90.0;
	else if(xRot>-135)
		zRot -= y/90.0;
	else
		yRot -= y/90.0;

	xRot += x/90.0;


	while(xRot>180)
		xRot -= 360;
	while(yRot>180)
		yRot -= 360;
	while(xRot<-180)
		xRot += 360;
	while(yRot<-180)
		yRot += 360;

	glutPostRedisplay();
}

// UI Controls
void Pan(int dir){
	printf("Mod Origin: %f %f\n", modOriginX, modOriginY);

	switch(dir){
	case(UP):
		modOriginY += PAN_RATE;
		break;
	case(DOWN):
		modOriginY -= PAN_RATE;
		break;
	case(LEFT):
		modOriginX -= PAN_RATE;
		break;
	case(RIGHT):
		modOriginX += PAN_RATE;
		break;
	}
	glutPostRedisplay();
}

void Zoom(int in){
	if(in){
		modOriginZ += ZOOM_RATE;
	} else{
		modOriginZ -= ZOOM_RATE;
	}

	SetCamera();
}

// LASER
void DrawLaser(){
	GLUquadric *q = gluNewQuadric();
	float lasDiam = 0.2;

	// Direction Vector of Laser
	float lasVecX = lasTargetX - lasOriginX;
	float lasVecY = lasTargetY - lasOriginY;
	float lasVecZ = lasTargetZ - lasOriginZ;

	// Normal Vector
	float n_X = lasVecY;
	float n_Y = -lasVecX;
	float n_Z = 0.0;

	// Find Rotation angle
	float dotProd = n_X * lasVecX + n_Y * lasVecY + n_Z * lasVecZ;
	float magLasVec = sqrt(lasVecX*lasVecX + lasVecY*lasVecY + lasVecZ*lasVecZ);
	float angle = acos(dotProd/magLasVec);


	glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
		glDisable(GL_LIGHTING);
		//glTranslatef(0.0f, -2.0f, 0.0f);
		//glRotatef(-90.0, 1.0, 0.0, 0.0);	// Rotate cone to face downwards in line with light
		glRotatef( angle, n_X, n_Y, n_Z);

		glPushMatrix();
			glBlendFunc (GL_SRC_ALPHA, GL_ONE);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			gluCylinder(q, 0.08, 0.1, lasOriginZ, 20, 200);
		glPopMatrix();

		glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();
			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
			gluCylinder(q, lasDiam, lasDiam, lasOriginZ, 20, 200);
		glPopMatrix();

		glPushMatrix();
			glColor4f(0.3f, 0.0f, 0.0f, 0.2f);
			gluCylinder(q, 0.25, 0.25, lasOriginZ, 20, 200);
		glPopMatrix();

		glPushMatrix();
			glColor4f(0.2f, 0.0f, 0.0f, 0.1f);
			gluCylinder(q, 0.28, 0.28, lasOriginZ, 20, 200);
		glPopMatrix();

		glPushMatrix();
			glColor4f(0.1f, 0.0f, 0.0f, 0.1f);
			gluCylinder(q, 0.3, 0.3, lasOriginZ, 20, 200);
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_BLEND);
}
