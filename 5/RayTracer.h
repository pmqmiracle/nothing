#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "SceneParser.h"
#include "Ray.h"
#include "Hit.h"

class SceneParser;

static int countIntersect = 0;
static int NotIntersect = 0;

class RayTracer
{
public:

  RayTracer()
  {
      assert( false );
  }

  RayTracer( SceneParser* scene, int max_bounces //more arguments as you need...
  );

  ~RayTracer();

  Vector3f traceRay( Ray& ray, float tmin, int bounces,
                     float refr_index, Hit& hit ) const;

  bool castShadows(Ray& ray, Hit& hit, Vector3f& dir) const;

private:
  SceneParser* m_scene;//a pointer of SceneParser for access to the geometry and light sources of the scene
  int m_maxBounces;

  Group* g;
};

#endif // RAY_TRACER_H
