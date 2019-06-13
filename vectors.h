#include <GL/glut.h>
#include <math.h>

#ifndef VECTORS_H

#define VECTORS_H

struct vec3
{
    GLfloat x, y, z;
};

struct vec2
{
    GLfloat x, y;
};

vec3 operator+(vec3 v1, vec3 v2);

vec3 operator-(vec3 v);

vec3 operator-(vec3 v1, vec3 v2);

vec3 &operator+=(vec3 &lhs, const vec3 &rhs);

vec3 &operator-=(vec3 &lhs, const vec3 &rhs);

vec3 &operator*=(vec3 &lhs, GLfloat c);

vec3 &operator/=(vec3 &lhs, GLfloat c);

vec3 operator*(vec3 v, GLfloat c);

vec3 operator*(GLfloat c, vec3 v);

vec3 operator/(vec3 v, GLfloat c);

GLfloat magnitude(vec3 v);

vec3 normalize(vec3 v);

void translate(vec3 v);

void rotate(GLfloat angle, vec3 v);

void scale(vec3 v);

void colorv(vec3 v);

#endif