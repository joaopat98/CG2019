#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

//--------------------------------- Definir cores
#define BLUE 0.0, 0.0, 1.0, 1.0
#define RED 1.0, 0.0, 0.0, 1.0
#define YELLOW 1.0, 1.0, 0.0, 1.0
#define GREEN 0.0, 1.0, 0.0, 1.0
#define WHITE 1.0, 1.0, 1.0, 1.0
#define BLACK 0.0, 0.0, 0.0, 1.0
#define PI 3.14159
#define DEGMULT PI / 180

//================================================================================
//===========================================================Variaveis e constantes

struct vec3
{
	GLfloat x, y, z;
};

vec3 operator+(vec3 v1, vec3 v2)
{
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec3 operator-(vec3 v)
{
	return {-v.x, -v.y, -v.y};
}

vec3 operator-(vec3 v1, vec3 v2)
{
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

void operator+=(vec3 &lhs, const vec3 &rhs)
{
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
}

void operator-=(vec3 &lhs, const vec3 &rhs)
{
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
}

vec3 operator*(vec3 v, GLfloat c)
{
	return {v.x * c, v.y * c, v.z * c};
}

vec3 operator*(GLfloat c, vec3 v)
{
	return {v.x * c, v.y * c, v.z * c};
}

struct vec2
{
	GLfloat x, y;
};
GLfloat t = 0;

vec3 lerp3(vec3 v1, vec3 v2, GLfloat c)
{
}

//------------------------------------------------------------ Sistema Coordenadas + objectos
int mousex = 0, mousey = 0;
GLfloat pos_state = 0, pos_state_incr = 5;
int steps = 1000;
int step_under = 50, step_over = 50;
GLfloat min_rad = 2, radius = 10, frac = 10, height = 0.6;
GLfloat border_size = 0.1;
GLfloat y_offset = 4;
bool free_cam = false;
bool forward, backward, left, right, up, down;
GLfloat speed = 5;
GLfloat incr = 0.1;
vec3 pos = {-10, 10, 0};
vec2 rot = {90, 0};
GLint wScreen = 800,
	  hScreen = 600;					  //.. janela (pixeis)
GLfloat xC = 10.0, yC = 10.0, zC = 100.0; //.. Mundo  (unidades mundo)
GLfloat color_time_mult = 50;

//------------------------------------------------------------ Observador
GLfloat angZoom = 90;
GLfloat incZoom = 3;
GLfloat offset = 0;

//================================================================================
//=========================================================================== INIT
void inicializa(void)
{
	glClearColor(BLACK);	 //………………………………………………………………………………Apagar
	glEnable(GL_DEPTH_TEST); //………………………………………………………………………………Profundidade
	glShadeModel(GL_SMOOTH); //………………………………………………………………………………Interpolacao de cores
	pos.x = cos(pos_state * frac * DEGMULT) * radius / 2;
	pos.z = sin(pos_state * frac * DEGMULT) * radius / 2;
	pos.y = pos_state * height + y_offset;
}

void glColorHSV(GLfloat h, GLfloat s, GLfloat v)
{
	GLfloat c = v * s;
	while (h < 0)
		h += 360;
	GLfloat x = c * (1 - fabs(fmod((h / 60), 2) - 1));
	GLfloat m = v - c;
	GLfloat r, g, b;
	h = fmod(h, 360);
	if (h < 60)
	{
		r = c;
		g = x;
		b = 0;
	}
	else if (h < 120)
	{
		r = x;
		g = c;
		b = 0;
	}
	else if (h < 180)
	{
		r = 0;
		g = c;
		b = x;
	}
	else if (h < 240)
	{
		r = 0;
		g = x;
		b = c;
	}
	else if (h < 300)
	{
		r = x;
		g = 0;
		b = c;
	}
	else
	{
		r = c;
		g = 0;
		b = x;
	}
	glColor3f(r, g, b);
}

void drawEixos()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo X
	glColor4f(RED);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(10, 0, 0);
	glEnd();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo Y
	glColor4f(GREEN);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 10, 0);
	glEnd();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Eixo Z
	glColor4f(BLUE);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 0, 10);
	glEnd();
}

