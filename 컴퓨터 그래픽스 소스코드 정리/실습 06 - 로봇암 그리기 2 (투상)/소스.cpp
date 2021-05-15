#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

#include <list>

#include <glut.h>

#include <iostream>
using namespace std;

#define DEGREE 2*M_PI/180.
#define millis 1000/60

int WIDTH = 1500, HEIGHT = 1000;

GLdouble robot_x = 0, robot_y = 0, robot_z = 0;

typedef struct CAM {
	GLdouble x, y, z,
		dx, dy, dz,
		ux, uy, uz;
}CAM;
GLdouble robot_cam_x = -1, robot_cam_y = 3, robot_cam_z = 0, robot_cam_a=0, robot_cam_f=90;
CAM cam_init = { robot_x + robot_cam_x, robot_y + robot_cam_y, robot_z + robot_cam_z, -0.1, 0, 0, 0, 1, 0 }, cam = cam_init;

list<unsigned char> key_list;
list<int> s_key_list;

void Rotation_Fomula(double angle, double nx, double ny, double nz,
	double &x, double &y, double &z) {
	double xt = x, yt = y, zt = z;
	double theta = angle * DEGREE;
	x = (cos(theta) - (cos(theta) - 1) * pow(nx, 2)) * xt
		+ ((1 - cos(theta)) * nx * ny - sin(theta) * nz) * yt
		+ (sin(theta) * ny - (cos(theta) - 1) * nx * nz) * zt;
	y = ((1 - cos(theta)) * nx * ny + sin(theta) * nz) * xt
		+ (cos(theta) - (cos(theta) - 1) * pow(ny, 2)) * yt
		+ (-sin(theta) * nx - (cos(theta) - 1) * ny * nz) * zt;
	z = (-sin(theta) * ny - (cos(theta) - 1) * nx * nz) * xt
		+ (sin(theta) * nx - (cos(theta) - 1) * ny * nz) * yt
		+ (cos(theta) - (cos(theta) - 1) * pow(nz, 2)) * zt;
}

void MyKeyboard(unsigned char key, int x, int y) {
	key_list.push_back(key);
	key_list.unique();
}

void MyKeyboardUp(unsigned char key, int x, int y) {
	key_list.remove(key);
}

void MyChange(int value) {
	CAM cam_temp = cam;
	for (list<unsigned char>::iterator iter = key_list.begin(); iter != key_list.end(); ++iter) {
		GLdouble nx = cam.dy * cam.uz - cam.dz * cam.uy,
			ny = -cam.dx * cam.uz + cam.dz * cam.ux,
			nz = cam.dx * cam.uy - cam.dy * cam.ux;
		GLdouble nlength = sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2));
		nx /= nlength; ny /= nlength; nz /= nlength;
		switch (*iter) {
		case 'z':
			if (robot_cam_f <= 140) robot_cam_f++;
			break;
		case 'q':
			if (robot_cam_f >= 40) robot_cam_f--;
			break;
		case 'w':
			Rotation_Fomula(1, nx, ny, nz, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(1, nx, ny, nz, cam.ux, cam.uy, cam.uz);
			break;
		case 'x':
			Rotation_Fomula(-1, nx, ny, nz, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(-1, nx, ny, nz, cam.ux, cam.uy, cam.uz);
			break;
		}
	}

	for (list<int>::iterator iter = s_key_list.begin(); iter != s_key_list.end(); ++iter) {
		GLfloat dx_t = cam.dx, dy_t = cam.dy, dz_t = cam.dz,
			rc_x = robot_cam_x, rc_y = robot_cam_y, rc_z = robot_cam_z;
		switch (*iter) {
		case GLUT_KEY_LEFT:
			robot_cam_a += 2;
			Rotation_Fomula(1, 0, 1, 0, robot_cam_x, robot_cam_y, robot_cam_z);
			Rotation_Fomula(1, 0, 1, 0, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(1, 0, 1, 0, cam.ux, cam.uy, cam.uz);
			break;
		case GLUT_KEY_RIGHT:
			robot_cam_a -= 2;
			Rotation_Fomula(-1, 0, 1, 0, robot_cam_x, robot_cam_y, robot_cam_z);
			Rotation_Fomula(-1, 0, 1, 0, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(-1, 0, 1, 0, cam.ux, cam.uy, cam.uz);
			break;
		case GLUT_KEY_UP:
			robot_x += 0.1 * robot_cam_x; robot_z += 0.1 * robot_cam_z;
			cam.x += 0.1 * robot_cam_x; cam.z += 0.1 * robot_cam_z;
			/*robot_x += cam.dx; robot_z += cam.dz;
			cam.x += cam.dx; cam.y += cam.dy; cam.z += cam.dz;*/
			break;
		case GLUT_KEY_DOWN:
			robot_x -= 0.1 * robot_cam_x; robot_z -= 0.1 * robot_cam_z;
			cam.x -= 0.1 * robot_cam_x; cam.z -= 0.1 * robot_cam_z;
			/*robot_x -= cam.dx; robot_z -= cam.dz;
			cam.x -= cam.dx; cam.y -= cam.dy; cam.z -= cam.dz;*/
			break;
		case GLUT_KEY_PAGE_UP:
			cam.y += 0.1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			cam.y -= 0.1;
			break;
		}
		cam = { robot_x + robot_cam_x, robot_y + robot_cam_y, robot_z + robot_cam_z, cam.dx, cam.dy, cam.dz, cam.ux, cam.uy, cam.uz };
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
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_LINE);

	glPushMatrix();
	glRotated(-90, 1, 0, 0);
	//Draw_Axes(5);

	gluDisk(q, 10, 10, 4, 1);
	gluCylinder(q, 5, 5, 3, 50, 1);

	gluDisk(q, 0, 5, 360, 1);
	glTranslated(0, 0, 3);
	gluDisk(q, 0, 5, 360, 1);

	glPopMatrix();
	gluDeleteQuadric(q);
}

void Draw_Lower_Arm() {
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_LINE);

	glPushMatrix();
	glTranslated(0, 8, 0);

	glPushMatrix();
	glScaled(2, 10, 2);
	glutWireCube(1);
	glPopMatrix();

	glTranslated(0, 6, 0);
	glScaled(2, 2, 0.5);

	glPushMatrix();
	glTranslated(0, 0, 1.5);
	glutWireCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, -1.5);
	glutWireCube(1);
	glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(q);
}

