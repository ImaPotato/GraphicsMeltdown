#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include "define.h"
#include "arcball.h"
#include "world.h"
#include <iostream>
#include <fstream>
#include <time.h>

static float aspect_ratio = 1.0f;
static int width, height;

#define ZOOM_RATE 1.0
#define PAN_RATE 1.0

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

// Panning/ Zooming/ Rotating Controls
float downModOriginX = 0.0,  downModOriginY = 0.0,  downModOriginZ = 0.0;
float leftSelectX = 0.0, leftSelectY = 0.0, leftSelectZ = 0.0;

// Model Positioning
float modOriginX = 0.0,  modOriginY = 0.0,  modOriginZ = 0.0; // Translation of model
float xRot = 0.0, yRot = 0.0, zRot = 0.0; // Rotation of Model

// Laser
int laserOn = 0;
Point computeLaserDirection();
Point lasOrigin = {0.0, 0.0, 1.0}; // Point at which laser begins
Point lasEnd = {0.0, 0.0, 0.0}; // Point at which laser ends (typically at a hit particle)
Point lasTarget = {0.0, 0.0, 0.0}; // Point clicked on as the target
Point lasVector = computeLaserDirection(); // Direction of laser (lasTarget - lasOrigin)
double loDotLv, lvDotLv; // Dot products: lasOrigin DOT lasVector and lasVector DOT lasVector
double PARTICLE_R2 = 0.02; // Square Radius of particles TODO
double partZProx = -1000; // Proximity (squared) of closest particle to laser Origin
double LAS_DIAM = 0.01;

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

