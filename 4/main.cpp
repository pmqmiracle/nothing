#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
  // Fill in your implementation here.

  // This loop loops over each of the input arguments.
  // argNum is initialized to 1 because the first
  // "argument" provided to the program is actually the
  // name of the executable (in our case, "a4").
  char* inputFile;
  char* outputFile;
  int width;
  int height;
  for( int argNum = 1; argNum < argc; ++argNum )
  {
     std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
     if(strcmp(argv[argNum],"-input")==0)
     {
         inputFile = argv[++argNum];
     }
     if(strcmp(argv[argNum],"-size")==0)
     {
         width = atoi(argv[++argNum]);
         height = atoi(argv[++argNum]);
     }
     if(strcmp(argv[argNum],"-output")==0)
     {
         outputFile = argv[++argNum];
     }
  }

  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene. Write the color at the intersection to that
  // pixel in your output image.
  const char* final_output = outputFile;
  SceneParser sp = SceneParser(inputFile);
  Camera* camera = sp.getCamera();
  Group* group = sp.getGroup();
  float t_min = 0.0f;
  Material* m;
  Image image(width,height);
  for(int i = 0;i < width;++i)
  {
      for(int j = 0;j < height;++j)
      {
          Ray ray = camera->generateRay(Vector2f(2.0*i/width - 1,2.0*j/height - 1));
          Hit hit;
          float tCurrent;
          if(group->intersect(ray, hit, t_min))
          {
              tCurrent = hit.getT();
              m = hit.getMaterial();
              Vector3f color;//()
              Vector3f p = ray.pointAtParameter(tCurrent);
              for(int k = 0;k < sp.getNumLights();++k)
              {
                  Light* l = sp.getLight(k);
                  //generate ray to to light
                  Vector3f dir, col;
                  float dis = 0.0f;//in fact not in use
                  l->getIllumination(p,dir,col,dis);
                  Ray r2 = Ray(p, dir);
                  Hit h2 = Hit();
                  color = color + m->Shade(ray, hit, dir, col);
              }
              color = color + sp.getAmbientLight();
              image.SetPixel(i,j,color);
          }
          else
              image.SetPixel(i,j,sp.getBackgroundColor());
      }
  }
  image.SaveImage(final_output);

  ///TODO: below demonstrates how to use the provided Image class
  ///Should be removed when you start
  /*
  Vector3f pixelColor (1.0f,0,0);
  //width and height
  Image image( 10 , 15 );
  image.SetPixel( 5,5, pixelColor );
  image.SaveImage("demo.bmp");
  */
  return 0;
}

