#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> x = particleSystem->getState();
    vector<Vector3f> v = particleSystem->evalF(x);
    vector<Vector3f> updatex;
    for(unsigned int i = 0;i < x.size();++i)
    {
        updatex.push_back(x[i] + stepSize*v[i]);
    }
    particleSystem->setState(updatex);
    //particleSystem->setTime(particleSystem->current_time+stepSize);
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
    vector<Vector3f> x0 = particleSystem->getState();

    vector<Vector3f> v0 = particleSystem->evalF(x0);
    vector<Vector3f> x1;
    for(unsigned int i = 0;i < x0.size();++i)
        x1.push_back(x0[i]+stepSize*v0[i]);

    vector<Vector3f> v1 = particleSystem->evalF(x1);//step1: bug: 注意evalF的参数不是getState() 而是x1
    vector<Vector3f> result;
    for(unsigned int i = 0;i < x0.size();++i)
    {
        result.push_back(x0[i]+0.5*stepSize*(v0[i]+v1[i]));
    }
    particleSystem->setState(result);
    //particleSystem->setTime(particleSystem->current_time+stepSize);
}

void MyRK4::takeStep(ParticleSystem* particleSystem, float stepSize) {

    // REFERENCE: wikipedia artcie on Runge-Kutta
    // comments are the corresponding variables used in the algo on the wiki description
    vector<Vector3f> x0, x1, x2, x3, v0, v1, v2, v3, result;
    x0 = particleSystem->getState(); //y_n
    unsigned size = x0.size();
    v0  = particleSystem->evalF(x0); // f(t_n, y_n); stepsize * v0 = k1

    for (unsigned i=0; i < size; i++) {

        x1.push_back(x0[i] + 0.5 * stepSize * v0[i]); // y_n + 0.5*h*f(t_n, y_n)

    }

    v1 = particleSystem->evalF(x1); // stepSize * v1 = k2

    for (unsigned i=0; i < size; i++) {
        x2.push_back(x0[i] + 0.5 * stepSize * v1[i]);
    }

    v2 = particleSystem->evalF(x2); // stepSize * v2 = k3

    for (unsigned i=0; i < size; i++) {
        x3.push_back(x0[i] + stepSize * v2[i]);
    }
    v3 = particleSystem->evalF(x3);

    for (unsigned i=0; i < size; i++) {
        result.push_back(x0[i] + 1.0f/6.0f * stepSize * (v0[i] + 2*v1[i] + 2*v2[i] + v3[i]));
    }

    particleSystem->setState(result);
    //particleSystem->setTime(particleSystem->getTime() + stepSize);
}
