#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "angel.hpp"

//--------------------------------- Definir cores
#define BLUE 0.0, 0.0, 1.0, 1.0
#define RED 1.0, 0.0, 0.0, 1.0
#define YELLOW 1.0, 1.0, 0.0, 1.0
#define GREEN 0.0, 1.0, 0.0, 1.0
#define WHITE 1.0, 1.0, 1.0, 1.0
#define BLACK 0.0, 0.0, 0.0, 1.0
#define PI 3.14159

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

vec3 operator/(vec3 v, GLfloat c)
{
	if (c == 0)
	{
		return v;
	}
	return {v.x / c, v.y / c, v.z / c};
}

struct vec2
{
	GLfloat x, y;
};
GLfloat t = 0;

vec3 lerp3(vec3 v1, vec3 v2, GLfloat c)
{
	return {};
}

/* #region  Program Params */

/* #region  	Mouse */
int mousex = 0, mousey = 0;
GLfloat mouse_incr = 0.1;
/* #endregion */

/* #region  	Stair Params */
int step_under = 25, step_over = 25;
GLfloat min_rad = 5, radius = 30, frac = 20, height = 2;
GLfloat color_time_mult = 100;
/* #region  		Stair Railing */
GLfloat border_size = 1;
GLfloat border_height = 8;
/* #endregion */
/* #endregion */

/* #region  	Stair State */
GLfloat pos_state_x = (radius + min_rad) / 2 / radius, pos_state_y = 0, pos_state_incr = 0.5;
/* #endregion */

/* #region  	Free Movement */
bool free_cam = false;
bool forward, backward, left, right, up, down;
GLfloat speed = 20;
/* #endregion */

/* #region  	Jumping */
GLfloat jumpVel = 40;
GLfloat curVel = 0;
GLfloat grav = -100;
bool jumping;
/* #endregion */

/* #region  	Observer */
vec3 pos = {-10, 10, 0};
vec2 rot = {90, 0};
GLfloat y_offset = 0;
/* #endregion */

/* #region  	View Settings */
GLint wScreen = 800, hScreen = 600;		  //.. janela (pixeis)
GLfloat xC = 10.0, yC = 10.0, zC = 100.0; //.. Mundo  (unidades mundo)
GLfloat angZoom = 90;
/* #endregion */

/* #region  	Angels */
const int num_angels = 1000;
GLfloat angel_delta = 0.2;
Angel angels[num_angels];
int angel_pos = 0;
GLfloat angelSize = 2;
GLfloat minAngelRad = 30, maxAngelRad = 50;
/* #endregion */

/* #endregion */

GLfloat angelSpeed()
{
	return 5 + frand() * 10;
}

void backAngel()
{
	for (int i = num_angels - 1; i > 0; i--)
	{
		angels[i] = angels[i - 1];
	}
	angels[0] = Angel(((angels[1].index - 1) - num_angels / (GLfloat)2) * angel_delta, angel_delta, minAngelRad, maxAngelRad, angelSpeed(), angelSize, angels[1].index - 1);
	angel_pos--;
}

void frontAngel()
{
	for (int i = 0; i < num_angels - 1; i++)
	{
		angels[i] = angels[i + 1];
	}
	angels[num_angels - 1] = Angel(((angels[num_angels - 2].index + 1) - num_angels / (GLfloat)2) * angel_delta, angel_delta, minAngelRad, maxAngelRad, angelSpeed(), angelSize, angels[num_angels - 2].index + 1);
	angel_pos++;
}

void init(void)
{
	glClearColor(BLACK);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	GLfloat ambLight[] = {0.2, 0.2, 0.2};
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);
	pos.x = cos(pos_state_y * frac * DEGMULT) * pos_state_x * radius;
	pos.z = sin(pos_state_y * frac * DEGMULT) * pos_state_x * radius;
	pos.y = pos_state_y * height + y_offset;
	angel_pos = pos.y / angel_delta;
	for (int i = 0; i < num_angels; i++)
	{
		angels[i] = Angel((i - num_angels / (GLfloat)2) * angel_delta, angel_delta, minAngelRad, maxAngelRad, angelSpeed(), angelSize, i);
	}
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

GLfloat dist2(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

GLfloat dist3(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
}

GLfloat flerp(GLfloat v1, GLfloat v2, GLfloat t)
{
	return v1 + (v2 - v1) * t;
}

void drawAxes()
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ X axis
	glColor4f(RED);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(10, 0, 0);
	glEnd();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Y axis
	glColor4f(GREEN);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 10, 0);
	glEnd();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Z axis
	glColor4f(BLUE);
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 0, 10);
	glEnd();
}

