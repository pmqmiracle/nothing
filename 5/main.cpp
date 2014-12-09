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

const float Kernel[] = {0.1201, 0.2339, 0.2931, 0.2339,0.1201};

bool jOutOfBoundary(int j, int width)
{
    return !(j>=0 && j < 3*width);
}

bool iOutOfBoundary(int i, int height)
{
    return !(i>=0 && i < 3*height);
}

#include "bitmap_image.hpp"

#define REFRACTION_INDEX 1

//[-0.5,0.5]
float getRandom()
{
    return -0.5+1.0*rand()/RAND_MAX;
}

int main( int argc, char* argv[] )
{
  cout << getRandom() << endl;
  cout << getRandom() << endl;
  cout << getRandom() << endl;
  char* inputFile;
  char* outputFile;
  int width;
  int height;
  int bounce_num = 0;
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
     if(strcmp(argv[argNum],"-bounces")==0)
     {
         bounce_num = atoi(argv[++argNum]);
         cout << "bounce number is : " << bounce_num << endl;
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
  Image BigImage(3*width,3*height);
  Image image(width,height);

  RayTracer *raytracer = new RayTracer(&sp, bounce_num);
  //输出中间结果写入文件
  //char fname[] = "ray.txt";
  //ofstream fout(fname);

  /////////////////////////////////////////
  //Jittering
  /////////////////////////////////////////
  for(int i = 0;i < 3*width;++i)
  {
      for(int j = 0;j < 3*height;++j)
      {
          //[-0.5,0.5]
          float myrandi = getRandom();
          float myrandj = getRandom();
          Ray ray = camera->generateRay(Vector2f(2.0*(i+myrandi)/(3*width) - 1 , 2.0*(j+myrandj)/(3*height) - 1 ));
          Hit hit;

          Vector3f finalcolor = raytracer->traceRay(ray, camera->getTMin(), 0, REFRACTION_INDEX, hit);

          BigImage.SetPixel(i,j,finalcolor);
          //image.SetPixel(i,j,finalcolor);
      }
  }

  ///////////////////////////////////////
  //Bluring
  ///////////////////////////////////////
  //Gaussian blur
  Image GBImage(3*width,3*height);
  //Blur horizontally
  for(int i = 0;i < 3*width;++i)
  {
      for(int j = 0;j < 3*height;++j)
      {
          int J[] = {i-2,i-1,i,i+1,i+2};
          for(int k = 0;k < 5;++k)
          {
              if(J[k] < 0)
                  J[k] = 0;
              if(J[k] >= 3*width)
                  J[k] = 3*width-1;
          }
          Vector3f ccc;
          for(int k = 0;k < 5;++k)
              ccc += Kernel[k] * BigImage.GetPixel(J[k],j);
          //GBImage.SetPixel(i,j,GBImage.GetPixel(i,j)+ccc);
          BigImage.SetPixel(i,j,ccc);
      }
  }
  //Blur vertically
  for(int i = 0;i < 3*width;++i)
  {
      for(int j = 0;j < 3*height;++j)
      {
          int J[] = {j-2,j-1,j,j+1,j+2};
          for(int k = 0;k < 5;++k)
          {
              if(J[k] < 0)
                  J[k] = 0;
              if(J[k] >= 3*height)
                  J[k] = 3*height-1;
          }
          Vector3f ccc;
          for(int k = 0;k < 5;++k)
              ccc += Kernel[k] * BigImage.GetPixel(i,J[k]);
          //GBImage.SetPixel(i,j,ccc);
          BigImage.SetPixel(i,j,ccc);
      }
  }

  /////////////////////////////////
  //Down Sampling Averaging
  /////////////////////////////////
  for(int i = 0;i < 3*width;++i)
  {
      for(int j = 0;j < 3*height;++j)
      {
          //image.SetPixel(i/3,j/3,GBImage.GetPixel(i,j)+image.GetPixel(i/3,j/3));
          image.SetPixel(i/3,j/3,BigImage.GetPixel(i,j)+image.GetPixel(i/3,j/3));
      }
  }

  //average 3x3
  for(int i = 0;i < width;++i)
  {
      for(int j = 0;j < height;++j)
      {
          image.SetPixel(i,j,image.GetPixel(i,j)/9.0);
      }
  }

  //TODO: 变量??how to
  //cout << "INTERSECTION " << countIntersect << endl;
  //cout << "NON-intersection" << NotIntersect << endl;
  image.SaveImage(final_output);

  return 0;
}

