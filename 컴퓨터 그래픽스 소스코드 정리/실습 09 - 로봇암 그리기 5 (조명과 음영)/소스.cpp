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
GLdouble robot_cam_x = -1, robot_cam_y = 3, robot_cam_z = 0, robot_cam_a = 0, robot_cam_f = 90;
CAM cam_init = { robot_x + robot_cam_x, robot_y + robot_cam_y, robot_z + robot_cam_z, -1, 0, 0, 0, 1, 0 }, cam = cam_init;

list<unsigned char> key_list;
list<int> s_key_list;

GLdouble angle_B = 0, angle_LA = 0, angle_UA = 0;
GLenum cull_mode = GL_BACK;

GLdouble spotlight_R = 0.7, spotlight_G = 0.0, spotlight_B = 0.0;
GLboolean lighting = true, light0 = true, light1 = true;

// 회전변환
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

// 키보드 입력을 통한 변환
void MyKeyboardChange(int value) {
	CAM cam_temp = cam;
	for (list<unsigned char>::iterator iter = key_list.begin(); iter != key_list.end(); ++iter) {
		GLdouble nx = cam.dy * cam.uz - cam.dz * cam.uy,
			ny = -cam.dx * cam.uz + cam.dz * cam.ux,
			nz = cam.dx * cam.uy - cam.dy * cam.ux;
		GLdouble nlength = sqrt(pow(nx, 2) + pow(ny, 2) + pow(nz, 2));
		nx /= nlength; ny /= nlength; nz /= nlength;
		switch (*iter) {
		// 줌
		case 'z':
			if (robot_cam_f <= 140) robot_cam_f++;
			break;
		case 'q':
			if (robot_cam_f >= 40) robot_cam_f--;
			break;
		// 틸팅
		case 'w':
			Rotation_Fomula(1, nx, ny, nz, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(1, nx, ny, nz, cam.ux, cam.uy, cam.uz);
			break;
		case 'x':
			Rotation_Fomula(-1, nx, ny, nz, cam.dx, cam.dy, cam.dz);
			Rotation_Fomula(-1, nx, ny, nz, cam.ux, cam.uy, cam.uz);
			break;
		// 로봇암 베이스 회전
		case '4':
			angle_B += 1;
			break;
		case '6':
			angle_B -= 1;
			break;
		// 로봇암 아래쪽 팔 회전
		case '1':
			angle_LA += 1;
			if (angle_LA >= 60) angle_LA = 60;
			break;
		case '7':
			angle_LA -= 1;
			if (angle_LA <= -60) angle_LA = -60;
			break;
		// 로봇암 위쪽 팔 회전
		case '3':
			angle_UA += 1;
			if (angle_UA >= 120) angle_UA = 120;
			break;
		case '9':
			angle_UA -= 1;
			if (angle_UA <= -120) angle_UA = -120;
			break;
		}
	}
	glutPostRedisplay();
	if (!key_list.empty()) glutTimerFunc(millis, MyKeyboardChange, 0);
}

// 키보드 특수키 입력을 통한 변환
void MySpecialChange(int value){
	for (list<int>::iterator iter = s_key_list.begin(); iter != s_key_list.end(); ++iter) {
		GLfloat dx_t = cam.dx, dy_t = cam.dy, dz_t = cam.dz,
			rc_x = robot_cam_x, rc_y = robot_cam_y, rc_z = robot_cam_z;
		switch (*iter) {
		// 탐사로봇 이동
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
			break;
		case GLUT_KEY_DOWN:
			robot_x -= 0.1 * robot_cam_x; robot_z -= 0.1 * robot_cam_z;
			cam.x -= 0.1 * robot_cam_x; cam.z -= 0.1 * robot_cam_z;
			break;
		case GLUT_KEY_PAGE_UP:
			robot_y += 0.1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			robot_y -= 0.1;
			break;
		}
		cam.x = robot_x + robot_cam_x; cam.y = robot_y + robot_cam_y; cam.z = robot_z + robot_cam_z;
	}
	glutPostRedisplay();
	if (!s_key_list.empty()) glutTimerFunc(millis, MySpecialChange, 0);
}

// 키보드 입력
void MyKeyboard(unsigned char key, int x, int y) {
	if (key_list.empty()) glutTimerFunc(millis, MyKeyboardChange, 0);
	key_list.push_back(key);
	key_list.unique();

	switch (key) {
	case '`': // 가시성 변경
		switch (cull_mode) {
		case GL_BACK:
			cull_mode = GL_FRONT;
			break;
		case GL_FRONT:
			cull_mode = GL_FRONT_AND_BACK;
			break;
		case GL_FRONT_AND_BACK:
			cull_mode = GL_BACK;
			break;
		}
		break;
	case '\t': // 스포트라이트 색상 변경
		if (spotlight_R && !spotlight_G && !spotlight_B) {
			spotlight_R = 0; spotlight_G = 0; spotlight_B = 0.7;
		}
		else if (!spotlight_R && !spotlight_G && spotlight_B) {
			spotlight_R = 0.7; spotlight_G = 0.7; spotlight_B = 0;
		}
		else if (spotlight_R && spotlight_G && !spotlight_B) {
			spotlight_R = 0.7; spotlight_G = 0; spotlight_B = 0;
		}
		break;
	case 'l': // 음영 설정
		lighting = !lighting;
		if (lighting) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
		break;
	case ';': // 광원0 설정
		light0 = !light0;
		if (light0) glEnable(GL_LIGHT0);
		else glDisable(GL_LIGHT0);
		break;
	case '\'': // 광원1 설정
		light1 = !light1;
		if (light1) glEnable(GL_LIGHT1);
		else glDisable(GL_LIGHT1);
		break;
	}
}

// 키보드 입력 해제
void MyKeyboardUp(unsigned char key, int x, int y) {
	key_list.remove(key);
}

// 키보드 특수키 입력
void MySpecial(int key, int x, int y) {
	if (s_key_list.empty()) glutTimerFunc(millis, MySpecialChange, 0);
	s_key_list.push_back(key);
	s_key_list.unique();
}

// 키보드 특수키 입력 해제
void MySpecialUp(int key, int x, int y) {
	s_key_list.remove(key);
}

// 좌표계
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

// 로봇암 베이스
void Draw_Base(GLboolean reverse) {
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glRotated(-90, 1, 0, 0);
	if (reverse) glRotated(-angle_B, 0, 0, 1);
	else glRotated(angle_B, 0, 0, 1);
	glPushMatrix();

	gluCylinder(q, 5, 5, 3, 360, 100);

	glPushMatrix();
	glRotated(180, 1, 0, 0);
	gluDisk(q, 0, 5, 360, 100);

	glPopMatrix();
	glTranslated(0, 0, 3);
	gluDisk(q, 0, 5, 360, 100);

	glPopMatrix();
	Draw_Axes(5);
	gluDeleteQuadric(q);
}

// 로봇암 아래쪽 팔
void Draw_Lower_Arm(GLboolean reverse) {
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glTranslated(0, 0, 3);
	glRotated(45, 0, 0, 1);

	if (reverse) glRotated(-angle_LA, 1, 1, 0);
	else glRotated(angle_LA, 1, 1, 0);
	glPushMatrix();

	gluCylinder(q, 1.5, 1.5, 10, 4, 100);


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
	gluDeleteQuadric(q);
}

// 로봇암 위쪽 팔
void Draw_Upper_Arm(GLboolean reverse) {
	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glTranslated(0, 0, 11);
	glTranslated(-0.75, -0.75, 0.5);
	if (reverse) glRotated(-angle_UA, 1, 1, 0);
	else glRotated(angle_UA, 1, 1, 0);

	glPushMatrix();

	glRotated(90, 0, 1, 0);
	glRotated(-45, 1, 0, 0);
	gluCylinder(q, 0.3, 0.3, 2, 360, 100);
	glPopMatrix();

	glPushMatrix();

	glTranslated(0.75, 0.75, -0.5);
	gluCylinder(q, 1, 1, 10, 4, 100);

	glPopMatrix();
	Draw_Axes(5);

	gluDeleteQuadric(q);
}

// 로봇암
void Draw_Robot_Arm(
	GLdouble R, GLdouble G, GLdouble B,
	GLdouble SX, GLdouble SY, GLdouble SZ,
	GLdouble AX, GLdouble AY, GLdouble AZ,
	GLdouble X, GLdouble Y, GLdouble Z,
	GLboolean reverse = false
) {
	glPushMatrix();
	glColor3d(R, G, B);
	// 물체 재질 설정
	GLfloat
		material_ambient[]		= { R / 3., G / 3., B / 3., 1 },
		material_diffuse[]		= { R, G, B, 1 },
		material_specular[]		= { R, G, B, 1 },
		material_shininess[]	= { 64 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glTranslated(X, Y, Z);
	glScaled(SX, SY, SZ);
	glRotated(AX, 1, 0, 0);
	glRotated(AY, 0, 1, 0);
	glRotated(AZ, 0, 0, 1);

	Draw_Base(reverse);
	Draw_Lower_Arm(reverse);
	Draw_Upper_Arm(reverse);

	glPopMatrix();
}

// 탐사로봇
void Draw_Robot() {
	glPushMatrix();
	glColor3d(1, 0.5, 0.5);
	// 물체 재질
	GLfloat
		material_ambient[]		= { 1 / 3., 0.5 / 3., 0.5 / 3., 1 },
		material_diffuse[]		= { 1, 0.5, 0.5, 1 },
		material_specular[]		= { 0.5, 0.5, 0.5, 1 },
		material_shininess[]	= { 32 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	GLUquadricObj *q;
	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);

	glTranslated(robot_x, robot_y, robot_z);
	glRotated(-90, 1, 0, 0);
	glRotated(90 + robot_cam_a, 0, 0, 1);
	glScaled(1, 2, 1);
	gluCylinder(q, 1, 1, 2, 3, 100);

	gluDisk(q, 0, 1, 3, 100);
	glTranslated(0, 0, 2);
	glRotated(180, 0, 1, 0);
	gluDisk(q, 0, 1, 3, 100);

	gluDeleteQuadric(q);
	glPopMatrix();
}

// 빛 설정
void Set_light() {
	GLfloat
		light1_ambient[] = { spotlight_R / 3., spotlight_G / 3., spotlight_B / 3., 1 },
		light1_diffuse[] = { spotlight_R, spotlight_G, spotlight_B, 1 },
		light1_specular[] = { spotlight_R, spotlight_G, spotlight_B, 1 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

	GLfloat light0_position[] = { -10, 10, 0, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	GLfloat
		light1_position[] = { cam.x, cam.y, cam.z, 1 },
		light1_direction[] = { cam.dx, cam.dy, cam.dz },
		light1_spot_angle[] = { 10 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, light1_spot_angle);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1);
}

// 장면 그리기
void Draw_scene() {
	glPushMatrix();
	Draw_Axes(50);

	Set_light();

	Draw_Robot_Arm(0.5, 1, 0.5, 2, 2, 2, 0, 0, 0, 0, 0, 0);
	Draw_Robot_Arm(1, 0.5, 0.5, 1.5, 1.5, 1.5, 0, 0, 0, -20, 0, -20, true);
	Draw_Robot_Arm(0.5, 0.5, 1, 1.5, 1.5, 1.5, 0, 0, 0, -20, 0, 20);
	Draw_Robot();
	glPopMatrix();
}

// 디스플레이
void MyDisplay() {
	glCullFace(cull_mode);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, WIDTH / 3. * 2, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(robot_cam_f, WIDTH / 3. * 2 / HEIGHT, 0.01, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam.x, cam.y, cam.z, cam.x + cam.dx, cam.y + cam.dy, cam.z + cam.dz, cam.ux, cam.uy, cam.uz);
	Draw_scene();


	glViewport(WIDTH / 3. * 2, HEIGHT / 2., WIDTH / 3., HEIGHT / 2.);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-30 * (WIDTH / 3.) / (HEIGHT / 2.), 30 * (WIDTH / 3.) / (HEIGHT / 2.), -30 - 10, 30 - 10, 0, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 50, 0, 0, 0, 0, 1, 0, 0);
	Draw_scene();


	glViewport(WIDTH / 3. * 2, 0, WIDTH / 3., HEIGHT / 2.);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5 * (WIDTH / 3.) / (HEIGHT / 2.), 0.5 * (WIDTH / 3.) / (HEIGHT / 2.), -0.5, 0.5, 1, 150);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-10, 10, 100, -10, 10, 0, 0, 1, 0);
	Draw_scene();

	glFlush();
}

// 윈도우 재설정
void MyReshape(int width, int height) {
	WIDTH = width; HEIGHT = height;
}

// 빛 설정 초기화
void MyLightInit() {
	GLfloat global_ambient[] = { 0.1, 0.1, 0.1, 1 };
	glClearColor(global_ambient[0], global_ambient[1], global_ambient[2], global_ambient[3]);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	GLfloat
		light0_ambient[] = { 0.2, 0.2, 0.2, 1 },
		light0_diffuse[] = { 0.6, 0.6, 0.6, 1 },
		light0_specular[] = { 0.2, 0.2, 0.2, 1 };

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

	glEnable(GL_LIGHT1);
}

// 메인 함수
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("로봇 팔");

	MyLightInit();
	glEnable(GL_CULL_FACE);

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutKeyboardFunc(MyKeyboard);
	glutKeyboardUpFunc(MyKeyboardUp);
	glutSpecialFunc(MySpecial);
	glutSpecialUpFunc(MySpecialUp);

	glutMainLoop();
	return 0;
}