void drawScene()
{
	glColor3f(1, 1, 1);
	GLfloat x2 = cos(frac * DEGMULT) * radius;
	GLfloat x3 = cos(frac * DEGMULT) * min_rad;
	GLfloat z2 = -sin(frac * DEGMULT) * radius;
	GLfloat z3 = -sin(frac * DEGMULT) * min_rad;
	GLfloat border_x = cos(frac * DEGMULT) * (radius - border_size);
	GLfloat border_z = -sin(frac * DEGMULT) * (radius - border_size);

	int step_begin = (int)floor(pos_state);
	glPushMatrix();
	{
		for (int i = step_begin - step_under; (i - (step_begin - step_under)) * frac < 360; i++)
		{
			glBegin(GL_POLYGON);
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x3, (step_begin + step_over) * height, z3);
			glVertex3f(x3, (step_begin - step_under) * height, z3);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glVertex3f(min_rad, (step_begin - step_under) * height, 0);
			glVertex3f(min_rad, (step_begin + step_over) * height, 0);
			glEnd();

			glRotatef(frac, 0, 1, 0);
		}
	}
	glPopMatrix();
	glRotatef(frac * step_begin - step_under, 0, 1, 0);
	printf("%f\n", t);
	glPushMatrix();
	{
		for (int i = step_begin - step_under; i < step_begin + step_over; i++)
		{
			glColorHSV((i - 1) * frac + t * color_time_mult + 180, 1, 1);
			glBegin(GL_POLYGON);
			glVertex3f(min_rad, i * height, 0);
			glVertex3f(radius, i * height, 0);
			glVertex3f(radius, (i + 1) * height, 0);
			glVertex3f(min_rad, (i + 1) * height, 0);
			glEnd();
			glColorHSV(i * frac + t * color_time_mult + 180, 1, 1);
			glBegin(GL_POLYGON);
			glVertex3f(x3, i * height, z3);
			glVertex3f(x2, i * height, z2);
			glVertex3f(x2, (i + 1) * height, z2);
			glVertex3f(x3, (i + 1) * height, z3);
			glEnd();

			glBegin(GL_POLYGON);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glVertex3f(min_rad, (i + 1) * height, 0);
			glVertex3f(radius, (i + 1) * height, 0);
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x2, (i + 1) * height, z2);
			glVertex3f(x3, (i + 1) * height, z3);
			glEnd();
			glBegin(GL_POLYGON);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glVertex3f(min_rad, i * height, 0);
			glVertex3f(radius, i * height, 0);
			glColorHSV(i * frac + +t * color_time_mult, 1, 1);
			glVertex3f(x2, i * height, z2);
			glVertex3f(x3, i * height, z3);
			glEnd();

			glBegin(GL_POLYGON);
			glColor3f(1, 1, 1);
			glVertex3f(radius - border_size, (i + 3) * height, 0);
			glVertex3f(radius, (i + 3) * height, 0);
			glVertex3f(x2, (i + 3) * height, z2);
			glVertex3f(border_x, (i + 3) * height, border_z);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(radius - border_size, (i + 3) * height, 0);
			glVertex3f(border_x, (i + 3) * height, border_z);
			glVertex3f(border_x, (i + 1) * height, border_z);
			glVertex3f(radius - border_size, (i + 1) * height, 0);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(radius, (i + 3) * height, 0);
			glVertex3f(x2, (i + 3) * height, z2);
			glVertex3f(x2, (i + 1) * height, z2);
			glVertex3f(radius, (i + 1) * height, 0);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(radius - border_size, (i + 3) * height, 0);
			glVertex3f(radius, (i + 3) * height, 0);
			glVertex3f(radius, (i + 1) * height, 0);
			glVertex3f(radius - border_size, (i + 1) * height, 0);
			glEnd();

			glColor3f(0, 0, 1);
			glBegin(GL_POLYGON);
			glVertex3f(min_rad, (i + 1) * height, 0);
			glVertex3f(x3, (i + 1) * height, z3);
			glVertex3f(x3, i * height, z3);
			glVertex3f(min_rad, i * height, 0);
			glEnd();
			glBegin(GL_POLYGON);
			glVertex3f(radius, (i + 1) * height, 0);
			glVertex3f(x2, (i + 1) * height, z2);
			glVertex3f(x2, i * height, z2);
			glVertex3f(radius, i * height, 0);
			glEnd();

			glRotatef(frac, 0, 1, 0);
		}
	}
	glPopMatrix();
}

void display(void)
{

	//================================================================= APaga ecran/profundidade
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//================================================================= NAO MOFIFICAR
	glViewport(0, 0, wScreen, hScreen);								// ESQUECER PoR AGORA
	glMatrixMode(GL_PROJECTION);									// ESQUECER PoR AGORA
	glLoadIdentity();												// ESQUECER PoR AGORA
	gluPerspective(angZoom, (float)wScreen / hScreen, 0.1, 3 * zC); // ESQUECER PoR AGORA
	glMatrixMode(GL_MODELVIEW);										// ESQUECER PoR AGORA
	glLoadIdentity();												// ESQUECER PoR AGORA
																	//================================================================= NAO MOFIFICAR

	//-------------------------------------------------------------- observador
	vec3 dir;
	dir.x = 5 * sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.z = 5 * cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.y = 5 * sin(rot.y * DEGMULT);
	dir += pos;

	gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, 0, 1, 0);

	//…………………………………………………………………………………………………………………………………………………………Objectos/modelos
	drawEixos();
	drawScene();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Actualizacao
	glutSwapBuffers();
}

