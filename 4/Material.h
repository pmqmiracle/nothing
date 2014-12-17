#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"

///bug!!!
//#include "SceneParser.h"
//这个include导致circular include的一系列inheritance的错误

///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:

  Material(const Vector3f& d_color,const Vector3f& s_color=Vector3f::ZERO, float s=0):
           diffuseColor( d_color),specularColor(s_color), shininess(s)
  {
  }

  virtual ~Material()
  {
  }

  virtual Vector3f getDiffuseColor() const
  {
    return  diffuseColor;
  }

  void setTexCoord(Vector2f texCoords)
  {
      this->texCoords = texCoords;
  }

  //miracle no ambient now.....
  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor)
  {
      //specular component in Phong shading
      Vector3f color;
      if(this->t.valid())
          color = t(texCoords[0],texCoords[1]);
      else
          color = diffuseColor;
      //diffuse
      //float Lo_r = color[0]*max(0.0f,Vector3f::dot(hit.getNormal(),dirToLight))*lightColor[0];
      //float Lo_g = color[1]*max(0.0f,Vector3f::dot(hit.getNormal(),dirToLight))*lightColor[1];
      //float Lo_b = color[2]*max(0.0f,Vector3f::dot(hit.getNormal(),dirToLight))*lightColor[2];
      Vector3f partDiffuse = color*max(0.0f, Vector3f::dot(hit.getNormal(),dirToLight))*lightColor;
      //specular
      Vector3f R = ray.getDirection()-2*Vector3f::dot(ray.getDirection(),hit.getNormal())*hit.getNormal();
      float LR = Vector3f::dot(dirToLight, R);
      if(LR < 0.0f)
          LR = 0.0f;
      float specular = pow(LR, shininess);
      //Dec 10, 2014
      //改变应该是只有diffuse color得时候需要考虑是否选择diffusecolor 还是 texture color, specular得时候不需要?
      //if(this->t.valid())
          //return Vector3f(Lo_r,Lo_g,Lo_b) + specular*t(texCoords[0],texCoords[1])*lightColor;
      //    return partDiffuse + specular*t(texCoords[0],texCoords[1])*lightColor;
      //else
          //return Vector3f(Lo_r,Lo_g,Lo_b) + specular*specularColor*lightColor;

      //Dec 10, 2014 test diffuse, specular's effects

          //only diffuse
          //return partDiffuse;
          //only specular
          //return specular * specularColor * lightColor;
          //All:diffuse+specular
          return partDiffuse + specular * specularColor * lightColor;
  }

  void loadTexture(const char * filename)
  {
      t.load(filename);
  }

protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;

  Texture t;
  Vector2f texCoords;
};
#endif // MATERIAL_H
