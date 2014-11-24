#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>


using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
    ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
          hasTex = false;
          //miracle: fix bug!!!!
          //alpha = 1.0f;
          this->a = a;
          this->b = b;
          this->c = c;
          this->material = m;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin)
    {
        Matrix3f A(a[0]-b[0],a[0]-c[0],ray.getDirection()[0],
                   a[1]-b[1],a[1]-c[1],ray.getDirection()[1],
                   a[2]-b[2],a[2]-c[2],ray.getDirection()[2]
                  );
        float Adeterminant = A.determinant();
        if(Adeterminant == 0.0f)
            return false;
        Matrix3f matrix1(a[0]-ray.getOrigin()[0],a[0]-c[0],ray.getDirection()[0],
                         a[1]-ray.getOrigin()[1],a[1]-c[1],ray.getDirection()[1],
                         a[2]-ray.getOrigin()[2],a[2]-c[2],ray.getDirection()[2]
                        );
        Matrix3f matrix2(a[0]-b[0],a[0]-ray.getOrigin()[0],ray.getDirection()[0],
                         a[1]-b[1],a[1]-ray.getOrigin()[1],ray.getDirection()[1],
                         a[2]-b[2],a[2]-ray.getOrigin()[2],ray.getDirection()[2]
                        );
        Matrix3f matrix3(a[0]-b[0],a[0]-c[0],a[0]-ray.getOrigin()[0],
                         a[1]-b[1],a[1]-c[1],a[1]-ray.getOrigin()[1],
                         a[2]-b[2],a[2]-c[2],a[2]-ray.getOrigin()[2]
                        );
        float beta = matrix1.determinant()/Adeterminant;
        float gamma = matrix2.determinant()/Adeterminant;
        float alpha = 1-beta-gamma;
        if(alpha < 0 || alpha > 1 || beta < 0 || beta > 1 || gamma < 0 || gamma > 1)
            return false;
        if(beta+gamma > 1)
            return false;

        float tt = matrix3.determinant()/Adeterminant;
        float current_t = hit.getT();
        if(tt < current_t && tt > tmin)
        {
            Vector3f normal = alpha * normals[0] + beta * normals[1] + gamma * normals[2];
            //bug
            //normal.normalized();
            normal.normalize();

            Vector2f tex = alpha * texCoords[0] + beta * texCoords[1] + gamma * texCoords[2];
            //this->material->setTexCoord(tex);
            hit.set(tt, this->material, normal);
            return true;
        }
        return false;
	}

	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];

protected:
    float alpha;
    float beta;
    float gamma;
    Vector3f a, b, c;
    Material* material;
};

#endif //TRIANGLE_H
