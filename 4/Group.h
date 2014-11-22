#ifndef GROUP_H
#define GROUP_H

#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>

using  namespace std;

///TODO:
///Implement Group
///Add data structure to store a list of Object*
class Group:public Object3D
{
public:

  Group(){
      objectSize = 0;
      group.clear();
  }

  Group( int num_objects ){
      objectSize = num_objects;
  }

  ~Group(){
      objectSize = 0;
      group.clear();
  }

  virtual bool intersect( const Ray& r , Hit& h , float tmin )
  {
      bool flag = false;
      for(int i = 0;i < objectSize;++i)
      {
          if(group[i]->intersect(r,h,tmin))
              flag = true;
      }
      return flag;
  }

  //miracle : index for what???
  void addObject( int index , Object3D* obj )
  {
      group.push_back(obj);
  }

  int getGroupSize()
  {
      return objectSize;
  }

private:
  int objectSize;
  vector<Object3D*> group;
};

#endif

