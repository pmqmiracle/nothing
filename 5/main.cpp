#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include <string.h>

#include "RayTracer.h"

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"

#define REFRACTION_INDEX 1

int main( int argc, char* argv[] )
{
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
  camera->setRatio(width*1.0/height);
  cout << "ratio is : " << width*1.0/height << endl;

  Group* group = sp.getGroup();

  //float t_min = 0.0f;

  Material* m;
  Image image(width,height);

  int bounce_num = 0;
  RayTracer *raytracer = new RayTracer(&sp, bounce_num);
  //输出中间结果写入文件
  //char fname[] = "ray.txt";
  //ofstream fout(fname);
  for(int i = 0;i < width;++i)
  {
      for(int j = 0;j < height;++j)
      {
          //[-1,1]
          Ray ray = camera->generateRay(Vector2f(2.0*i/width - 1,2.0*j/height - 1));
          Hit hit;

          Vector3f finalcolor = raytracer->traceRay(ray, camera->getTMin(), 0, REFRACTION_INDEX, hit);

          image.SetPixel(i,j,finalcolor);
      }
  }
  //TODO: 变量??how to
  //cout << "INTERSECTION " << countIntersect << endl;
  //cout << "NON-intersection" << NotIntersect << endl;
  image.SaveImage(final_output);

  return 0;
}