void drawLights()
{
	GLfloat lightPos[4] = {-1, -1, 0};
	GLfloat lightAmb[4] = {0.0, 0.0, 0.0, 0};
	GLfloat lightDiffuse[4] = {0.8, 0.8, 0.8, 0};
	GLfloat lightSpecular[4] = {0.5, 0.5, 0.5, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
}

void drawStairs()
{
	glColor3f(1, 1, 1);
	GLfloat x2 = cos(frac * DEGMULT) * radius;
	GLfloat x3 = cos(frac * DEGMULT) * min_rad;
	GLfloat z2 = -sin(frac * DEGMULT) * radius;
	GLfloat z3 = -sin(frac * DEGMULT) * min_rad;
	GLfloat border_frac = border_size / dist2(radius, 0, x2, z2);
	GLfloat border_x = cos(frac * DEGMULT) * (radius - border_size);
	GLfloat border_z = -sin(frac * DEGMULT) * (radius - border_size);

	int step_begin = (int)floor(pos_state_y);

	// center pillar

	glPushMatrix();
	{
		glRotatef(frac * step_begin - step_under, 0, 1, 0);
		float white[] = {1.0, 1.0, 1.0, 1.0};
		glMaterialfv(GL_FRONT, GL_AMBIENT, white);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		glMaterialfv(GL_FRONT, GL_SPECULAR, white);
		glMaterialf(GL_FRONT, GL_SHININESS, 10);
		for (int i = step_begin - step_under; (i - (step_begin - step_under)) * frac < 360; i++)
		{
			for (int j = -step_under; j < step_over; j++)
			{
				glBegin(GL_QUADS);
				//glColorHSV(i * frac + t * color_time_mult, 1, 1);
				glNormal3f(cos(frac * DEGMULT), 0, -sin(frac * DEGMULT));
				glVertex3f(x3, (step_begin + j + 1) * height, z3);
				glVertex3f(x3, (step_begin + j) * height, z3);
				//glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
				glNormal3f(1, 0, 0);
				glVertex3f(min_rad, (step_begin + j) * height, 0);
				glVertex3f(min_rad, (step_begin + j + 1) * height, 0);
				glEnd();
			}

			glRotatef(frac, 0, 1, 0);
		}
	}
	glPopMatrix();

	//steps

	glPushMatrix();
	{
		glRotatef(frac * step_begin - step_under, 0, 1, 0);
		glTranslatef(0, (step_begin - step_under) * height, 0);

		for (int i = step_begin - step_under; i < step_begin + step_over; i++)
		{
			//front
			glColorHSV((i - 1) * frac + t * color_time_mult + 120, 1, 1);
			glBegin(GL_QUADS);
			glNormal3f(0, 0, -1);
			glVertex3f(min_rad, 0, 0);
			glVertex3f(radius, 0, 0);
			glVertex3f(radius, height, 0);
			glVertex3f(min_rad, height, 0);
			glEnd();

			//top
			glBegin(GL_QUADS);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glNormal3f(0, -1, 0);
			glVertex3f(min_rad, height, 0);
			glVertex3f(radius, height, 0);
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x2, height, z2);
			glVertex3f(x3, height, z3);
			glEnd();

			//back
			glPushMatrix();
			{
				glRotatef(frac, 0, 1, 0);
				glColorHSV(i * frac + t * color_time_mult + 120, 1, 1);
				glBegin(GL_QUADS);
				glNormal3f(0, 0, -1);
				glVertex3f(min_rad, 0, 0);
				glVertex3f(radius, 0, 0);
				glVertex3f(radius, height, 0);
				glVertex3f(min_rad, height, 0);
				glEnd();
			}
			glPopMatrix();

			//bottom
			glBegin(GL_QUADS);
			glNormal3f(0, -1, 0);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glVertex3f(min_rad, 0, 0);
			glVertex3f(radius, 0, 0);
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x2, 0, z2);
			glVertex3f(x3, 0, z3);
			glEnd();

			//railing

			glColorHSV((i - 1) * frac + t * color_time_mult + 120, 1, 1);

			glBegin(GL_QUADS);
			glNormal3f(0, 0, -1);
			glVertex3f(radius - border_size, height, 0);
			glVertex3f(radius, height, 0);
			glVertex3f(radius, height + border_height, 0);
			glVertex3f(radius - border_size, height + border_height, 0);
			glEnd();

			glBegin(GL_QUADS);
			glNormal3f(0, 0, -1);
			glVertex3f(flerp(radius - border_size, border_x, border_frac), height, flerp(0, border_z, border_frac));
			glVertex3f(flerp(radius, x2, border_frac), height, flerp(0, z2, border_frac));
			glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
			glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
			glEnd();

			glBegin(GL_QUADS);
			glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(-1, 0, 0);
			glVertex3f(radius, height + border_height, 0);
			glVertex3f(radius, height + border_height - border_size, 0);
			glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(-cos(frac * DEGMULT), 0, sin(frac * DEGMULT));
			glVertex3f(x2, height + border_height - border_size, z2);
			glVertex3f(x2, height + border_height, z2);
			glEnd();

			glBegin(GL_QUADS);
			glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(-1, 0, 0);
			glVertex3f(radius, height + border_height - border_size, 0);
			glVertex3f(radius, height, 0);
			glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult + 240, 1, 1);
			glVertex3f(flerp(radius, x2, border_frac), height, flerp(0, z2, border_frac));
			glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
			glEnd();

			glBegin(GL_QUADS);
			glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(-1, 0, 0);
			glVertex3f(radius - border_size, height + border_height, 0);
			glVertex3f(radius - border_size, height + border_height - border_size, 0);
			glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(-cos(frac * DEGMULT), 0, sin(frac * DEGMULT));
			glVertex3f(border_x, height + border_height - border_size, border_z);
			glVertex3f(border_x, height + border_height, border_z);
			glEnd();

			glBegin(GL_QUADS);
			glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(1, 0, 0);
			glVertex3f(radius - border_size, height, 0);
			glVertex3f(radius - border_size, height + border_height - border_size, 0);
			glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult + 240, 1, 1);
			glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
			glVertex3f(flerp(radius - border_size, border_x, border_frac), height, flerp(0, border_z, border_frac));
			glEnd();

			glBegin(GL_QUADS);
			glNormal3f(0, -1, 0);
			glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult, 1, 1);
			glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
			glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x2, height + border_height - border_size, z2);
			glVertex3f(border_x, height + border_height - border_size, border_z);
			glEnd();

			glBegin(GL_QUADS);
			glNormal3f(0, -1, 0);
			glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glVertex3f(radius - border_size, height + border_height, 0);
			glVertex3f(radius, height + border_height, 0);
			glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glVertex3f(x2, height + border_height, z2);
			glVertex3f(border_x, height + border_height, border_z);
			glEnd();

			//right

			glBegin(GL_QUADS);

			glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(1, 0, 0);
			glVertex3f(radius, 0, 0);
			glVertex3f(radius, height, 0);
			glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(cos(frac * DEGMULT), 0, -sin(frac * DEGMULT));
			glVertex3f(x2, height, z2);
			glVertex3f(x2, 0, z2);
			glEnd();

			glRotatef(frac, 0, 1, 0);
			glTranslatef(0, height, 0);
		}
	}
	glPopMatrix();
}

