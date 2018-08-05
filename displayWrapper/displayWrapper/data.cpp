//
//  data.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 05.08.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#include "data.h"
#include "global.h"

Data::Data(RGBMatrix* _canvas, const int &_fontWidth, const int &_fontHeight) {
  canvas = _canvas;
  std::stringstream fontName;
  fontName << _fontWidth << "x" << _fontHeight << ".bdf";
  bool found = font.LoadFont(fontName.str().c_str());
  assert(found);
  color = &params.color;
  text = &params.text;
  fontWidth = _fontWidth;
  fontHeight = _fontHeight;
  xMax = std::max((int)text->size()*fontWidth,canvas->width());
  initialize();
}

Data::~Data() {
}

void Data::initialize() {
  if (pixels.size()>0) {
    for (auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, 0, 0, 0);
    }
  }
  pixels.clear();
  pixels.reserve(xMax*fontHeight);
  int x = 0;
  int y = 38;
  for (const uint32_t cp : *text) {
    x += font.DrawGlyph(pixels, x, y, cp);
  }
  update(0);
}

void Data::update(const int &dx) {
  if (dx>0) {
    for (auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, 0, 0, 0);
    }
    for (auto &pixel : pixels) {
      pixel.first = (pixel.first-dx);
      if (pixel.first<0) pixel.first += xMax;
      canvas->SetPixel(pixel.first, pixel.second, color->r, color->g, color->b);
    }
  } else {
    for (const auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, color->r, color->g, color->b);
    }
  }
#ifdef __APPLE__
  canvas->Refresh();
#endif
}
