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
    vector<Vector3f> v1 = particleSystem->evalF(x1);
    vector<Vector3f> result;
    for(unsigned int i = 0;i < x0.size();++i)
    {
        result.push_back(x0[i]+0.5*stepSize*(v0[i]+v1[i]));
    }
    particleSystem->setState(result);
    //particleSystem->setTime(particleSystem->current_time+stepSize);
}
