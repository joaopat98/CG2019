#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include "shaders.h"
#include <GL/glut.h>
#include "angel.hpp"
#include "RgbImage.h"
#include "png.h"

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

GLuint textures[32];
RgbImage img;

GLfloat sizeSky = 150;

bool flash = true;

GLfloat flash_ang = 30;
GLfloat flash_exp = 10;
GLfloat flash_att = 0.15;

bool no_clip = false;

/* #region  	Mouse */
int mousex = 0, mousey = 0;
GLfloat mouse_incr = 0.1;
/* #endregion */

/* #region  	Stair Params */
int step_under = 25, step_over = 25;
GLfloat min_rad = 5, radius = 30, frac = 20, height = 2;
GLfloat color_time_mult = 100;
GLfloat day_length = 100;
GLfloat day_time = day_length;
GLfloat day_contrast = 3;
GLfloat day_state;
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
vec3 dir;
vec2 rot = {90, 0};
GLfloat y_offset = 0;
/* #endregion */

/* #region  	View Settings */
GLint wScreen = 800, hScreen = 600;		  //.. janela (pixeis)
GLfloat xC = 10.0, yC = 10.0, zC = 100.0; //.. Mundo  (unidades mundo)
GLfloat angZoom = 90;
/* #endregion */

/* #region  	Angels */
const int num_angels = 100;
GLfloat angel_space = 200;
GLfloat angel_delta = angel_space / num_angels;
Angel angels[num_angels];
int angel_pos = 0;
GLfloat angelSize = 6;
GLfloat minAngelRad = 30, maxAngelRad = 50;
/* #endregion */

/* #endregion */

GLfloat angelSpeed()
{
	return 2 + frand() * 20;
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

void init_textures()
{
	glGenTextures(6, textures);

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	img.LoadBmpFile("images/pillar.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				 img.GetNumCols(),
				 img.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
				 img.ImageData());

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	img.LoadBmpFile("images/marble.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				 img.GetNumCols(),
				 img.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
				 img.ImageData());

	/*
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	img.LoadBmpFile("images/angel.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				 img.GetNumCols(),
				 img.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
				 img.ImageData());
*/

	int w, h;
	textures[2] = loadTexture("images/angel.png", w, h);
	//textures[3] = loadTexture("images/skyBox.png", w, h);

	glBindTexture(GL_TEXTURE_2D, textures[4]);
	img.LoadBmpFile("images/railing.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,
				 img.GetNumCols(),
				 img.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE,
				 img.ImageData());

	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[3]);

	char skyBox[6][100] = {
		"images/skybox/day/right.bmp",
		"images/skybox/day/left.bmp",
		"images/skybox/day/top.bmp",
		"images/skybox/day/bottom.bmp",
		"images/skybox/day/front.bmp",
		"images/skybox/day/back.bmp"};
	GLenum cube[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
					  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
					  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
					  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

	for (unsigned int i = 0; i < 6; i++)
	{
		img.LoadBmpFile(skyBox[i]);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(cube[i],
					 0, GL_RGB, img.GetNumRows(), img.GetNumCols(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.ImageData());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[5]);

	char skyBox_night[6][100] = {
		"images/skybox/night/right.bmp",
		"images/skybox/night/left.bmp",
		"images/skybox/night/top.bmp",
		"images/skybox/night/bottom.bmp",
		"images/skybox/night/front.bmp",
		"images/skybox/night/back.bmp"};

	for (unsigned int i = 0; i < 6; i++)
	{
		img.LoadBmpFile(skyBox_night[i]);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexImage2D(cube[i],
					 0, GL_RGB, img.GetNumRows(), img.GetNumCols(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.ImageData());
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void init(void)
{
	glClearColor(BLACK);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
	GLfloat ambLight[] = {0.1, 0.1, 0.1};
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);
	init_textures();
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
	GLfloat red[] = {1, 0, 0};
	glMaterialfv(GL_FRONT, GL_EMISSION, red);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ X axis
	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(12, 0, 0);
	glEnd();

	GLfloat green[] = {0, 1, 0};
	glMaterialfv(GL_FRONT, GL_EMISSION, green);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Y axis

	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 12, 0);
	glEnd();

	GLfloat blue[] = {0, 0, 1};
	glMaterialfv(GL_FRONT, GL_EMISSION, blue);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Z axis

	glBegin(GL_LINES);
	glVertex3i(0, 0, 0);
	glVertex3i(0, 0, 12);
	glEnd();

	GLfloat zero[] = {0, 0, 0};

	glMaterialfv(GL_FRONT, GL_EMISSION, zero);
}

void DrawSkyBox(int tex)
{
	GLfloat fExtent = sizeSky;
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textures[tex]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glPushMatrix();
	glTranslatef(0, pos.y, 0);
	glRotatef(-day_time * 180, 0, 1, 0);
	//////////////////////////////////////////////
	// Negative X
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);
	glEnd();
	glPopMatrix();

	///////////////////////////////////////////////
	//  Postive X
	glPushMatrix();
	glRotatef(180, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);
	glEnd();
	glPopMatrix();

	////////////////////////////////////////////////
	// Negative Z
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);
	glEnd();
	glPopMatrix();

	////////////////////////////////////////////////
	// Positive Z
	glPushMatrix();
	glRotatef(180, 0, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);
	glEnd();
	glPopMatrix();

	//////////////////////////////////////////////////
	// Positive Y
	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);

	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);

	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);
	glEnd();
	glPopMatrix();

	///////////////////////////////////////////////////
	// Negative Y
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);

	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);

	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);

	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glEnd();
	glPopMatrix();

	glPopMatrix();

	glDisable(GL_TEXTURE_CUBE_MAP);
}

