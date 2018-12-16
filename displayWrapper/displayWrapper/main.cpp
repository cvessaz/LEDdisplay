//
//  main.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 23.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

#include "localControl.h"
#include "rgb_matrix.h"

int main(int argc, const char * argv[]) {
  
  // Initialize display
  auto canvas = RGBMatrix();
  
  // Start local display control
  displayControl(canvas);
  
  return 0;
}
