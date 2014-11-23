#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
#include "VecUtils.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public:
  Transform(){}
  Transform( const Matrix4f& m, Object3D* obj ):o(obj)
  {
      this->M = m;//matrix
  }
  ~Transform()
  {
  }
  virtual bool intersect( const Ray& r , Hit& h , float tmin)
  {
      //transform the ray from world space to object space
      Matrix4f Minverse = this->M.inverse();
      Vector3f newO = VecUtils::transformPoint(Minverse, r.getOrigin());
      Vector3f newD = VecUtils::transformDirection(Minverse, r.getDirection());
      newD.normalized();
      Ray newR(newO, newD);
      //////////////////////////////////////////////
      //转换完ray后,才开始intersect routine
      if(o->intersect( newR , h , tmin))
      {
            //correctly transform the resulting normal
            Vector3f newNormal = this->M.getSubmatrix3x3(0,0).inverse().transposed()*h.getNormal();
            newNormal.normalized();

            h.set(h.getT(), h.getMaterial(), newNormal);
            return true;
      }
      return false;
  }

protected:
  Object3D* o; //un-transformed object
  Matrix4f M;
};

#endif //TRANSFORM_H
