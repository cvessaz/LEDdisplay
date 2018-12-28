//
//  global.h
//  displayWrapper
//
//  Created by Christian Vessaz on 17.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef global_h
#define global_h

#ifdef __APPLE__
#include "bdfPrinter.h"
#else
#include "../../rpi-rgb-led-matrix/include/graphics.h"
using namespace RGBMatrix
#endif

struct Params {
  std::string text = "Hello world!";
  Color color=Color(255, 0, 0);
  double speed = 0;
  bool isCleared = false;
  bool isStopped = false;
};
extern Params params;

extern std::mutex mu;

#endif /* global_h */
