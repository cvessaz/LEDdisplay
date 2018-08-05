//
//  main.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 23.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

#include <unistd.h>
#include <ctime>
#include <thread>
#include <mutex>

#include "global.h"
#include "data.h"
#include "remoteControl.h"
#ifdef __APPLE__
#include "rgb_matrix.h"
#include "bdfPrinter.h"
#endif

Params params;
int fontWidth = 10;
int fontHeight = 20;
std::mutex mu;

int main(int argc, const char * argv[]) {
  
  // Initialize display, data and communication
  auto canvas = RGBMatrix();
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
  
  return 0;
}
