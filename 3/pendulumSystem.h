#ifndef PENDULUMSYSTEM_H
#define PENDULUMSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "particleSystem.h"

class PendulumSystem: public ParticleSystem
{
public:
	PendulumSystem(int numParticles);

	vector<Vector3f> evalF(vector<Vector3f> state);
    Vector3f evalF(vector<Vector3f> state, int index);

	void draw();

    static float mass;
    static float k1;
    static float k2;
    static float restLength;
    static Vector3f g;

    Vector3f getVelocity(int particle_index);
    Vector3f getPosition(int particle_index);

};

#endif
