#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define ELIPSON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
//镜面反射方向计算
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
    //normalized already
    return Vector3f(incoming-2*Vector3f::dot(incoming, normal)*normal).normalized();
}

//折射方向计算返回值是bool值
bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming,
                           float index_n, float index_nt, Vector3f& transmitted)
{
    float inner = 1 - (pow(index_n,2)*(1 - pow(Vector3f::dot(incoming,normal),2))/pow(index_nt,2));
    if(inner < 0)
        return false;
    Vector3f t = index_n*(incoming-normal*Vector3f::dot(incoming,normal))/index_nt - normal*sqrt(inner);
    //normalized already
    transmitted = t.normalized();
    return true;
}

RayTracer::RayTracer( SceneParser * scene, int max_bounces
  //more arguments if you need...
                      ) : m_scene(scene)
{
    g=scene->getGroup();
    m_maxBounces = max_bounces;
}

RayTracer::~RayTracer()
{
}


bool RayTracer::castShadows(Ray& ray, Hit& hit, Vector3f& dir) const
{
    Ray newray = Ray(ray.pointAtParameter(hit.getT()), dir);
    Hit newhit;
    float ep = 0.001;
    bool intersect = g->intersect(newray, newhit, ep);
    return !intersect;//Miracle??? true==没有障碍物阻挡 false==有东西相交挡住了光
}

float calculateR(float R0, float c)
{
    return R0 + (1-R0)*pow(1-c,5);
}

//current refractionindex in ==> refr_index
Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces,
                              float refr_index, Hit& hit ) const
{
    //hit = Hit( FLT_MAX, NULL, Vector3f( 0, 0, 0 ) );
    //return Vector3f(0,0,0);
    Vector3f color = Vector3f(0,0,0);
    if(g->intersect(ray, hit, tmin))
    {
        float tCurrent = hit.getT();
        Material* m = hit.getMaterial();
        Vector3f p = ray.pointAtParameter(tCurrent);

        /////////////////////////////////////////////////
        //do this regardless of reflection and refraction
        /////////////////////////////////////////////////
        for(int k = 0;k < m_scene->getNumLights();++k)
        {
            Light* current_light = m_scene->getLight(k);
            //generate ray to to light
            Vector3f dir, col;
            float dis = 0.0f;//in fact not in use
            //得到当前光l的dir, col
            current_light->getIllumination(p,dir,col,dis);

            Ray nowray(p,dir);
            Hit nowhit(dis, NULL, Vector3f());
            if(castShadows(ray, hit, dir) || !g->intersect(nowray, nowhit, ELIPSON))
            {
                color = color + m->Shade(ray, hit, dir, col);
            }
        }

        //reflection and refraction should care the bounces number
        Vector3f reflectColor(0,0,0);
        Vector3f refractColor(0,0,0);
        if(bounces < m_maxBounces)
        {
              //////////////////////////////////////////////////////
              //                reflection
              //////////////////////////////////////////////////////
              {
                  Vector3f mirrordir = mirrorDirection(hit.getNormal(),ray.getDirection());
                  Ray ray2(p, mirrordir);
                  Hit reflectHit;
                  reflectColor = m->getSpecularColor() * traceRay(ray2, ELIPSON, bounces+1, refr_index, reflectHit);
              }

              //////////////////////////////////////////////////////
              //                refraction
              //////////////////////////////////////////////////////

              float newIndex;
              Vector3f newNormal;
              //Miracle : still confused with it
              //从里面出来
              if(Vector3f::dot(hit.getNormal(),ray.getDirection())>0)
              {
                  newIndex = 1.0;
                  newNormal = -1 * hit.getNormal();
              }
              //进去
              else
              {
                  newIndex = m->getRefractionIndex();
                  newNormal = hit.getNormal();
              }
              if(newIndex > 0)
              {
                  Vector3f t;
                  if(transmittedDirection(newNormal,ray.getDirection(),refr_index, newIndex, t))
                  {
                      //t already normalized
                      Ray ray3(p, t);
                      Hit refractHit;
                      refractColor = m->getSpecularColor() * traceRay(ray3, ELIPSON, bounces+1, newIndex, refractHit);

                      float c;
                      if(refr_index > newIndex)
                          c = abs(Vector3f::dot(t, newNormal));
                      else
                          c = abs(Vector3f::dot(ray.getDirection(),newNormal));
                      float R0 = pow((newIndex-refr_index)/(newIndex+refr_index), 2);

                      float R = calculateR(R0, c);

                      color = color + R*reflectColor + (1-R)*refractColor;
                      return color;
                  }
                  else
                      return color+reflectColor;
              }
              return color+reflectColor;
        }
        //++NotIntersect;
        return color;
    }
    //没有相交的就返回背景颜色
    else
        return m_scene->getBackgroundColor(ray.getDirection());
}
