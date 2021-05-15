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
CAM cam_init = { 0, 0.1, 0, 0.1, 0, 0, 0, 2, 0 }, cam = cam_init;

list<int> s_key_list;

void Read_3VN(char* dir) {
	fstream fp;

	fp.open(dir, ios::in);
	fp >> v_size >> n_size >> f_size;

	vertex = new GLfloat[3 * v_size];
	for (int i = 0; i<v_size; i++) {
		fp >> vertex[3 * i] >> vertex[3 * i + 1] >> vertex[3 * i + 2];
	}

	normal = new GLfloat[3 * n_size];
	for (int i = 0; i<n_size; i++) {
		fp >> normal[3 * i] >> normal[3 * i + 1] >> normal[3 * i + 2];
	}

	face_v = new GLuint*[f_size];
	face_n = new GLuint*[f_size];
	v_amount = new GLint[f_size];
	for (int i = 0; i<f_size; i++) {
		fp >> v_amount[i];
		face_v[i] = new GLuint[v_amount[i]];
		face_n[i] = new GLuint[v_amount[i]];
		for (int j = 0; j<v_amount[i]; j++)
			fp >> face_v[i][j];
		for (int j = 0; j<v_amount[i]; j++)
			fp >> face_n[i][j];
	}

	fp.close();
}

void MyKeyboard(unsigned char key, int x, int y) {
	switch (key) {
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
	}
	glutPostRedisplay();
}

void MySpecial(int key, int x, int y) {
	s_key_list.push_back(key);
	s_key_list.unique();
}

void MySpecialUp(int key, int x, int y) {
	s_key_list.remove(key);
}

void MyChange(int value) {
	list<int>::iterator iter;
	for (iter = s_key_list.begin(); iter != s_key_list.end(); ++iter) {
		GLfloat dx_t = cam.dx, dy_t = cam.dy, dz_t = cam.dz;
		switch (*iter) {
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
		}
	}
	glutPostRedisplay();
	glutTimerFunc(millis, MyChange, 0);
}

void Instance_Barn(GLfloat R, GLfloat G, GLfloat B, GLfloat SX, GLfloat SY, GLfloat SZ,
	GLint AX, GLint AY, GLint AZ, GLfloat X, GLfloat Y, GLfloat Z) // 함수 정의 
{
	glPushMatrix();

	glColor3f(R, G, B);
	glTranslatef(X, Y, Z);
	glRotatef(AX, 1, 0, 0);
	glRotatef(AY, 0, 1, 0);
	glRotatef(AZ, 0, 0, 1);
	glScalef(SX, SY, SZ);
	for (int i = 0; i<f_size; i++)
		glDrawElements(GL_POLYGON, v_amount[i], GL_UNSIGNED_INT, face_v[i]);

	glPopMatrix();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	gluPerspective(90, 1, 0, 5);

	gluLookAt(cam.x, cam.y, cam.z, cam.x + cam.dx, cam.y + cam.dy, cam.z + cam.dz, 0, cam.y + 1, 0);


	glBegin(GL_QUADS);
	glColor3f(0, 1, 0);
	glVertex3f(-50, 0, 50);
	glVertex3f(-50, 0, -50);
	glVertex3f(50, 0, -50);
	glVertex3f(50, 0, 50);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(-50, 0, 0);
	glVertex3f(50, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, -50, 0);
	glVertex3f(0, 50, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, -50);
	glVertex3f(0, 0, 50);
	glEnd();


	Instance_Barn(1, 1, 0, 5, 1, 1, 0, 30, 0, 3, 0, 3);
	Instance_Barn(1, 0, 1, 1, 5, 1, 0, 60, 0, -3, 0, 3);
	Instance_Barn(0, 1, 1, 1, 1, 5, 0, 90, 0, -3, 0, -3);
	Instance_Barn(0, 0, 0, 5, 5, 5, 0, 120, 0, 3, 0, -3);
	Instance_Barn(1, 0, 0, 0.5, 0.5, 0.5, 0, 150, 0, 3, 0, 0);

	glFlush();
}

int main(int argc, char** argv) {
	Read_3VN("basicBarn.3vn");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("마을");

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluPerspective(90, 1, 0, 5);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutSpecialUpFunc(MySpecialUp);

	glutTimerFunc(millis, MyChange, 0);

	glutMainLoop();
	return 0;
}

