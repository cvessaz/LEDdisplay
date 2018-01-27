// Copyright (C) 2014 Henner Zeller <h.zeller@acm.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://gnu.org/licenses/gpl-2.0.txt>
//
//  bdfPrinter.h
//  displayWrapper
//
//  Created by Christian Vessaz on 12.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef bdfPrinter_h
#define bdfPrinter_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <stdint.h>
#include "rgb_matrix.h"

struct Color {
  Color(uint8_t rr, uint8_t gg, uint8_t bb) : r(rr), g(gg), b(bb) {}
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// Font loading bdf files. If this ever becomes more types, just make virtual
// base class.
class Font {
public:
  // Initialize font, but it is only usable after LoadFont() has been called.
  Font();
  ~Font();
  
  bool LoadFont(const char *font);
  
  // Return height of font in pixels. Returns -1 if font has not been loaded.
  int height() const { return font_height_; }
  
  // Return baseline. Pixels from the topline to the baseline.
  int baseline() const { return base_line_; }
  
  // Return width of given character, or -1 if font is not loaded or character
  // does not exist.
  int CharacterWidth(uint32_t unicode_codepoint) const;
  
  // Draws the unicode character at position "x","y" with "color". The "y"
  // position is the baseline of the font.
  // If we don't have it in the font, draws the replacement character "�" if
  // available.
  // Returns how much we advance on the screen, which is the width of the
  // character or 0 if we didn't draw any chracter.
  int DrawGlyph(RGBMatrix *c, int x, int y, const Color &color,
                uint32_t unicode_codepoint) const;
private:
  struct Glyph;
  typedef std::map<uint32_t, Glyph*> CodepointGlyphMap;
  
  const Glyph *FindGlyph(uint32_t codepoint) const;
  
  int font_height_;
  int base_line_;
  CodepointGlyphMap glyphs_;
};

// Draw text, encoded in UTF-8, with given "font" at "x","y" with "color".
// Returns how far we advance on the screen.
int DrawText(RGBMatrix *c, const Font &font, int x, int y, const Color &color,
             const char *utf8_text);

// Utility function that reads UTF-8 encoded codepoints from byte iterator.
// No error checking, we assume string is UTF-8 clean.
template <typename byte_iterator>
uint32_t utf8_next_codepoint(byte_iterator &it) {
  uint32_t cp = *it++;
  if (cp < 0x80) {
    return cp;   // iterator already incremented.
  }
  else if ((cp & 0xE0) == 0xC0) {
    cp = ((cp & 0x1F) << 6) + (*it & 0x3F);
  }
  else if ((cp & 0xF0) == 0xE0) {
    cp = ((cp & 0x0F) << 12) + ((*it & 0x3F) << 6);
    cp += (*++it & 0x3F);
  }
  else if ((cp & 0xF8) == 0xF0) {
    cp = ((cp & 0x07) << 18) + ((*it & 0x3F) << 12);
    cp += (*++it & 0x3F) << 6;
    cp += (*++it & 0x3F);
  }
  else if ((cp & 0xFC) == 0xF8) {
    cp = ((cp & 0x03) << 24) + ((*it & 0x3F) << 18);
    cp += (*++it & 0x3F) << 12;
    cp += (*++it & 0x3F) << 6;
    cp += (*++it & 0x3F);
  }
  else if ((cp & 0xFE) == 0xFC) {
    cp = ((cp & 0x01) << 30) + ((*it & 0x3F) << 24);
    cp += (*++it & 0x3F) << 18;
    cp += (*++it & 0x3F) << 12;
    cp += (*++it & 0x3F) << 6;
    cp += (*++it & 0x3F);
  }
  ++it;
  return cp;
}

#endif /* bdfPrinter_h */
