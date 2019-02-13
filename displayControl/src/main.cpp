//
//  main.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 23.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

#include "localControl.h"
#ifdef RPI
#include "led-matrix.h"
using namespace rgb_matrix;
#else
#include "rgb_matrix.h"
#endif

int main(int argc, const char * argv[]) {
  
  // Initialize GPIO
  GPIO io;
  if (!io.Init()) {
    std::cout << "GPIO ERROR" << std::endl;
    return 1;
  }
  
  // Initialize display (width, height, orientation)
  auto canvas = RGBMatrix(&io, 128, 64, 0);
  canvas.SetPWMBits(1); // if all_extreme_colors in text-example
  
  // Start local display control
  displayControl(canvas);
  
  // Cleanup
  canvas.Clear();
  
  return 0;
}