void drawAngels()
{
	for (int i = 0; i < num_angels; i++)
	{
		angels[i].render();
	}
}

void drawScene()
{
	drawStairs();
	drawAngels();
}

void display(void)
{

	// set view

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, wScreen, hScreen);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angZoom, (float)wScreen / hScreen, 0.1, 3 * zC);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// update observer

	vec3 dir;
	dir.x = 5 * sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.z = 5 * cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.y = 5 * sin(rot.y * DEGMULT);
	dir += pos;

	gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, 0, 1, 0);

	// draw
	drawLights();
	drawAxes();
	drawScene();

	glutSwapBuffers();
}

GLfloat headBob(GLfloat state)
{
	return pow(cos(PI * state), 4) / 2 + state;
}

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
		jumping = false;
		free_cam = !free_cam;
		break;
	case ' ':
		if (!jumping)
		{
			jumping = true;
			curVel = jumpVel;
		}
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
		// Escape
	case 27:
		exit(0);
		break;
	}
}

void teclasNotAscii(int key, int x, int y)
{

	glutPostRedisplay();
}

//resize window
void resize(GLsizei width, GLsizei height)
{
	if (height == 0)
		height = 1;
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);
	hScreen = height;
	wScreen = width;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	rot.x += (wScreen / 2 - x) * mouse_incr;
	rot.y += (hScreen / 2 - y) * mouse_incr;
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

	vec3 dir{0, 0, 0};

	// update direction vector

	if (forward)
	{
		if (free_cam)
		{
			dir.x = sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.z = cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.y = sin(rot.y * DEGMULT);
			pos += dir * deltaT * speed;
		}
		else
		{
			dir.x += sin(rot.x * DEGMULT);
			dir.z += cos(rot.x * DEGMULT);
		}
	}
	if (backward)
	{
		if (free_cam)
		{

			dir.x = sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.z = cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
			dir.y = sin(rot.y * DEGMULT);
			pos -= dir * deltaT * speed;
		}
		else
		{

			dir.x -= sin(rot.x * DEGMULT);
			dir.z -= cos(rot.x * DEGMULT);
		}
	}
	if (left)
	{
		if (free_cam)
		{
			dir.x = sin((rot.x + 90) * DEGMULT);
			dir.z = cos((rot.x + 90) * DEGMULT);
			pos += dir * deltaT * speed;
		}
		else
		{
			dir.x += sin((rot.x + 90) * DEGMULT);
			dir.z += cos((rot.x + 90) * DEGMULT);
		}
	}
	if (right)
	{
		if (free_cam)
		{
			dir.x = sin((rot.x + 90) * DEGMULT);
			dir.z = cos((rot.x + 90) * DEGMULT);
			pos -= dir * deltaT * speed;
		}
		else
		{
			dir.x -= sin((rot.x + 90) * DEGMULT);
			dir.z -= cos((rot.x + 90) * DEGMULT);
		}
	}
	if (free_cam && up)
	{

		dir.x = sin(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.z = cos(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.y = sin((rot.y + 90) * DEGMULT);
		pos += dir * deltaT * speed;
	}
	if (free_cam && down)
	{

		dir.x = sin(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.z = cos(rot.x * DEGMULT) * cos((rot.y + 90) * DEGMULT);
		dir.y = sin((rot.y + 90) * DEGMULT);
		pos -= dir * deltaT * speed;
	}
	if (free_cam)
	{
		//free camera mode
		pos_state_y = (pos.y - y_offset) / height;
	}
	else
	{
		// stair mode

		//get rotation relative to current stair step

		GLfloat ang = pos_state_y * frac;
		dir = dir / dist2(0, 0, dir.x, dir.z);
		vec3 rotdir{
			dir.x * cos(ang * DEGMULT) - dir.z * sin(ang * DEGMULT),
			0,
			-dir.z * cos(ang * DEGMULT) - dir.x * sin(ang * DEGMULT),
		};

		// calc next position in stairs

		vec3 newPos = {pos_state_x, 0, 0};
		newPos += rotdir * pos_state_incr * deltaT;
		GLfloat magn = dist2(0, 0, newPos.x, newPos.z);

		// stair bounds

		if (magn < min_rad / radius + 0.01)
		{
			newPos = newPos / dist2(0, 0, newPos.x, newPos.z) * ((min_rad / radius) + 0.01);
			magn = min_rad / radius + 0.01;
		}
		if (magn > 1 - (border_size / radius) - 0.01)
		{
			newPos = newPos / dist2(0, 0, newPos.x, newPos.z) * (1 - (border_size / radius) - 0.01);
			magn = 1 - (border_size / radius) - 0.01;
		}

		//calc stair state

		GLfloat moveAngle = 0;

		if (newPos.z != 0)
			moveAngle = atan(newPos.z / newPos.x) / DEGMULT;
		pos_state_y += moveAngle / frac;
		pos_state_x = magn;

		//update position

		pos.x = cos(pos_state_y * frac * DEGMULT) * pos_state_x * radius;
		pos.z = -sin(pos_state_y * frac * DEGMULT) * pos_state_x * radius;

		//update jumping params

		if (jumping)
		{
			curVel += grav * deltaT;
			pos.y += curVel * deltaT;
			GLfloat stdY = headBob(pos_state_y) * height + y_offset;
			if (pos.y <= stdY)
			{
				jumping = false;
				pos.y = stdY;
			}
		}
		else
			pos.y = headBob(pos_state_y) * height + y_offset;
	}

	// update rendered angels

	int new_angel_pos = (int)floor(pos.y / angel_delta);
	while (new_angel_pos > angel_pos)
		frontAngel();
	while (new_angel_pos < angel_pos)
		backAngel();
	angel_pos = new_angel_pos;

	for (int i = 0; i < num_angels; i++)
	{
		angels[i].update(deltaT);
	}

	// reset pointer to center of screen

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

	init();

	glutSpecialFunc(teclasNotAscii);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyRelease);
	glutReshapeFunc(resize);
	glutMotionFunc(mouse);
	glutPassiveMotionFunc(mouse);
	glutIdleFunc(update);
	glutSetCursor(GLUT_CURSOR_NONE);
	glutMainLoop();

	return 0;
}
