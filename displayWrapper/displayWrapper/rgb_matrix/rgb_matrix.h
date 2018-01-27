//
//  rgb_matrix.h
//  rgb_matrix
//
//  Created by Christian Vessaz on 10.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef rgb_matrix_h
#define rgb_matrix_h

#include <iostream>
#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "image.h"

#define ScreenWidth 64
#define ScreenHeight 128
#define PixelSize 8

class RGBMatrix{
public:
  RGBMatrix();
  ~RGBMatrix();
  
  int width();
  int height();
  
  void Clear(const int &x0=0, const int &y0=0, const int &nx=ScreenWidth, const int &ny=ScreenHeight);
  void Fill(const uint8_t &red, const uint8_t &green, const uint8_t &blue);
  void SetPixel(const int &x, const int &y, const uint8_t &red, const uint8_t &green, const uint8_t &blue);
  
private:
  GLFWwindow* window;
  GLuint programID;
  GLuint vertexbuffer;
  GLuint colorbuffer;
  GLfloat g_color_buffer_data[ScreenWidth*ScreenHeight*2*3*3];
  
  int Initialize();
  int UpdateScreen();
  
public:
  void Refresh();
  void Save(const int &frame = 0);
  void Movie();
};

#endif /* rgb_matrix_h */

