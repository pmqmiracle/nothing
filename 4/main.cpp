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
     //std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
     if(strcmp(argv[argNum],"-input")==0)
     {
         inputFile = argv[++argNum];
         //cout << "inputFile is : " << inputFile << endl;
     }
     if(strcmp(argv[argNum],"-size")==0)
     {
         width = atoi(argv[++argNum]);
         //cout << "width : " << width << endl;
         height = atoi(argv[++argNum]);
         //cout << "height : " << height << endl;
     }
     if(strcmp(argv[argNum],"-output")==0)
     {
         outputFile = argv[++argNum];
         //cout << "OutputFile is : " << outputFile << endl;
     }
  }
  for( int argNum = 1;argNum < argc; ++argNum )
  {
      cout << "#" << argNum << " : " << argv[argNum] << endl;
  }

  // First, parse the scene using SceneParser.
  // Then loop over each pixel in the image, shooting a ray
  // through that pixel and finding its intersection with
  // the scene. Write the color at the intersection to that
  // pixel in your output image.
  const char* final_output = outputFile;
  SceneParser sp = SceneParser(inputFile);
  Camera* camera = sp.getCamera();

  //Nov23 for non-square viewport
  //camera->setRatio(width*1.0/height);

  Group* group = sp.getGroup();
  float t_min = 0.0f;
  Material* m;
  Image image(width,height);
  int countTrue = 0;
  int countFalse = 0;
  for(int i = 0;i < width;++i)
  {
      for(int j = 0;j < height;++j)
      {
          //[-1,1]
          Ray ray = camera->generateRay(Vector2f(2.0*i/width - 1,2.0*j/height - 1));
          Hit hit;
          float tCurrent = 1000.0;
          bool flag = group->intersect(ray, hit, t_min);
          //cout << "intersection " << flag << endl;
          if(flag)
          {
              ++countTrue;
              //cout << i << ", " << j << endl;
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

                  //得到当前光l的dir, col
                  l->getIllumination(p,dir,col,dis);

                  color = color + m->Shade(ray, hit, dir, col);
              }
              //adding ambient light right now, not in Shade
              color = color + sp.getAmbientLight();
              image.SetPixel(i,j,color);
          }
          else
          {
              ++countFalse;
              //cout << i << ", " << j << endl;
              image.SetPixel(i,j,sp.getBackgroundColor());
          }
      }
  }
  cout << "INTERSECTION " << countTrue << endl;
  cout << "NON-intersection" << countFalse << endl;
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

