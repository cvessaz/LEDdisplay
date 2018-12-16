//
//  data.h
//  displayWrapper
//
//  Created by Christian Vessaz on 05.08.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef data_h
#define data_h

#include <vector>
#ifdef __APPLE__
#include "rgb_matrix.h"
#endif

class Data {
public:
  std::vector<std::pair<int,int>> pixels;
  RGBMatrix *canvas;
  Color *color;
  Font font;
  int fontWidth;
  int fontHeight;
  std::string *text;
  int xMax;
  
  Data(RGBMatrix* _canvas, const int &_fontWidth, const int &_fontHeight);
  
  ~Data();
  
  void initialize();
  
  void update(const int &dx);
};

#endif /* data_h */
