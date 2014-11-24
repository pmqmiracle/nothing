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
    //virtual float getAngle() const = 0;
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
        this->ratio = 1;

        w = direction.normalized();
        v = up.normalized();
        //bug
        //u = Vector3f::cross(w, v);
        //u.normalized();
        //the above is different from the below: a+b is not equal to a += b
        u = Vector3f::cross(w,v).normalized();
        /*
        w = direction.normalized();
        u = Vector3f::cross(w, up).normalized();
        v = Vector3f::cross(u,w).normalized();
        */

    }

    /*float getAngle()
    {
        return angle;
    }*/

    //miracle
	virtual Ray generateRay( const Vector2f& point)
    {
        float D = 1.0/tan((this->angle)*0.5);
        //Vector3f r = (point[0]*this->u + this->ratio*point[1]*this->v + D*this->w);
        Vector3f r = (point[0]*this->u + point[1]*this->v + D*this->w);
        static int count = 0;
        if(count < 3)
            cout << "before normalized : " << r[0] << ", " << r[1] << ", " << r[2] << endl;
        ///////////////////////////////////////////////////////////////////////////////////////
        //NOV24 super bug!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //注意normalize()和normalized()的差别!!!
        r.normalize();
        ////////////////////////////////////////////////////////////////////////////////////////////
        if(count < 3)
            cout << "after normalized : " << r[0] << ", " << r[1] << ", " << r[2] << endl;
        if(count++ < 3)
        {
            cout << D << endl;
            cout << "point :" << point[0] << ", " << point[1] << endl;
            cout << "U: " << u[0] << ", " << u[1] << ", " << u[2] << endl;
            cout << "V: " << v[0] << ", " << v[1] << ", " << v[2] << endl;
            cout << "W: " << w[0] << ", " << w[1] << ", " << w[2] << endl;
            cout << r[0] << ", " << r[1] << ", " << r[2] << endl;
        }
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
