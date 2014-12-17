#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#include <GL/glut.h>

#include "particleSystem.h"
#include <ctime>

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int m);
	vector<Vector3f> evalF(vector<Vector3f> state);

    //miracle
    Vector3f evalF(vector<Vector3f> &state, int index_i, int index_j);

	void draw();
    vector<Vector3f> m_normals;

    static float mass;
    static Vector3f g;

    static float drag_k;

    static float DIST_BETWEEN_POINTS;
    static float sqrt2;

    //structural springs
    static float structural_k;
    static float structural_restLength;
    //shear springs
    static float shear_k;
    static float shear_restLength;
    //flexion springs
    static float flexion_k;
    static float flexion_restLength;

    //wind, noise
    static float MAX_WIND;
    Vector3f wind;
    static float wind_noise;
    static float getRandomFloat()
    {
        return 1.0f*rand()/(RAND_MAX);
    }


    Vector3f getVelocity(int particle_index);
    Vector3f getPosition(int particle_index);

    int linearIndex(int i, int j);
    bool inGrid(int i,int j);


private:
};

#endif
