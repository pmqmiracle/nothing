#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;

//normal direction!!!!!!!!!!!

///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m)
    {
        this->normal = -1 * normal;
        this->d = d;
        this->material = m;
	}
	~Plane(){}
	virtual bool intersect( const Ray& r , Hit& h , float tmin)
    {
        float current_t = h.getT();
        float tmp = Vector3f::dot(this->normal, r.getDirection());
        if(tmp == 0.0f)
            return false;
        float tt = -(this->d+Vector3f::dot(this->normal,r.getOrigin()))/tmp;
        if(tt > tmin && tt < current_t)
        {
            //miracle Nov23
            //need surface normal for shading
            //
            //need to debug!!!!!!!!!!!111
            if(tmp > 0.0)
                h.set(tt, this->material, -1 * this->normal);
            else
                h.set(tt, this->material, this->normal);
            return true;
        }
        else
            return false;
	}

protected:
    Vector3f normal;
    float d;

    Material* material;
};
#endif //PLANE_H


