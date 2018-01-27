//
//  main.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 23.12.17.
//  Copyright © 2017 Christian Vessaz. All rights reserved.
//

#include <unistd.h>
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
int desk[4] = {0,0,0,0};
char label[4] = {'A','B','C','D'};
std::map<int,std::string> specialChar = {
  {9, "  √81"},
  {18, " 180%"}
};
std::mutex mu;

void InitializeDeskLoop(RGBMatrix &canvas, const Font &font) {
  for (int i=0; i<4; ++i) {
    char line[2];
    sprintf(line, "%c", label[i]);
    DrawText(&canvas, font, 2, (i*32)+6 + font.baseline(),  params.color, line);
  }
}

void UpdateDeskLoop(RGBMatrix &canvas, const Font &font, const std::map<int,std::string> &specialChar) {
  unsigned int a = rand();
  int d = a % 4;
  char line[100];
  auto it = specialChar.find(params.i);
  if (it!=specialChar.end()) {
    sprintf(line, "%c%s", label[d],it->second.c_str());
  } else {
    sprintf(line, "%c %4d", label[d],params.i);
  }
  canvas.Clear(12, (d*32), 52, 32);
  DrawText(&canvas, font, 2, (d*32)+6 + font.baseline(),  params.color, line);
}

int main(int argc, const char * argv[]) {
  auto canvas = RGBMatrix();
  Font font;
  font.LoadFont(fontName.c_str());
  srand((unsigned int)time(NULL));
  std::thread rc(communicate);
#ifdef __APPLE__
  int f=0;
  int frameRate=25;
#endif
  while (true) {
    mu.lock();
    if (params.i==0||params.i>params.N) {
      InitializeDeskLoop(canvas,font);
      params.i=0;
    }
    auto startTime = std::clock();
    if (!params.paused) {
      ++params.i;
      UpdateDeskLoop(canvas,font,specialChar);
    }
    auto waitTime = params.waitMin + (rand()/(RAND_MAX/(params.waitMax-params.waitMin)));
    params.t += waitTime;
#ifdef __APPLE__
    for (auto s=0;s<(int)(waitTime*frameRate); ++s) {
      canvas.Save(f);
      ++f;
    }
#endif
    mu.unlock();
    auto endTime = (std::clock() - startTime) / (float)CLOCKS_PER_SEC;
    if (endTime<waitTime) {
      sleep(waitTime-endTime);
    }
    if (params.t>=params.tMax) {
      break;
    }
  }
#ifdef __APPLE__
  canvas.Movie();
#endif
  rc.join();
  return 0;
}
