#include <glut.h>

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	// ��ǥ�� ��
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2f(1.0, 0.0);
	glVertex2f(-1.0, 0.0);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, -1.0);
	glEnd();

	// "��" ����
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(-0.6, -0.5);
	glVertex2f(-0.4, -0.5);
	glVertex2f(-0.2, 0.5);
	glVertex2f(-0.4, 0.5);

	glVertex2f(-0.4, -0.1);
	glVertex2f(-0.2, -0.5);
	glVertex2f(0.0, -0.5);
	glVertex2f(-0.3, 0.1);
	glEnd();

	// "��" ����
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glVertex2f(-0.1, -0.1);
	glVertex2f(0.1, -0.1);
	glVertex2f(0.1, 0.1);
	glVertex2f(-0.1, 0.1);

	glVertex2f(0.1, -0.4);
	glVertex2f(0.3, -0.4);
	glVertex2f(0.3, 0.4);
	glVertex2f(0.1, 0.4);


	glVertex2f(0.4, -0.5);
	glVertex2f(0.6, -0.5);
	glVertex2f(0.6, 0.5);
	glVertex2f(0.4, 0.5);
	glEnd();

	glFlush();
}

int main()
{
	glutCreateWindow("Example");
	glutDisplayFunc(MyDisplay);
	glutMainLoop();
	return 0;
}