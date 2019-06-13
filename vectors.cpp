#include "vectors.h"

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

vec3 &operator+=(vec3 &lhs, const vec3 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
}

vec3 &operator-=(vec3 &lhs, const vec3 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
}

vec3 &operator*=(vec3 &lhs, GLfloat c)
{
    lhs.x *= c;
    lhs.y *= c;
    lhs.z *= c;
}

vec3 &operator/=(vec3 &lhs, GLfloat c)
{
    if (c != 0)
    {
        lhs.x /= c;
        lhs.y /= c;
        lhs.z /= c;
    }
}

vec3 operator*(vec3 v, GLfloat c)
{
    return {v.x * c, v.y * c, v.z * c};
}

vec3 operator*(GLfloat c, vec3 v)
{
    return v * c;
}

vec3 operator/(vec3 v, GLfloat c)
{
    if (c == 0)
    {
        return v;
    }
    return {v.x / c, v.y / c, v.z / c};
}

GLfloat magnitude(vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3 normalize(vec3 v)
{
    return v / magnitude(v);
}

void translate(vec3 v)
{
    glTranslatef(v.x, v.y, v.z);
}

void rotate(GLfloat angle, vec3 v)
{
    glRotatef(angle, v.x, v.y, v.z);
}

void scale(vec3 v)
{
    glScalef(v.x, v.y, v.z);
}

void colorv(vec3 v)
{
    glColor3f(v.x, v.y, v.z);
}