#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

#include <stack>
#include <list>

#include <glut.h>

#include <iostream>
using namespace std;

#define DEGREE 2*M_PI/180.
#define millis 1000/60

GLsizei v_size, n_size, f_size;
GLfloat *vertex, *normal;
GLuint **face_v, **face_n;
GLint *v_amount;

typedef struct CAM {
	GLfloat x, y, z,
		dx, dy, dz,
		ux, uy, uz;
}CAM;
stack<CAM> CAM_STACK;
CAM cam_init = { 10, 10, 0, -0.1, 0, 0, 0, 1, 0 }, cam = cam_init;

list<unsigned char> key_list;
list<int> s_key_list;

GLUquadricObj *q;
GLdouble angle_B = 0, angle_LA = 0, angle_UA = 0;


void MyKeyboard(unsigned char key, int x, int y) {
	key_list.push_back(key);
	key_list.unique();
}

void MyKeyboardUp(unsigned char key, int x, int y) {
	key_list.remove(key);
}

void MyChange(int value) {
	list<int>::iterator iter_s;
	for (iter_s = s_key_list.begin(); iter_s != s_key_list.end(); ++iter_s) {
		GLfloat dx_t = cam.dx, dy_t = cam.dy, dz_t = cam.dz;
		switch (*iter_s) {
		case GLUT_KEY_LEFT:
			cam.dx = dx_t * cos(DEGREE) + dz_t * sin(DEGREE);
			cam.dz = -dx_t * sin(DEGREE) + dz_t * cos(DEGREE);
			break;
		case GLUT_KEY_RIGHT:
			cam.dx = dx_t * cos(DEGREE) - dz_t * sin(DEGREE);
			cam.dz = dx_t * sin(DEGREE) + dz_t * cos(DEGREE);
			break;
		case GLUT_KEY_UP:
			cam.x += cam.dx; cam.y += cam.dy; cam.z += cam.dz;
			break;
		case GLUT_KEY_DOWN:
			cam.x -= cam.dx; cam.y -= cam.dy; cam.z -= cam.dz;
			break;
		case GLUT_KEY_PAGE_UP:
			cam.y += 0.1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			cam.y -= 0.1;
			break;
		}
	}

	list<unsigned char>::iterator iter;
	for (iter = key_list.begin(); iter != key_list.end(); ++iter) {
		switch (*iter) {
		case ' ':
			CAM_STACK.push(cam);
			break;
		case '\b':
			if (!CAM_STACK.empty()) {
				cam = CAM_STACK.top();
				CAM_STACK.pop();
			}
			break;
		case 27:
			while (!CAM_STACK.empty()) {
				CAM_STACK.pop();
			}
			cam = cam_init;
			break;
		case 'a':
			angle_B += 1;
			break;
		case 'd':
			angle_B -= 1;
			break;
		case 'w':
			angle_LA += 1;
			if (angle_LA >= 60) angle_LA = 60;
			break;
		case 's':
			angle_LA -= 1;
			if (angle_LA <= -60) angle_LA = -60;
			break;
		case 'q':
			angle_UA += 1;
			if (angle_UA >= 120) angle_UA = 120;
			break;
		case 'e':
			angle_UA -= 1;
			if (angle_UA <= -120) angle_UA = -120;
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(millis, MyChange, 0);
}

void MySpecial(int key, int x, int y) {
	s_key_list.push_back(key);
	s_key_list.unique();
}

void MySpecialUp(int key, int x, int y) {
	s_key_list.remove(key);
}

void Draw_Axes(GLdouble length) {
	GLdouble cur_col[4];
	glGetDoublev(GL_CURRENT_COLOR, cur_col);

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(-length, 0, 0);
	glVertex3d(length, 0, 0);

	glColor3d(0, 1, 0);
	glVertex3d(0, -length, 0);
	glVertex3d(0, length, 0);

	glColor3d(0, 0, 1);
	glVertex3d(0, 0, -length);
	glVertex3d(0, 0, length);
	glEnd();

	glColor4dv(cur_col);
}

void Draw_Base() {
	//glPushMatrix();
	glRotated(-90, 1, 0, 0);
	glRotated(angle_B, 0, 0, 1);
	glPushMatrix();

	gluCylinder(q, 5, 5, 3, 360, 30);

	gluDisk(q, 0, 5, 360, 10);

	glTranslated(0, 0, 3);
	gluDisk(q, 0, 5, 360, 10);

	glPopMatrix();
	Draw_Axes(5);
	//glPopMatrix();
}

void Draw_Lower_Arm() {
	//glPushMatrix();
	glTranslated(0, 0, 3);
	//glRotated(-90, 1, 0, 0);
	glRotated(45, 0, 0, 1);

	//glRotated(angle_B, 0, 0, 1);
	glRotated(angle_LA, 1, 1, 0);
	glPushMatrix();

	gluCylinder(q, 1.5, 1.5, 10, 4, 50);

	
	glTranslated(0, 0, 11);
	glRotated(45, 0, 0, 1);

	glPushMatrix();
	glScaled(0.5, 2.5, 3);

	glPushMatrix();
	glTranslated(2, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-2, 0, 0);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
	Draw_Axes(5);
	//glPopMatrix();
}

void Draw_Upper_Arm() {
	glTranslated(0, 0, 11);
	glTranslated(-0.75, -0.75, 0.5);
	glRotated(angle_UA, 1, 1, 0);

	glPushMatrix();
	
	glRotated(90, 0, 1, 0);
	glRotated(-45, 1, 0, 0);
	gluCylinder(q, 0.3, 0.3, 2, 360, 10);
	glPopMatrix();

	//glPushMatrix();
	
	//glRotated(-90, 1, 0, 0);
	//glRotated(45, 0, 0, 1);
	
	glPushMatrix();

	glTranslated(0.75, 0.75, -0.5);
	gluCylinder(q, 1, 1, 10, 4, 50);

	glPopMatrix();
	Draw_Axes(5);
	//glPopMatrix();
}

void Draw_Robot_Arm(
	GLdouble R, GLdouble G, GLdouble B,
	GLdouble SX, GLdouble SY, GLdouble SZ,
	GLdouble AX, GLdouble AY, GLdouble AZ,
	GLdouble X, GLdouble Y, GLdouble Z
) {
	glPushMatrix();
	glColor3d(R, G, B);

	glTranslated(X, Y, Z);
	glScaled(SX, SY, SZ);
	glRotated(AX, 1, 0, 0);
	glRotated(AY, 0, 1, 0);
	glRotated(AZ, 0, 0, 1);


	Draw_Base();
	Draw_Lower_Arm();
	Draw_Upper_Arm();

	glPopMatrix();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(cam.x, cam.y, cam.z, cam.x + cam.dx, cam.y + cam.dy, cam.z + cam.dz, 0, 1, 0);

	Draw_Axes(50);

	Draw_Robot_Arm(0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0);

	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("·Îº¿ ÆÈ");

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0, 5);

	glMatrixMode(GL_MODELVIEW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	gluQuadricOrientation(q, GLU_OUTSIDE);
	gluQuadricTexture(q, GL_FALSE);

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutKeyboardUpFunc(MyKeyboardUp);
	glutSpecialFunc(MySpecial);
	glutSpecialUpFunc(MySpecialUp);

	glutTimerFunc(millis, MyChange, 0);

	glutMainLoop();
	gluDeleteQuadric(q);
	return 0;
}