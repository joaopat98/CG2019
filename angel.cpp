#include "angel.hpp"

float frand()
{
    return rand() / (float)RAND_MAX;
}

Angel::Angel()
{
}

Angel::Angel(GLfloat base_height, GLfloat delta_height, GLfloat base_rad, GLfloat delta_rad, GLfloat speed, GLfloat size, int index)
{
    angle = frand() * 360;
    height = base_height + delta_height * frand();
    radius = base_rad + delta_rad * frand();
    this->speed = speed;
    this->size = size;
    this->index = index;
}

void Angel::render()
{
    glPushMatrix();
    {
        glTranslatef(cos(angle * DEGMULT) * radius, height, sin(angle * DEGMULT) * radius);
        glRotatef(-angle + 90, 0, 1, 0);
        glColor3f(1, 1, 1);
        glBegin(GL_POLYGON);
        {
            glVertex3f(-size / 2, -size / 2, 0);
            glVertex3f(size / 2, -size / 2, 0);
            glVertex3f(size / 2, size / 2, 0);
            glVertex3f(-size / 2, size / 2, 0);
        }
        glEnd();
    }
    glPopMatrix();
}

void Angel::update(GLfloat deltaT)
{
    angle += speed * deltaT;
}