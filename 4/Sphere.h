#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere : public Object3D
{
public:
	Sphere(){
		//unit ball at the center
        center = Vector3f(0,0,0);
        radius = 1.0f;
        material = NULL;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
        this->center = center;
        this->radius = radius;
        this->material = material;
	}

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin)
    {
        //ray-sphere intersection
        float a = 1.0f;
        float b = 2*Vector3f::dot(r.getDirection(),r.getOrigin()-this->center);
        float c = Vector3f::dot(r.getOrigin()-this->center, r.getDirection()-this->center)-radius*radius;

        //这个只是球的中心在(0,0,0)时候的特例 pay attention
        //float b = 2*Vector3f::dot(r.getDirection(),r.getOrigin());
        //float c = Vector3f::dot(r.getOrigin(),r.getOrigin())-radius*radius;

        float inner = b*b-4*a*c;
        if(inner<0)
            return false;
        float t1 = (-b+sqrt(inner))/(2*a);
        float t2 = (-b-sqrt(inner))/(2*a);
        float result = -1;
        if(t1 < t2 && t1 > 0)
            result = t1;
        else if (t1 > t2 && t2 > 0)
            result = t2;
        if(result > 0 && result > tmin && result < h.getT())
        {
            Vector3f t_normal = (r.getOrigin()+result*r.getDirection())-(this->center);
            t_normal = t_normal.normalized();
            h.set(result, this->material, t_normal);
            return true;
        }
        return false;
	}

protected:
    Vector3f center;
    float radius;
};

#endif