//======================================================= EVENTOS
void keyboard(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'w':
	case 'W':
		forward = true;
		break;
	case 's':
	case 'S':
		backward = true;
		break;
	case 'a':
	case 'A':
		left = true;
		break;
	case 'd':
	case 'D':
		right = true;
		break;
	case 'q':
	case 'Q':
		up = true;
		break;
	case 'e':
	case 'E':
		down = true;
		break;
	case 'f':
	case 'F':
		free_cam = !free_cam;
		pos.x = cos(pos_state * frac * DEGMULT) * radius / 2;
		pos.z = -sin(pos_state * frac * DEGMULT) * radius / 2;
		pos.y = pos_state * height + y_offset;
		break;
	//--------------------------- Escape
	case 27:
		exit(0);
		break;
	}
}

void keyRelease(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'w':
	case 'W':
		forward = false;
		break;
	case 's':
	case 'S':
		backward = false;
		break;
	case 'a':
	case 'A':
		left = false;
		break;
	case 'd':
	case 'D':
		right = false;
		break;
	case 'q':
	case 'Q':
		up = false;
		break;
	case 'e':
	case 'E':
		down = false;
		break;
		//--------------------------- Escape
	case 27:
		exit(0);
		break;
	}
}

void teclasNotAscii(int key, int x, int y)
{

	glutPostRedisplay();
}

void reshape(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
	// Compute aspect ratio of the new window
	if (height == 0)
		height = 1; // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);
	hScreen = height;
	wScreen = width;
	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
	glLoadIdentity();			 // Reset
	// Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	glutPostRedisplay();
}

//======================================================= MAIN
//======================================================= MAIN

void mouse(int x, int y)
{
	rot.x += (wScreen / 2 - x) * incr;
	rot.y += (hScreen / 2 - y) * incr;
	if (rot.y > 85)
		rot.y = 85;
	if (rot.y < -85)
		rot.y = -85;
}

void update()
{
	GLfloat deltaT;
	GLfloat curTime = glutGet(GLUT_ELAPSED_TIME) / (float)1000;
	if (t == 0)
		deltaT = 0;
	else
		deltaT = curTime - t;
	t = curTime;

	if (forward)
	{
		if (free_cam)
		{
			vec3 dir;
			dir.x = sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.z = cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.y = sin(rot.y * DEGMULT);
			pos += dir * deltaT * speed;
		}
		else
		{
			pos_state += deltaT * pos_state_incr;
			pos.x = cos(pos_state * frac * DEGMULT) * radius / 2;
			pos.z = -sin(pos_state * frac * DEGMULT) * radius / 2;
			pos.y = pos_state * height + y_offset;
		}
	}
	if (backward)
	{
		if (free_cam)
		{
			vec3 dir;
			dir.x = sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.z = cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.y = sin(rot.y * DEGMULT);
			pos -= dir * deltaT * speed;
		}
		else
		{
			pos_state -= deltaT * pos_state_incr;
			pos.x = cos(pos_state * frac * DEGMULT) * radius / 2;
			pos.z = -sin(pos_state * frac * DEGMULT) * radius / 2;
			pos.y = pos_state * height + y_offset;
		}
	}
	if (free_cam && left)
	{
		vec3 dir;
		dir.x = sin((rot.x + 90) * DEGMULT);
		dir.z = cos((rot.x + 90) * DEGMULT);
		pos += dir * deltaT * speed;
	}
	if (free_cam && right)
	{
		vec3 dir;
		dir.x = sin((rot.x + 90) * DEGMULT);
		dir.z = cos((rot.x + 90) * DEGMULT);
		pos -= dir * deltaT * speed;
	}
	if (free_cam && up)
	{
		vec3 dir;
		dir.x = sin(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.z = cos(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.y = sin((rot.y + 90) * DEGMULT);
		pos += dir * deltaT * speed;
	}
	if (free_cam && down)
	{
		vec3 dir;
		dir.x = sin(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.z = cos(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.y = sin((rot.y + 90) * DEGMULT);
		pos -= dir * deltaT * speed;
	}
	if (free_cam)
	{
		pos_state = (pos.y - y_offset) / height;
	}
	glutWarpPointer(wScreen / 2, hScreen / 2);
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("{jh,pjmm}@dei.uc.pt|       |FaceVisivel:'f'|      |Observador:'SETAS'|        |Andar-'a/s'|        |Rodar -'e/d'| ");

	inicializa();

	glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyRelease);
	glutReshapeFunc(reshape); // Register callback handler for window re-size event
	glutMotionFunc(mouse);
	glutPassiveMotionFunc(mouse);
	glutIdleFunc(update);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMainLoop();

	return 0;
}
