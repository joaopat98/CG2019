#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#ifndef ANGEL_H
#define ANGEL_H

#define PI 3.14159
#define DEGMULT PI / 180

float frand();

class Angel
{
  private:
    GLfloat angle;
    GLfloat radius;
    GLfloat height;
    GLfloat speed;
    GLfloat size;

  public:
    Angel();
    Angel(GLfloat base_height, GLfloat delta_height, GLfloat base_rad, GLfloat delta_rad, GLfloat speed, GLfloat size);
    void render();
    void update(GLfloat deltaT);
};

#endif