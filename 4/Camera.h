#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>

//for non-square viewports
//#include "Image.h"

class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ;

    //miracle
    virtual void setRatio(float r) = 0;

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
        this->angle = angle;
        this->center = center;
        w = direction.normalized();
        v = up.normalized();
        //u = Vector3f::cross(w, up);
        //u.normalized();
        u = Vector3f::cross(w, v);
        v.normalized();

        this->ratio = 1;
    }

    //miracle
	virtual Ray generateRay( const Vector2f& point)
    {
        float D = 1.0/tan(this->angle*0.5);
        //Vector3f r = (point[0]*this->u + this->ratio*point[1]*this->v + D*this->w);
        Vector3f r = (point[0]*this->u + point[1]*this->v + D*this->w);
        r.normalized();
        return Ray(this->center, r);
	}
    //miracle
    virtual void setRatio(float r)
    {
        this->ratio = r;
    }

	virtual float getTMin() const {
		return 0.0f;
	}

private:
    float angle;
    Vector3f center;
    Vector3f u, v, w;

    float ratio;
};

#endif //CAMERA_H