void drawLights()
{
	glPushMatrix();
	{
		glRotatef(-day_time * 180, 0, 1, 0);
		glTranslatef(sizeSky, sizeSky, sizeSky);
		GLfloat lightPos[4] = {0, 0, 0, 1};
		GLfloat lightAmb[4] = {0.0, 0.0, 0.0, 0};
		GLfloat lightDiffuse[4] = {1 * day_state, 0.9 * day_state, 0.8 * day_state, 0};
		GLfloat lightSpecular[4] = {0.5 * day_state, 0.45 * day_state, 0.4 * day_state, 0};

		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	}
	glPopMatrix();

	GLfloat flashPos[4] = {pos.x, pos.y, pos.z, 1.0};
	GLfloat flashDir[3] = {dir.x, dir.y, dir.z};
	GLfloat flashAmb[4] = {0.0, 0.0, 0.0, 0};
	GLfloat flashDiffuse[4] = {1.0, 0.9, 0.3, 0};
	GLfloat flashSpecular[4] = {0.5, 0.45, 0.15, 0};

	glLightfv(GL_LIGHT1, GL_POSITION, flashPos);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, flashDir);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, flash_exp);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, flash_ang);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, flash_att);
	glLightfv(GL_LIGHT1, GL_AMBIENT, flashAmb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, flashDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, flashSpecular);

	/*
	glPushMatrix();
	vec3 off = {1, -1, 1};
	vec3 oof = pos + off;
	glTranslatef(oof.x, oof.y, oof.z);
	glutSolidCube(1);
	glPopMatrix();
	*/
}

