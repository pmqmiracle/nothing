#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ;

	virtual float getTMin() const = 0 ;
	virtual ~Camera(){}
protected:
	Vector3f center;
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle)
    {
        w = direction;
        u = Vector3f::cross(w, up);
        v = Vector3f::cross(u, w);
        w.normalized();
        u.normalized();
        v.normalized();
        this->angle = angle;
    }

    //miracle
    //confusing??
	virtual Ray generateRay( const Vector2f& point)
    {
        Vector3f dir = Vector3f(point,0)-this->direction;
        dir.normalized();
        return Ray(this->center,dir);
	}

	virtual float getTMin() const {
		return 0.0f;
	}

private:
    float angle;
    Vector3f u, v, w;
};

#endif //CAMERA_H
