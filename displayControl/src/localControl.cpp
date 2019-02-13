//
//  localControl.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 08.08.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#include <thread>
#include "global.h"
#include "data.h"
#include "remoteControl.h"
#include "localControl.h"

// Global variables
Params params;
std::mutex mu;

void displayControl(RGBMatrix &canvas) {
  
  // Initialize data and communication
  int fontWidth = 10;
  int fontHeight = 20;
  Data text(&canvas, fontWidth, fontHeight);
  std::thread rc(communicate);
  
  // Main loop
  double elapseTime = std::clock();
  while (true) {
    // Cleared
    if (params.isCleared) {
      elapseTime = std::clock();
      mu.lock();
      text.initialize();
      params.isCleared = false;
      mu.unlock();
    }
    
    // Update
    if (params.speed==0) elapseTime = std::clock();
    double pixelDiff = (std::clock()-elapseTime)/CLOCKS_PER_SEC*params.speed;
    if (pixelDiff>=1.0) {
      elapseTime = std::clock();
      mu.lock();
      text.update((int)pixelDiff);
      mu.unlock();
    }
    
    // Exit
    if (params.isStopped) break;
  }
  
  // End communication
  rc.join();
}