void drawStairs()
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3f(1, 1, 1);
	GLfloat x2 = cos(frac * DEGMULT) * radius;
	GLfloat x3 = cos(frac * DEGMULT) * min_rad;
	GLfloat z2 = -sin(frac * DEGMULT) * radius;
	GLfloat z3 = -sin(frac * DEGMULT) * min_rad;
	GLfloat full_y = tan(frac) * radius;
	GLfloat border_frac = border_size / dist2(radius, 0, x2, z2);
	GLfloat border_x = cos(frac * DEGMULT) * (radius - border_size);
	GLfloat border_z = -sin(frac * DEGMULT) * (radius - border_size);

	int step_begin = (int)floor(pos_state_y);

	// center pillar
	glPushMatrix();
	{
		glBindTexture(GL_TEXTURE_2D, textures[0]);
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
				glNormal3f(1, 0, 0);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(min_rad, (step_begin + j + 1) * height, 0);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(min_rad, (step_begin + j) * height, 0);
				//glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
				glNormal3f(cos(frac * DEGMULT), 0, -sin(frac * DEGMULT));
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(x3, (step_begin + j) * height, z3);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(x3, (step_begin + j + 1) * height, z3);
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

		float white[] = {1.0, 1.0, 1.0, 1.0};
		glMaterialfv(GL_FRONT, GL_AMBIENT, white);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		glMaterialfv(GL_FRONT, GL_SPECULAR, white);
		glMaterialf(GL_FRONT, GL_SHININESS, 10);
		for (int i = step_begin - step_under; i < step_begin + step_over; i++)
		{

			glBindTexture(GL_TEXTURE_2D, textures[1]);

			//front
			//glColorHSV((i - 1) * frac + t * color_time_mult + 120, 1, 1);
			glNormal3f(0, 0, 1);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(min_rad, 0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(radius, 0, 0);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(radius, height, 0);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(min_rad, height, 0);
			glEnd();

			//top
			glNormal3f(0, 1, 0);
			glBegin(GL_QUADS);
			//glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(min_rad, height, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(radius, height, 0);
			//glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(x2, height, z2);
			glTexCoord2f(0.0, z3 / z2);
			glVertex3f(x3, height, z3);
			glEnd();

			//back
			glPushMatrix();
			{
				glRotatef(frac, 0, 1, 0);
				//glColorHSV(i * frac + t * color_time_mult + 120, 1, 1);
				glNormal3f(0, 0, -1);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(min_rad, 0, 0);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(min_rad, height, 0);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(radius, height, 0);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(radius, 0, 0);
				glEnd();
			}
			glPopMatrix();

			//bottom
			glBegin(GL_QUADS);
			glNormal3f(0, -1, 0);
			//glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(min_rad, 0, 0);
			glTexCoord2f(0.0f, z3 / z3);
			glVertex3f(x3, 0, z3);
			//glColorHSV(i * frac + t * color_time_mult, 1, 1);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x2, 0, z2);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(radius, 0, 0);
			glEnd();

			//railing

			glBindTexture(GL_TEXTURE_2D, textures[4]);

			//glColorHSV((i - 1) * frac + t * color_time_mult + 120, 1, 1);
			{

				//railing front
				glNormal3f(0, 0, 1);

				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(radius - border_size, height, 0);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(radius, height, 0);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(radius, height + border_height, 0);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(radius - border_size, height + border_height, 0);
				glEnd();

				//railing back
				glNormal3f(0, 0, -1);

				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(flerp(radius - border_size, border_x, border_frac), height, flerp(0, border_z, border_frac));
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(flerp(radius, x2, border_frac), height, flerp(0, z2, border_frac));
				glEnd();

				//railing right top
				glNormal3f(1, 0, 0);

				glBegin(GL_QUADS);
				//glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(radius, height + border_height, 0);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(radius, height + border_height - border_size, 0);
				//glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);d
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(x2, height + border_height - border_size, z2);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(x2, height + border_height, z2);
				glEnd();

				//railing right bottom
				glNormal3f(1, 0, 0);

				glBegin(GL_QUADS);
				//glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(radius, height, 0);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(flerp(radius, x2, border_frac), height, flerp(0, z2, border_frac));
				//glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(radius, height + border_height - border_size, 0);
				glEnd();

				//railing left top
				glNormal3f(-1, 0, 0);

				glBegin(GL_QUADS);
				//glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(border_x, height + border_height - border_size, border_z);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(radius - border_size, height + border_height - border_size, 0);
				//glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(radius - border_size, height + border_height, 0);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(border_x, height + border_height, border_z);
				glEnd();

				//railing left bottom
				glNormal3f(-1, 0, 0);

				glBegin(GL_QUADS);
				//glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(flerp(radius - border_size, border_x, border_frac), height, flerp(0, border_z, border_frac));
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(radius - border_size, height, 0);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(radius - border_size, height + border_height - border_size, 0);
				//glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult + 240, 1, 1);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
				glEnd();

				//railing bottom
				glNormal3f(0, -1, 0);

				glBegin(GL_QUADS);
				//glColorHSV((i - 1 + border_frac) * frac + t * color_time_mult, 1, 1);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(flerp(radius, x2, border_frac), height + border_height - border_size, flerp(0, z2, border_frac));
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(flerp(radius - border_size, border_x, border_frac), height + border_height - border_size, flerp(0, border_z, border_frac));
				//glColorHSV(i * frac + t * color_time_mult, 1, 1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(border_x, height + border_height - border_size, border_z);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(x2, height + border_height - border_size, z2);
				glEnd();

				//railing top
				glBegin(GL_QUADS);
				glNormal3f(0, 1, 0);

				//glColorHSV((i - 1) * frac + t * color_time_mult, 1, 1);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(radius - border_size, height + border_height, 0);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(radius, height + border_height, 0);
				//glColorHSV(i * frac + t * color_time_mult, 1, 1);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(x2, height + border_height, z2);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(border_x, height + border_height, border_z);
				glEnd();
			}

			//right

			glBegin(GL_QUADS);

			//glColorHSV((i - 1) * frac + t * color_time_mult + 240, 1, 1);
			glNormal3f(1, 0, 0);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(radius, 0, 0);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(x2, 0, z2);
			//glColorHSV(i * frac + t * color_time_mult + 240, 1, 1);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(x2, height, z2);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(radius, height, 0);
			glEnd();
			glRotatef(frac, 0, 1, 0);
			glTranslatef(0, height, 0);
		}
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawAngels()
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	for (int i = 0; i < num_angels; i++)
	{
		angels[i].render();
	}
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
}

void drawScene()
{
	drawStairs();
	drawAngels();
	DrawSkyBox(3);
	glEnable(GL_BLEND);
	glBlendColor(day_state, day_state, day_state, day_state);
	glBlendFunc(GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_COLOR);
	glPushMatrix();
	glScalef(0.999, 0.999, 0.999);
	DrawSkyBox(5);
	glPopMatrix();
	glDisable(GL_BLEND);
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

	dir.x = 5 * sin(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.z = 5 * cos(rot.x * DEGMULT) * cos(rot.y * DEGMULT);
	dir.y = 5 * sin(rot.y * DEGMULT);
	dir += pos;

	gluLookAt(pos.x, pos.y, pos.z, dir.x, dir.y, dir.z, 0, 1, 0);

	dir -= pos;

	// draw
	drawAxes();
	drawLights();
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
	case 'c':
	case 'C':
		if (!jumping)
		{
			curVel = 0;
		}
		no_clip = !no_clip;
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
	{
		deltaT = curTime - t;
	}
	t = curTime;

	day_time += deltaT / day_length;

	day_state = (sqrt((1 + day_contrast * day_contrast) / (1 + day_contrast * day_contrast * pow(cos(PI * day_time), 2))) * cos(PI * day_time) + 1) / 2;

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
		if (!no_clip && magn > 1 - (border_size / radius) - 0.01)
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

		if (jumping || no_clip)
		{
			curVel += grav * deltaT;
			pos.y += curVel * deltaT;
			GLfloat stdY = headBob(pos_state_y) * height + y_offset;
			if (pos.y <= stdY && !no_clip)
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(wScreen, hScreen);
	glutInitWindowPosition(300, 100);
	glutCreateWindow("{jh,pjmm}@dei.uc.pt|       |FaceVisivel:'f'|      |Observador:'SETAS'|        |Andar-'a/s'|        |Rodar -'e/d'| ");

	//loadShaders();
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
