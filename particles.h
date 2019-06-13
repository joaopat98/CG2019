#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include "vectors.h"
#include "frand.h"

#ifndef PARTICLES_H

#define PARTICLES_H

class Particle
{
private:
    void (*update_fn)(Particle *, GLfloat);
    void (*render_fn)(Particle *);

public:
    vec3 pos, velocity;
    GLfloat speed, size;
    vec3 init_pos, dir, color;
    GLuint texture;
    bool ended = false;

    Particle();
    Particle(vec3 init_pos, vec3 dir, vec3 color, GLfloat size, GLuint texture, void (*update_fn)(Particle *, GLfloat), void (*render_fn)(Particle *));
    ~Particle();
    void reset();
    void update(GLfloat deltaT);
    void render(vec3 player);
};

class ParticleSystem
{
private:
    int num_particles;
    GLfloat particle_size;
    vec3 color;
    Particle *particles;

public:
    vec3 pos;
    GLfloat time = 0;
    int type;
    bool ended;

    ParticleSystem(vec3 pos, vec3 color, int num_particles, GLfloat speed, GLfloat particle_size, GLuint texture, void (*init_fn)(Particle *, int), void (*update_fn)(Particle *, GLfloat), void (*render_fn)(Particle *), int type);
    ~ParticleSystem();
    void reset();
    void update(GLfloat deltaT);
    void render(vec3 player);
};

#endif