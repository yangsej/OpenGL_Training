#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

#include <glut.h>

using namespace std;

GLsizei v_size, n_size, f_size, *v_amount;
GLfloat *vertex, *normal;
GLuint **face_v, **face_n;

GLdouble cam_pos[3] = { 2, 2, 2 }, fovy = 60, cam_center[3] = { 0,0,0 };

void Read_3VN(char* dir) {
	ifstream fp;

	fp.open(dir);
	fp >> v_size >> n_size >> f_size;

	vertex = new GLfloat[v_size * 3]{ 0 };
	for (int i = 0; i<v_size; i++) {
		fp >> vertex[3 * i] >> vertex[3 * i + 1] >> vertex[3 * i + 2];
		cam_center[0] += vertex[3 * i];
		cam_center[1] += vertex[3 * i + 1];
		cam_center[2] += vertex[3 * i + 2];
	}
	cam_center[0] /= v_size; cam_center[1] /= v_size; cam_center[2] /= v_size;

	normal = new GLfloat[n_size * 3]{ 0 };
	for (int i = 0; i<n_size; i++) {
		fp >> normal[3 * i] >> normal[3 * i + 1] >> normal[3 * i + 2];
	}

	face_v = new GLuint*[f_size] { 0 };
	face_n = new GLuint*[f_size] { 0 };
	v_amount = new GLsizei[f_size];
	for (int i = 0; i<f_size; i++) {
		fp >> v_amount[i];
		face_v[i] = new GLuint[v_amount[i]]{ 0 };
		face_n[i] = new GLuint[v_amount[i]]{ 0 };
		for (int j = 0; j < v_amount[i]; j++) {
			fp >> face_v[i][j];
		}
		for (int j = 0; j < v_amount[i]; j++) {
			fp >> face_n[i][j];
		}
	}

	fp.close();
}

void MySpecial(int key, int X, int Y) {
	GLdouble pos_temp[3] = { cam_pos[0], cam_pos[1], cam_pos[2] };
	GLdouble center_temp[3] = { cam_center[0], cam_center[1], cam_center[2] };
	switch (key) {
	case GLUT_KEY_LEFT:
		cam_pos[0] = ((pos_temp[0] - center_temp[0]) * cos(M_PI / 180)
			- (pos_temp[2] - center_temp[2]) * sin(M_PI / 180)) + center_temp[0];
		cam_pos[2] = ((pos_temp[0] - center_temp[0]) * sin(M_PI / 180)
			+ (pos_temp[2] - center_temp[2]) * cos(M_PI / 180)) + center_temp[2];
		break;
	case GLUT_KEY_RIGHT:
		cam_pos[0] = ((pos_temp[0] - center_temp[0]) * cos(-M_PI / 180)
			- (pos_temp[2] - center_temp[2]) * sin(-M_PI / 180)) + center_temp[0];
		cam_pos[2] = ((pos_temp[0] - center_temp[0]) * sin(-M_PI / 180)
			+ (pos_temp[2] - center_temp[2]) * cos(-M_PI / 180)) + center_temp[2];
		break;
	case GLUT_KEY_UP:
		if (fovy >= 10) fovy--; break;
	case GLUT_KEY_DOWN:
		if (fovy <= 170) fovy++; break;
	}
	glutPostRedisplay();
}

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, 1, 1, 7);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam_pos[0], cam_pos[1], cam_pos[2],
		cam_center[0], cam_center[1], cam_center[2],
		0, 1, 0);

	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(3, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 3, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 3);
	glEnd();

	glColor3f(0.6, 0.3, 0.3);
	for (GLint i = 0; i < f_size; i++) {
		glDrawElements(GL_POLYGON, v_amount[i], GL_UNSIGNED_INT, face_v[i]);
	}

	glFlush();
}

int main(int argc, char** argv) {
	Read_3VN("basicBarn.3vn");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("3D ¸Þ½¬");

	glClearColor(1, 1, 1, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);

	glutDisplayFunc(MyDisplay);
	glutSpecialFunc(MySpecial);

	glutMainLoop();

	for (int i = 0; i < f_size; i++)
		delete[] face_v[i], face_n[i];
	delete[] v_amount, vertex, normal, face_v, face_n;

	return 0;
}