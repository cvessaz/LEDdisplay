//
//  main.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 23.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

#include <unistd.h>
#include <vector>
#include <ctime>
#include <map>
#include <thread>
#include <mutex>

#include "global.h"
#include "remoteControl.h"
#ifdef __APPLE__
#include "rgb_matrix.h"
#include "bdfPrinter.h"
#endif

Params params;
std::string fontName="10x20.bdf";
std::mutex mu;

class Data {
public:
  std::vector<std::vector<Color>> pixels;
  
  Data() {};
  ~Data() {};
  
  void initialize() {};
  
  void update() {};
};

int main(int argc, const char * argv[]) {
  
  // Initialize display, data and communication
  auto canvas = RGBMatrix();
  Font font;
  font.LoadFont(fontName.c_str());
  Data text;
  std::thread rc(communicate);

  // Main loop
  double elapseTime = std::clock();
  while (true) {
    // Cleared
    if (params.isCleared) {
      text.initialize();
      elapseTime = std::clock();
    }
    
    // Update
    double pixelDiff = (std::clock()-elapseTime)*params.speed;
    if (pixelDiff>=1.0) {
      text.update();
      elapseTime = std::clock();
      if (pixelDiff>=2.0) assert(false);
    }
    
    // Exit
    if (params.isStopped) break;
  }

  // End communication
  rc.join();
  
  return 0;
}
