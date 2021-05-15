#include <fstream>
#include <iostream>
#include <list>

#include <cstdlib>

#include <glut.h>

using namespace std;

// 현재 설정 저장용 구조체
typedef struct setting {
	int width = 1000, height = 1000, x = 0, y = 0;
	GLfloat Red = 0.0, Green = 0.0, Blue = 0.0;
	GLenum Mode = 0;
}setting;

setting Setting;

// 그림 저장용 구조체
typedef struct figure {
	GLint TopLeftX = 0, TopLeftY = 0, BottomRightX = 0, BottomRightY = 0;
	GLfloat Red = 0.0, Green = 0.0, Blue = 0.0;
	GLenum Mode = 0;
	bool state = true;
}figure;

list<figure> List;
list<figure>::iterator it = List.begin();

// 저장
void MySave() {
	ofstream fout;
	fout.open("save.txt");
	
	list<figure>::iterator iter;
	for (iter = List.begin(); iter != List.end(); iter++) {
		fout << iter->TopLeftX << "\t" << iter->TopLeftY << "\t"
			<< iter->BottomRightX << "\t" << iter->BottomRightY << "\t"
			<< iter->Red << "\t" << iter->Green << "\t" << iter->Blue << "\t"
			<< iter->Mode << "\t" << endl;
	}
	if (fout.is_open()) fout.close();
}

// 불러오기
void MyLoad() {
	ifstream fin;
	fin.open("save.txt");

	if (!fin.is_open()) return;
	else {
		list<figure>::iterator iter;
		while (!fin.eof()) {
			figure temp;
			fin >> temp.TopLeftX >> temp.TopLeftY
				>> temp.BottomRightX >> temp.BottomRightY
				>> temp.Red >> temp.Green >> temp.Blue
				>> temp.Mode;
			List.push_back(temp);
		}
	}
	if (fin.is_open()) fin.close();
	glutPostRedisplay();
}

// 마우스 클릭 콜백 함수
void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if (!Setting.Mode) return; // 아직 설정이 안된 경우
	// 마우스 왼쪽 버튼을 눌렀을 경우
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
		List.push_back(figure());
		it = List.begin();
		advance(it, List.size() - 1);
		it->Red = Setting.Red; it->Green = Setting.Green; it->Blue = Setting.Blue;
		it->Mode = Setting.Mode;
		
		it->TopLeftX = X; it->TopLeftY = Y;
	} 
	// 마우스 왼쪽 버튼을 뗀 경우
	else if (Button == GLUT_LEFT_BUTTON && State == GLUT_UP) {
		if (!List.size()) return;
		it->BottomRightX = X;
		it->BottomRightY = Y;
		it->state = false;
		glutPostRedisplay();
	}
}

// 마우스 클릭 및 움직임 콜백 함수
void MyMouseMove(GLint X, GLint Y) {
	// 아직 설정이 안된 경우와 아직 안만들어진 경우
	if (!Setting.Mode || !List.size()) return;
	if (!it->state) return; // 이미 그림이 다 그려진 경우
	it->BottomRightX = X; it->BottomRightY = Y;
	glutPostRedisplay();
}

// 키보드 입력 콜백 함수
void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	// 종료 키
	case 'Q':
		exit(0); break;
	case 'q':
		exit(0); break;
	// 전체 지우기
	case 27: //'esc' 키의 아스키 코드 값
		List.clear();
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		break;
	// 색상 선택
	case '1':
		Setting.Red = 1.0; Setting.Green = 0.0; Setting.Blue = 0.0; break;
	case '2':
		Setting.Red = 0.0; Setting.Green = 1.0; Setting.Blue = 0.0; break;
	case '3':
		Setting.Red = 0.0; Setting.Green = 0.0; Setting.Blue = 1.0; break;
	// 모양 선택
	case 'r':
		Setting.Mode = GL_QUADS; break;
	case 't':
		Setting.Mode = GL_TRIANGLES; break;
	case 'l':
		Setting.Mode = GL_LINES; break;
	// 저장, 불러오기
	case 'S':
		MySave(); break;
	case 'L':
		MyLoad(); break;
	}
}

// 출력 콜백 함수
void MyDisplay() {
	glViewport(Setting.x, Setting.y, Setting.width, Setting.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	list<figure>::iterator iter;
	for (iter = List.begin(); iter != List.end(); iter++) {
		glColor3f(iter->Red, iter->Green, iter->Blue);
		glBegin(GL_LINE_LOOP);//iter->Mode);
		if (iter->Mode == GL_QUADS) {
			glVertex3f(iter->TopLeftX / (double) Setting.width,
				(Setting.height - iter->TopLeftY) / (double) Setting.height, 0.0);
			glVertex3f(iter->TopLeftX / (double)Setting.width,
				(Setting.height - iter->BottomRightY) / (double) Setting.height, 0.0);
			glVertex3f(iter->BottomRightX / (double)Setting.width,
				(Setting.height - iter->BottomRightY) / (double) Setting.height, 0.0);
			glVertex3f(iter->BottomRightX / (double)Setting.width,
				(Setting.height - iter->TopLeftY) / (double) Setting.height, 0.0);
		}
		else if (iter->Mode == GL_TRIANGLES) {
			glVertex3f(iter->TopLeftX / (double)Setting.width,
				(Setting.height - iter->TopLeftY) / (double) Setting.height, 0.0);
			glVertex3f(iter->TopLeftX / (double)Setting.width,
				(Setting.height - iter->BottomRightY) / (double) Setting.height, 0.0);
			glVertex3f(iter->BottomRightX / (double)Setting.width,
				(Setting.height - iter->BottomRightY) / (double) Setting.height, 0.0);
		}
		else if (iter->Mode == GL_LINES) {
			glVertex3f(iter->TopLeftX / (double)Setting.width,
				(Setting.height - iter->TopLeftY) / (double) Setting.height, 0.0);
			glVertex3f(iter->BottomRightX / (double)Setting.width,
				(Setting.height - iter->BottomRightY) / (double) Setting.height, 0.0);
		}
		glEnd();
	}
	glFlush();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(Setting.width, Setting.height);
	glutInitWindowPosition(Setting.x, Setting.y);
	glutCreateWindow("그림판");

	glutDisplayFunc(MyDisplay);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMove);
	glutKeyboardFunc(MyKeyboard);
	glutMainLoop();
	return 0;
}