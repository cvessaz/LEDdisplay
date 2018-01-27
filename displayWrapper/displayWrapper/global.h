//
//  global.h
//  displayWrapper
//
//  Created by Christian Vessaz on 17.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef global_h
#define global_h

#include "bdfPrinter.h"

struct Params {
  float t=0.0;
  float tMax=60.0;
  float waitMin=1.0;
  float waitMax=5.0;
  int i=0;
  int N=100;
  bool paused=false;
  Color color=Color(255, 0, 0);
};
extern Params params;

extern std::mutex mu;

#endif /* global_h */
