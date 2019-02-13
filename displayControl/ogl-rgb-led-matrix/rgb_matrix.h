//
//  rgb_matrix.h
//  rgb_matrix
//
//  Created by Christian Vessaz on 10.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef rgb_matrix_h
#define rgb_matrix_h

#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "image.h"

#define PixelSize 8

class GPIO{
public:
  GPIO(){};
  ~GPIO(){};
  bool Init(){ return true; };
};

class RGBMatrix{
public:
  RGBMatrix(GPIO* _io, const int &_ScreenWidth, const int &_ScreenHeight, const int &_orientation);
  ~RGBMatrix();
  
  int ScreenWidth;
  int ScreenHeight;
  
  void Clear(const int &x0=0, const int &y0=0, int nx=0, int ny=0);
  void Fill(const uint8_t &red, const uint8_t &green, const uint8_t &blue);
  void SetPixel(const int &x, const int &y, const uint8_t &red, const uint8_t &green, const uint8_t &blue);
  
private:
  GLFWwindow* window;
  GLuint programID;
  GLuint vertexbuffer;
  GLuint colorbuffer;
  std::vector<GLfloat> g_vertex_buffer_data;
  std::vector<GLfloat> g_color_buffer_data;
  
  int Initialize();
  int UpdateScreen();
  
public:
  void Refresh();
  void Save(int frame = 0);
  void Movie();
  void SetPWMBits(int i) {};
};

#endif /* rgb_matrix_h */

