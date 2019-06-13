#include "particles.h"

ParticleSystem::ParticleSystem(vec3 pos, vec3 color, int num_particles, GLfloat speed, GLfloat particle_size, GLuint texture, void (*init_fn)(Particle *, int), void (*update_fn)(Particle *, GLfloat), void (*render_fn)(Particle *), int type)
{
    particles = (Particle *)malloc(num_particles * sizeof(Particle));
    if (init_fn)
        init_fn(particles, num_particles);
    else
    {
        for (int i = 0; i < num_particles; i++)
        {
            vec3 dir = normalize(vec3{frand() * 2 - 1, frand() * 2 - 1, frand() * 2 - 1});
            particles[i] = Particle(pos, speed * frand() * dir, color, particle_size, texture, update_fn, render_fn);
        }
    }
    this->pos = pos;
    this->num_particles = num_particles;
    this->type = type;
}

ParticleSystem::~ParticleSystem()
{
    free(particles);
}

void ParticleSystem::update(GLfloat deltaT)
{
    for (int i = 0; i < num_particles; i++)
        particles[i].update(deltaT);
}

void ParticleSystem::reset()
{
    for (int i = 0; i < num_particles; i++)
        particles[i].reset();
}

void ParticleSystem::render(vec3 player)
{
    for (int i = 0; i < num_particles; i++)
        particles[i].render(player);
}

Particle::Particle(vec3 init_pos, vec3 dir, vec3 color, GLfloat size, GLuint texture, void (*update_fn)(Particle *, GLfloat), void (*render_fn)(Particle *))
{
    this->init_pos = init_pos;
    pos = init_pos;
    this->dir = dir;
    this->color = color;
    this->size = size;
    this->texture = texture;
    this->update_fn = update_fn;
    this->render_fn = render_fn;
}

Particle::~Particle()
{
}

void Particle::reset()
{
    pos = init_pos;
}
void Particle::update(GLfloat deltaT)
{
    if (update_fn)
        update_fn(this, deltaT);
    else
        pos += dir * deltaT;
}
void Particle::render(vec3 player)
{
    GLfloat pi = 3.14159;
    GLfloat degmult = pi / 180;
    vec3 offset = player - pos;
    GLfloat theta = atan2(offset.x, offset.z) / degmult;
    GLfloat phi = atan2(sqrt(pow(offset.x, 2) + pow(offset.z, 2)), offset.y) / degmult;
    colorv(this->color);

    glPushMatrix();
    {
        translate(pos);
        glRotatef(theta, 0, 1, 0);
        glRotatef(phi + 90, 1, 0, 0);
        if (render_fn)
            render_fn(this);
        else
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glEnable(GL_BLEND);
            glAlphaFunc(GL_GREATER, 0.5);
            glEnable(GL_ALPHA_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBegin(GL_QUADS);
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex3f(-size / 2, -size / 2, 0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex3f(-size / 2, size / 2, 0);
                glTexCoord2f(1.0f, 1.0f);
                glVertex3f(size / 2, size / 2, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex3f(size / 2, -size / 2, 0);
            }
            glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_TEXTURE_2D);
        }
    }
    glPopMatrix();
}