bool invertMatrix(const double m[16], double invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

float getRandom(){
	float f = randf();
	return f;
}

// LASER
void readyLaserForCollisions(){
	lasEnd.x = lasTarget.x;
	lasEnd.y = lasTarget.y;
	lasEnd.z = lasTarget.z;
	//printf("Finding Intersection Ready End: %f %f %f\n", lasEnd.x, lasEnd.y, lasEnd.z );

//	lasEnd.x = lasVector.x + lasOrigin.x;
//	lasEnd.y = lasVector.y + lasOrigin.y;
//	lasEnd.z = lasVector.z + lasOrigin.z;

	partZProx = -1000;
	loDotLv = lasOrigin.x*lasVector.x + lasOrigin.y*lasVector.y + lasOrigin.z*lasVector.z;
	lvDotLv = lasVector.x*lasVector.x + lasVector.y*lasVector.y + lasVector.z*lasVector.z;
}

void intersectLaser(double p_x, double p_y, double p_z){ // Arguments are x, y, z of point p (p is the particle)

	// TODO

	p_x /= 16;
	p_y /= 16;
	p_z /= 16;
	p_x -= 0.5;
	p_y -= 0.5;
	p_z -= 0.5;

	// TODO

	double lvDotP = lasVector.x*p_x + lasVector.y*p_y + lasVector.z*p_z;
	double t = (lvDotP - loDotLv)/lvDotLv;

	// Find point p2. p2 is the closest point on lasVector to p
	Point p2;
	p2.x = t*lasVector.x + lasOrigin.x;
	p2.y = t*lasVector.y + lasOrigin.y;
	p2.z = t*lasVector.z + lasOrigin.z;

	// Find the square of distance between p2 and p. This will be shortest distance (squared) between lasVector and p
	double dist2 = (p2.x-p_x)*(p2.x-p_x) + (p2.y-p_y)*(p2.y-p_y) + (p2.z-p_z)*(p2.z-p_z);
	if(dist2 < PARTICLE_R2){ // If laser vector passes through particle's radius
		// Find squared distance to laser origin
		//printf("Found candidate...");
		//printf("Dist2=%f ;   p = {%f %f %f}   p2 = {%f %f %f}   lv  = {%f %f %f}\n", dist2, p_x, p_y, p_z, p2.x, p2.y, p2.z, lasVector.x, lasVector.y, lasVector.z );
		//double prox2 = (p2.x-lasOrigin.x)*(p2.x-lasOrigin.x) + (p2.y-lasOrigin.y)*(p2.y-lasOrigin.y) + (p2.z-lasOrigin.z)*(p2.z-lasOrigin.z);

		if(p2.z > partZProx){ // If particle is closer than current closest particle
		//	printf("Nearest\n");
			//nearestPart = pp; // Set this particle as current nearest

			// Set laser end point to this particles coords
			lasEnd.x = p2.x;
			lasEnd.y = p2.y;
			lasEnd.z = p2.z;

			//printf("Better End: %f %f %f\n", lasEnd.x, lasEnd.y, lasEnd.z );

			partZProx = p2.z;
		}
	}
}

Point computeLaserDirection(){
	// Direction Vector of Laser
	Point dir;
	dir.x = lasTarget.x - lasOrigin.x;
	dir.y = lasTarget.y - lasOrigin.y;
	dir.z = lasTarget.z - lasOrigin.z;
	return dir;
}

void DrawLaserSection(float lasLength ){
	float radiusRatio = 1.25;
	float glowRadiusRatio = 2.0;
	float curveRadiusRatio = 0.884; // sin(45) * radiusratio
	float lasDiam = LAS_DIAM;
	glPushMatrix();
		float i = 0.0;
		float segLength = 0.1;
		for(i = 0; i < lasLength ; i += segLength){
			lasDiam += lasDiam*((getRandom()/10.0));
			if(lasDiam > LAS_DIAM*3.0)
				lasDiam = LAS_DIAM*3.0;
			if(lasDiam < LAS_DIAM)
				lasDiam = LAS_DIAM;
			printf("%f\n",getRandom());
			glBegin(GL_QUAD_STRIP);
				glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
					glVertex3d(0.0,0.0-lasDiam*glowRadiusRatio,i);
					glVertex3d(0.0,0.0-lasDiam*glowRadiusRatio,segLength+i);
				glColor4f(0.7f, 0.0f, 0.0f, 0.7f);
					glVertex3d(0.0-lasDiam*curveRadiusRatio/2.0,0.0-lasDiam*radiusRatio,i);
					glVertex3d(0.0-lasDiam*curveRadiusRatio/2.0,0.0-lasDiam*radiusRatio,segLength+i);
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					glVertex3d(0.0-lasDiam*curveRadiusRatio,0.0-lasDiam,i);
					glVertex3d(0.0-lasDiam*curveRadiusRatio,0.0-lasDiam,segLength+i);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
					glVertex3d(0.0-lasDiam*radiusRatio,0.0,i);
					glVertex3d(0.0-lasDiam*radiusRatio,0.0,segLength+i);
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					glVertex3d(0.0-lasDiam*curveRadiusRatio,lasDiam,i);
					glVertex3d(0.0-lasDiam*curveRadiusRatio,lasDiam,segLength+i);
				glColor4f(0.7f, 0.0f, 0.0f, 0.7f);
					glVertex3d(0.0-lasDiam*curveRadiusRatio/2.0,lasDiam*radiusRatio,i);
					glVertex3d(0.0-lasDiam*curveRadiusRatio/2.0,lasDiam*radiusRatio,segLength+i);
				glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
					glVertex3d(0.0,lasDiam*glowRadiusRatio,i);
					glVertex3d(0.0,lasDiam*glowRadiusRatio,segLength+i);
			glEnd();
		}
	glPopMatrix();
}

void DrawLaser(){
	GLUquadric *q = gluNewQuadric();

	//lasEnd = lasTarget;

	int unBlocked = 0;
	if(partZProx < -100) // If laser is never blocked by any particles set it to go off infinitely
		unBlocked = 1;
	float infLength = 10;

	float lasLength = sqrt((lasEnd.x - lasOrigin.x)*(lasEnd.x - lasOrigin.x)
			+ (lasEnd.y - lasOrigin.y)*(lasEnd.y - lasOrigin.y)
			+ (lasEnd.z - lasOrigin.z)*(lasEnd.z - lasOrigin.z));

	// Align Laser to lasVector
	// Normal Vector found by simplified cross product: lasVector X Z-axis
	double n_X = - lasVector.y;
	double n_Y = lasVector.x;
	double n_Z = 0.0;

	// Find Rotation angle
	double dotProd = lasVector.z; // Simplififed dot product: lasVector DOT Z-Axis
	double magLasVec = sqrt(lasVector.x*lasVector.x + lasVector.y*lasVector.y + lasVector.z*lasVector.z);
	double lvAngle = acos(dotProd/magLasVec); // Angle ofrotation to align laser to lasVector
	lvAngle *= 57.29577951; // Convert rads to degrees
	lvAngle += 180; // Not sure why I need to do this but otherwise it ends up backwards...

	// Align Flat Laser to camera vector

	glPushMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glTranslatef(lasOrigin.x, lasOrigin.y, lasOrigin.z);
		glRotatef( lvAngle, n_X, n_Y, n_Z);
		glColor4f(0.5,0.5,0.5,1.0);
		gluDisk(q, 0.0, 0.03, 20, 20);
		gluCylinder(q,0.03,0.03, 0.05, 20, 20);
		glTranslatef(0.0, 0.0, 0.05);
		gluDisk(q, 0.0, 0.03, 20, 20);
		gluCylinder(q,0.02,0.02, 0.02, 20, 20);
		glTranslatef(0.0, 0.0, 0.02);
		gluCylinder(q,0.03,0.03, 0.05, 20, 20);
		gluDisk(q, 0.0, 0.03, 20, 20);
		glTranslatef(0.0, 0.0, 0.05);
		gluCylinder(q,0.03,0.00, 0.02, 20, 20);
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	glPopMatrix();

	if(laserOn ==0)
			return;

	glEnable (GL_BLEND);
	glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
		glDisable(GL_LIGHTING);

		glTranslatef(lasEnd.x, lasEnd.y, lasEnd.z);
		glRotatef( lvAngle, n_X, n_Y, n_Z);

		glPushMatrix();

//			glRotatef( 180, 0.0, 0.0, 1.0);
//
//			invertAb_Quat();
//			float* inv_quat = getInverseQuat();
//			printf("inv_quat: [%f %f %f %f] [%f %f %f %f] [%f %f %f %f] [%f %f %f %f]\n", inv_quat[0], inv_quat[1], inv_quat[2], inv_quat[3],
//						inv_quat[4], inv_quat[5], inv_quat[6] , inv_quat[7] , inv_quat[8], inv_quat[9], inv_quat[10] , inv_quat[11], inv_quat[12], inv_quat[13], inv_quat[14] , inv_quat[15]  );
//			inv_quat[2] = 0;
//			inv_quat[6] = 0;
//			inv_quat[8] = 0;
//			inv_quat[9] = 0;
//
//			float xLen = sqrt(inv_quat[0]*inv_quat[0] + inv_quat[1]*inv_quat[1]); // Singularity if either of these
//			float yLen = sqrt(inv_quat[4]*inv_quat[4] + inv_quat[5]*inv_quat[5]); //  is equal to zero.
//			if(xLen==0) xLen=1;
//			if(yLen==0) yLen=1;
//
//			inv_quat[0]/=xLen; inv_quat[1]/=xLen; inv_quat[2]=0; // Set the x column
//			inv_quat[4]/=yLen; inv_quat[5]/=yLen; inv_quat[6]=0; // Set the y column
//			inv_quat[8]=0; inv_quat[9]=0; inv_quat[10]=1;        // Set the z column
//
//			glMultMatrixf(inv_quat);

			glPushMatrix();
				DrawLaserSection(lasLength);
				if(unBlocked==1){
					glRotatef( 180, 1.0, 0.0, 0.0);
					DrawLaserSection(10.0);
				}
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
	glDisable(GL_BLEND);
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



//	glUseProgram(p);
	draw_scene();
//	glUseProgram(-1);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_NORMALIZE);
	glDisable(GL_AUTO_NORMAL);

	DrawLaser();
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

Point pickPoint(int x, int y){
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

	Point p = { .x=posX, .y=posY, .z=posZ };
	return p;
}

static void aimLaser(int x, int y){
	Point p = pickPoint(x,y);
	if(p.x > -1 && p.y > -1 && p.z > -1 && p.x < 1 && p.y < 1 && p.z < 1){ // Check point is reasonable
		lasTarget.x = p.x;
		lasTarget.y = p.y;
		lasTarget.z = p.z;
		lasVector = computeLaserDirection();
		laserOn = 1;
	} else
		laserOn = 0;
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
	if (state == GLUT_DOWN){
		if(button==GLUT_LEFT_BUTTON)
			click_scene(x, y);
		else if (button==GLUT_RIGHT_BUTTON){
			aimLaser(x,y);
		}
	}
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
///** I don't think this is working, but at least it compiles :) */
//void setShaders() {
//
//	char *vs, *fs;
//
//	v = glCreateShader(GL_VERTEX_SHADER);
//	f = glCreateShader(GL_FRAGMENT_SHADER);
//
//	vs = textFileRead("phong.vert");
//	fs = textFileRead("phong.frag");
//
//	const char * vv = vs;
//	const char * ff = fs;
//
//	glShaderSource(v, 1, &vv, NULL);
//	glShaderSource(f, 1, &ff, NULL);
//
//	free(vs);
//	free(fs);
//
//	glCompileShader(v);
//	glCompileShader(f);
//
//	p = glCreateProgram();
//	glAttachShader(p, v);
//	glAttachShader(p, f);
//
//	glLinkProgram(p);
//	//glUseProgram(p); // TODO
//
//}

int main(int argc, char ** argv) {
	arcball_reset();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(10, 10);
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);

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
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);

//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);

//	glewInit();
//
//	if (glewIsSupported("GL_VERSION_2_0"))
//		printf("Ready for OpenGL 2.0\n");
//	else {
//		printf("OpenGL 2.0 not supported\n");
//		exit(1);
//	}

	//setShaders();

	glutMainLoop();

	return 0; // never reached
}


