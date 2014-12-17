#include "pendulumSystem.h"

//当chain钟摆模式下粒子的数目很多的时候,不稳定性会增大需要更加多地调整系数
float PendulumSystem::DISTANCE_BETWEEN_PARTICLE = 0.25f;
float PendulumSystem::mass = 1.0f;
float PendulumSystem::k1 = 0.1f;//0.1f;
float PendulumSystem::k2 = 1.0f;//5.0f
float PendulumSystem::restLength = 0.15f;//0.25f;
Vector3f PendulumSystem::g = Vector3f(0,-0.1,0);//(0,-1,0);

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;

	// fill in code for initializing the state based on the number of particles
    vector<Vector3f> vv;
	for (int i = 0; i < m_numParticles; i++) {
		// for this system, we care about the position and the velocity
        // 偶数 position
        //vv.push_back(Vector3f(0.5*i+1,0.5*i+1,0.5*i+1));
        vv.push_back(DISTANCE_BETWEEN_PARTICLE*Vector3f(i+2,3,2));
        // 奇数 velocity
        vv.push_back(Vector3f(0,0,0));
	}
    setState(vv);
    setTime(0);
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE
    // f六元组(v,F/m)===>(v1,v2,v3,a1,a2,a3)
    // 第一个particle对应的f(X,t) 就是它的速度
    f.push_back(state[1]);
    //固定第一个,初始为0
    f.push_back(Vector3f(0,0,0));
    for(int k = 1;k < m_numParticles;++k)
    {
        //速度
        f.push_back(state[2*k+1]);
        //F/m
        f.push_back(evalF(state, k));
    }

	return f;
}

//November 19, 2014 fix bug, the same as Clothsystem did.
Vector3f PendulumSystem::evalF(vector<Vector3f> &state, int index)
{
    Vector3f f_m;
    Vector3f gravity = mass*g;
    //Vector3f force1 = -1 * k1 * getVelocity(index);
    Vector3f force1 = -1 * k1 * state[2*index+1];
    Vector3f force2(0,0,0);
    //bug 一个粒子不是单纯被它旁边两个粒子影响
    /*
    if(index > 0 && index < m_numParticles-1)
    {
        Vector3f left = getPosition(2*index-2);
        Vector3f right = getPosition(2*index+2);
        Vector3f distance = left - right;
        float len = distance.abs();
        force2 = -1 * k2 * len - restLength * distance.normalized();
    }
    */
    if(index > 0)
    {
        //Vector3f left = getPosition(2*index-2);
        //Vector3f diff = getPosition(index)-left;
        //fix
        Vector3f left = state[2*index-2];
        Vector3f diff = state[2*index]-left;
        force2 += -1 * k2 * (diff.abs() - restLength) * diff.normalized();
    }
    if(index < m_numParticles-1)
    {
        //fix
        Vector3f right = state[2*index+2];
        Vector3f diff = state[2*index]-right;
        force2 += -1 * k2 * (diff.abs() - restLength) * diff.normalized();
    }
    f_m = (gravity+force1+force2)/mass;
    return f_m;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
    //draw lines
    glLineWidth(3);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    for(int i = 0;i < m_numParticles-1;++i)
    {
        Vector3f now = m_vVecState[2*i];
        Vector3f next = m_vVecState[2*(i+1)];
        glVertex3f(now[0], now[1], now[2]);
        glVertex3f(next[0], next[1], next[2]);
    }
    glEnd();


    glEnable(GL_LIGHTING);
    //draw particles
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = getState()[2*i];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		//glutSolidSphere(0.075f,10.0f,10.0f);
		glutSolidSphere(0.1f,8.0f,8.0f);
		glPopMatrix();
	}
}

Vector3f PendulumSystem::getVelocity(int particle_index)
{
    return m_vVecState[2*particle_index+1];
}

Vector3f PendulumSystem::getPosition(int particle_index)
{
    return m_vVecState[2*particle_index];
}
