#include "angel.hpp"

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
    GLfloat emission[] = {0.5, 0.5, 0.5, 0.0};
    float white[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT, white);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_EMISSION, emission);
    glMaterialfv(GL_BACK, GL_EMISSION, emission);
    glPushMatrix();
    {
        glTranslatef(cos(angle * DEGMULT) * radius, height, sin(angle * DEGMULT) * radius);
        glRotatef(-angle + 90, 0, 1, 0);
        glBegin(GL_POLYGON);
        {
            glNormal3f(0, 0, 1);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-size / 2, -size / 2, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(size / 2, -size / 2, 0);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(size / 2, size / 2, 0);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-size / 2, size / 2, 0);
        }
        glEnd();
    }
    glPopMatrix();
    float zeroArr[] = {0, 0, 0, 0};
    glMaterialfv(GL_FRONT, GL_EMISSION, zeroArr);
    glMaterialfv(GL_BACK, GL_EMISSION, zeroArr);
}

void Angel::update(GLfloat deltaT)
{
    angle += speed * deltaT;
}

float Angel::angleTo(GLfloat ang)
{
    return atan2(sin(angle * DEGMULT - ang * DEGMULT), cos(angle * DEGMULT - ang * DEGMULT)) / DEGMULT;
}