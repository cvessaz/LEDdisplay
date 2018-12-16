#include <iostream>
#include "led-matrix.h"
#include "graphics.h"
#include "../displayWrapper/displayWrapper/localControl.h"


int main(int argc, char *argv[]) {
  
  // Initialize display
  GPIO io;
  if (!io.Init()) {	  
	  std::cout << "GPIO ERROR" << std::endl;
	  return 1;
  }
  RGBMatrix *canvas = new RGBMatrix(&io, 32, 8); // rows 32 / chain 8
  canvas->SetPWMBits(1); // if all_extreme_colors in text-example

  // Start local display control
  displayControl(canvas);

  // Cleanup
  canvas->Clear();
  delete canvas;

  return 0;
}