void Draw_Upper_Arm() {
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_LINE);

	glPushMatrix();
	glTranslated(0, 19, 0);

	glPushMatrix();
	glScaled(1, 10, 1);
	glutWireCube(1);
	glPopMatrix();

	glPopMatrix();

	gluDeleteQuadric(q);
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

void Draw_Robot() {
	glPushMatrix();
	glColor3d(1, 0.5, 0.5);

	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_LINE);

	glTranslated(robot_x, robot_y, robot_z);
	glRotated(-90, 1, 0, 0);
	glRotated(90 + robot_cam_a, 0, 0, 1);
	glScaled(1, 2, 1);
	gluCylinder(q, 1, 1, 2, 3, 1);

	gluDisk(q, 0, 1, 3, 1);
	glTranslated(0, 0, 2);
	gluDisk(q, 0, 1, 3, 1);

	glPopMatrix();
	gluDeleteQuadric(q);
}

void Draw_scene() {
	glPushMatrix();
	Draw_Axes(50);
	Draw_Robot_Arm(0.5, 1, 0.5, 0.3, 0.3, 0.3, 0, 0, 0, 0, 0, 0);
	Draw_Robot();
	glPopMatrix();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, WIDTH / 3. * 2, HEIGHT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(robot_cam_f, WIDTH / 3. * 2 / HEIGHT, 0, 5);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cam.x, cam.y, cam.z, cam.x + cam.dx, cam.y + cam.dy, cam.z + cam.dz, cam.ux, cam.uy, cam.uz);
		Draw_scene();

	glViewport(WIDTH / 3. * 2, HEIGHT / 2., WIDTH / 3., HEIGHT / 2.);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-5 * (WIDTH / 3.) / (HEIGHT / 2.), 5 * (WIDTH / 3.) / (HEIGHT / 2.), -5, 5, 0, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		gluLookAt(0, 10, 0, 0, 0, 0, 1, 0, 0);
		Draw_scene();

	glViewport(WIDTH / 3. * 2, 0, WIDTH / 3., HEIGHT / 2.);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-0.5 * (WIDTH / 3.) / (HEIGHT / 2.), 0.5 * (WIDTH / 3.) / (HEIGHT / 2.), -0.5, 0.5, 1, 100);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);
		Draw_scene();

	glFlush();
}

void MyReshape(int width, int height) {
	WIDTH = width; HEIGHT = height;
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("·Îº¿ ÆÈ");

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKeyboard);
	glutKeyboardUpFunc(MyKeyboardUp);
	glutSpecialFunc(MySpecial);
	glutSpecialUpFunc(MySpecialUp);

	glutTimerFunc(millis, MyChange, 0);

	glutMainLoop();
	return 0;
}