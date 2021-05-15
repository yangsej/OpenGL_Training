#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <glut.h>
#include <iostream>
using namespace std;

typedef void (*func)(int value);
func function = NULL;
bool stop = false;
GLint value_temp = 0, vertical = 0;

GLfloat figure[4][2] = { {-300, 150}, {-300, -150}, {300, -150}, {300, 150} };
float degree = 0.0, move_d = 0.0, zoom_d = 0.0;
int msec = 15;
GLubyte color[3] = { 0,0,0 };


void MyFreeChange(int value) {
	if (stop || function != MyFreeChange) return;
	else {
		color[0] = rand() % 256;
		color[1] = rand() % 256;
		color[2] = rand() % 256;
	}
	glutPostRedisplay();
	glutTimerFunc(msec, MyFreeChange, value_temp);
}

void MyRotate(int value) {
	if (stop || function != MyRotate) return;
	else{
		GLfloat post[4][2];
		for (int i = 0; i < 4; i++) {
			post[i][0] = figure[i][0];
			post[i][1] = figure[i][1];
		}

		GLint MidX, MidY;
		MidX = (figure[0][0] + figure[1][0] + figure[2][0] + figure[3][0]) / 4;
		MidY = (figure[0][1] + figure[1][1] + figure[2][1] + figure[3][1]) / 4;

		GLdouble sin_val = sin(degree), cos_val = cos(degree);
		
		figure[0][0] = ((post[0][0] - MidX) * cos_val
			- (post[0][1] - MidY) * sin_val) + MidX;
		figure[1][0] = ((post[1][0] - MidX) * cos_val
			- (post[1][1] - MidY) * sin_val) + MidX;
		figure[2][0] = ((post[2][0] - MidX) * cos_val
			- (post[2][1] - MidY) * sin_val) + MidX;
		figure[3][0] = ((post[3][0] - MidX) * cos_val
			- (post[3][1] - MidY) * sin_val) + MidX;

		figure[0][1] = ((post[0][0] - MidX) * sin_val
			+ (post[0][1] - MidY) * cos_val) + MidY;
		figure[1][1] = ((post[1][0] - MidX) * sin_val
			+ (post[1][1] - MidY) * cos_val) + MidY;
		figure[2][1] = ((post[2][0] - MidX) * sin_val
			+ (post[2][1] - MidY) * cos_val) + MidY;
		figure[3][1] = ((post[3][0] - MidX) * sin_val
			+ (post[3][1] - MidY) * cos_val) + MidY;
	}
	glutPostRedisplay();
	glutTimerFunc(msec, MyRotate, value_temp);
}

void MyScale(int value) {
	if (stop || function != MyScale) return;
	else {
		figure[0][0] *= zoom_d;
		figure[1][0] *= zoom_d;
		figure[2][0] *= zoom_d;
		figure[3][0] *= zoom_d;

		figure[0][1] *= zoom_d;
		figure[1][1] *= zoom_d;
		figure[2][1] *= zoom_d;
		figure[3][1] *= zoom_d;
	}
	glutPostRedisplay();
	glutTimerFunc(msec, MyScale, value_temp);
}

void MyMove(int value) {
	if (stop || function != MyMove) return;
	else{
		figure[0][value] += move_d;
		figure[1][value] += move_d;
		figure[2][value] += move_d;
		figure[3][value] += move_d;
	}
	glutPostRedisplay();
	glutTimerFunc(msec, MyMove, value_temp);
}

void MyRotateMenu(int entryID) {
	if (entryID == 1) {
		degree = M_PI / 180;
	}
	else if (entryID == 2) {
		degree = -M_PI / 180;
	} else {
		function = NULL;
		return;
	}
	if (function == MyRotate) return;
	function = MyRotate;
	glutTimerFunc(msec, MyRotate, value_temp);
	stop = false;
}

void MyScaleMenu(int entryID) {
	if (entryID == 1) {
		zoom_d = 1.001;
	}
	else if (entryID == 2) {
		zoom_d = 0.999;
	}
	else {
		function = NULL;
		return;
	}
	if (function == MyScale) return;
	function = MyScale;
	glutTimerFunc(msec, MyScale, value_temp);
	stop = false;
}

void MyMoveMenu(int entryID) {
	if (entryID == 1 || entryID == 4) {
		move_d = 1.0;
	}
	else if (entryID == 2 || entryID == 3) {
		move_d = -1.0;
	}
	else {
		function = NULL;
		return;
	}
	if (entryID == 1 || entryID == 2) value_temp = 1;
	else value_temp = 0;
	if (function == MyMove) return;
	function = MyMove;
	glutTimerFunc(msec, MyMove, value_temp);
	stop = false;
}

void MyMainMenu(int entryID) {
	switch (entryID)
	{
	case 0:
		exit(0);
		break;
	case 1:
		if (function == MyFreeChange) return;
		function = MyFreeChange;
		glutTimerFunc(msec, MyFreeChange, value_temp);
		stop = false;
		break;
	}
}

void MyMenuState(int status) {
	if (status == GLUT_MENU_IN_USE) {
		stop = true;
	}
	else if (status == GLUT_MENU_NOT_IN_USE) {
		stop = false;
		glutTimerFunc(msec, function, value_temp);
	}
}

void MyMenus() {
	GLint MyRotateMenuID = glutCreateMenu(MyRotateMenu);
	glutAddMenuEntry("선택 취소", 0);
	glutAddMenuEntry("시계 방향", 1);
	glutAddMenuEntry("반시계 방향", 2);

	GLint MyZoomMenuID = glutCreateMenu(MyScaleMenu);
	glutAddMenuEntry("선택 취소", 0);
	glutAddMenuEntry("확대", 1);
	glutAddMenuEntry("축소", 2);

	GLint MyMoveMenuID = glutCreateMenu(MyMoveMenu);
	glutAddMenuEntry("선택 취소", 0);
	glutAddMenuEntry("상", 1);
	glutAddMenuEntry("하", 2);
	glutAddMenuEntry("좌", 3);
	glutAddMenuEntry("우", 4);

	GLint MyMainMenuID = glutCreateMenu(MyMainMenu);
	glutAddSubMenu("회전", MyRotateMenuID);
	glutAddSubMenu("스케일링", MyZoomMenuID);
	glutAddSubMenu("이동", MyMoveMenuID);
	glutAddMenuEntry("반짝", 1);
	glutAddMenuEntry("프로그램 종료", 0);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMenuStateFunc(MyMenuState);
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case 's':
		stop = !stop;
		if (!stop) {
			glutTimerFunc(msec, function, value_temp);
		}
		break;
	case 27:
		exit(0); break;
	}
}

// 출력 콜백 함수
void MyDisplay() {
	glViewport(0, 0, 1000, 1000);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-500.0, 500.0, -500.0, 500.0, -1.0, 1.0);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(figure[0][0], figure[0][1]);
	glVertex2f(figure[1][0], figure[1][1]);
	glVertex2f(figure[2][0], figure[2][1]);
	glVertex2f(figure[3][0], figure[3][1]);
	glEnd();
	glFlush();
}

int main(int argc, char** argv) {
	srand((unsigned)time(NULL));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("그림판");

	MyMenus();

	glutDisplayFunc(MyDisplay);
	glutKeyboardFunc(MyKeyboard);
	glutMainLoop();
	return 0;
}