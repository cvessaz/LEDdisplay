// NOTE: taken from Pieter Peers website, thank you Pieter
// raw PPM support (read & write)
// RGBE (uncompressed PIC) support (read & write)
// PIC support (adapted from original code) (read & write)
//
//  image.cpp
//  rgb_matrix
//
//  Created by Christian Vessaz on 12.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <assert.h>

#include "image.h"


unsigned int high(const Image::IND &val) { return((val >> 8) & 0xFF); }
unsigned int low(const Image::IND &val) { return(val & 0xFF); }

//==============================================================================
//! Constructor
//==============================================================================
Image::Image() {
  map = NULL;
  imageBuffer = NULL;
  numChannels = 0;
  width = 0;
  height = 0;
  format = FORMAT::AUTO;
}
//==============================================================================
//! Constructor
//==============================================================================
Image::Image(const IND &_width, const IND &_height, const IND &_numChannels)
{
  auto mapsize = _width * _height * _numChannels;
  map = new float[mapsize];
  imageBuffer = NULL; //SAFE_FREE(imageBuffer);
  numChannels = _numChannels;
  width = _width;
  height = _height;
  format = FORMAT::AUTO;
  
  memset(map, 0., mapsize);
}
//==============================================================================
//! Constructor
//==============================================================================
Image::Image(char *filename)
{
  map = NULL;
  imageBuffer = NULL;
  format = FORMAT::AUTO;
  load(filename);
}

//==============================================================================
//! Destructor
//==============================================================================
Image::~Image() { clear(); }


//==============================================================================
// Error Function
//==============================================================================
void Image::Error(char *where, char *msg)
{
  fprintf(stderr, "Error(%s) : %s.\n", where, msg);
  exit(0);
}


//==============================================================================
// clear
//==============================================================================
void Image::clear()
{
  delete[] map;
  delete[] imageBuffer;
  map = NULL;
  numChannels = 0;
  width = 0;
  height = 0;
}


//==============================================================================
// return 1 channel image which is a specific channel from this image
// TODO: define an iterator instead of providing a channel
//==============================================================================
Image Image::get_channel(const IND &channel)
{
  if ((map || numChannels > 1) && (channel <= numChannels))
  {
    Image result(width, height, 1);
    auto numpixels = width * height;
    auto pos = channel - 1;
    for (auto i = 0; i < numpixels; i++)
    {
      result.map[i] = map[pos];
      pos += numChannels;
    }
    
    return result;
  }
  else return *this;
}


//==============================================================================
// operators
//==============================================================================
Image& Image::operator=(const Image& src)
{
  if (this != &src)
  {
    clear();
    
    if(src.map == NULL) return *this;
    
    // copy complete map
    auto mapsize = src.width * src.height * src.numChannels;
    map = new float[mapsize];
    
#if 0
    // memcopy is limited in size (FIXME: max of size_t should be enough)
    for(auto i=0; i < mapsize; i++)
      map[i] = src.map[i];
#else
    // FIXME: check mapsize to ensure it is not larger then max memcpy limit?
    memcpy(map, src.map, sizeof(IND) * mapsize);
#endif
    
    // copy data
    numChannels = src.numChannels;
    width = src.width;
    height = src.height;
  }
  
  return *this;
}

Image& Image::operator=(const float* pixel)
{
  if(map && pixel)
  {
#if 0
    // fill complete map
    auto numpixels = width * height;
    IND pos = 0;
    for(auto i=0; i < numpixels; i++)
      for(auto c=0; c < numChannels; c++)
      {
        map[pos] = pixel[c];
        pos++;
      }
#else
    // FIXME: check size in case we reach limit of memcpy?
    auto mapsize = width * height * numChannels;
    memcpy(map, pixel, sizeof(IND) * mapsize);
#endif
  }
  
  return *this;
}

Image& Image::operator=(const float &value)
{
  if(map)
  {
    // fill complete map
    auto mapsize = width * height * numChannels;
#if 0
    for(unsigned long i=0; i < amount; i++)
      map[i] = value;
#else
    // FIXME: check size in case we reach limit of memset?
    memset(map, value, sizeof(IND) * mapsize);
#endif
  }
  
  return *this;
}

bool Image::operator==(const Image& src)
{
  if(width != src.width) return false;
  if(height != src.height) return false;
  if(numChannels != src.numChannels) return false;
  if(!map || !src.map) return false;
  
  unsigned long mapsize = width * height * numChannels;
  
  while(mapsize)
  {
    if(map[mapsize] != src.map[mapsize]) return false;
    mapsize--;
  }
  
  return true;
}

bool Image::operator!=(const Image& src)
{
  if(width != src.width) return true;
  if(height != src.height) return true;
  if(numChannels != src.numChannels) return true;
  if(!map || !src.map) return true;
  
  auto mapsize = width * height * numChannels;
  
  while(mapsize)
  {
    if(map[mapsize] == src.map[mapsize]) return false;
    mapsize--;
  }
  
  return true;
}

Image& Image::operator_assign(const Image& src, void (*op)(const float &val1, float &val2))
{
  // map != NULL
  // src.map != NULL
  // size and channels are equal!
  if ((map) && (src.map) && (src.width == width) && (src.height == height) && (src.numChannels == numChannels))
  {
    auto mapsize = src.width * src.height * src.numChannels;
    
    for (auto i = 0; i < mapsize; i++)
      op(src.map[i], map[i]);
  }
  
  return *this;
}

Image& Image::operator_assign(const float *pixel, void(*op)(const float &val1, float &val2))
{
  // map != NULL
  // pixel != NULL
  if (map && pixel)
  {
    auto numpixels = width * height;
    auto pos = 0;
    
    for (auto i = 0; i < numpixels; i++)
      for (auto c = 0; c < numChannels; c++)
      {
        op(pixel[c], map[pos]);
        pos++;
      }
  }
  
  return *this;
}

Image& Image::operator_assign(const float &value, void(*op)(const float &val1, float &val2))
{
  // map != NULL
  if (map)
  {
    auto mapsize = width * height * numChannels;
    
    for (auto i = 0; i < mapsize; i++)
      op(value, map[i]);
  }
  
  return *this;
}

// FIXME: can we use a macro with operators as argument, e.g. +=?
Image& Image::operator+=(const Image& src)
{
  return operator_assign(src, [](const float &val1, float &val2) {val2 += val1; });
}
Image& Image::operator-=(const Image& src)
{
  return operator_assign(src, [](const float &val1, float &val2) {val2 -= val1; });
}
Image& Image::operator*=(const Image& src)
{
  return operator_assign(src, [](const float &val1, float &val2) {val2 *= val1; });
}
Image& Image::operator/=(const Image& src)
{
  return operator_assign(src, [](const float &val1, float &val2) {val2 /= val1; });
}

Image& Image::operator+=(const float *pixel)
{
  return operator_assign(pixel, [](const float &val1, float &val2) {val2 += val1; });
}
Image& Image::operator-=(const float *pixel)
{
  return operator_assign(pixel, [](const float &val1, float &val2) {val2 -= val1; });
}
Image& Image::operator*=(const float *pixel)
{
  return operator_assign(pixel, [](const float &val1, float &val2) {val2 *= val1; });
}
Image& Image::operator/=(const float *pixel)
{
  return operator_assign(pixel, [](const float &val1, float &val2) {val2 /= val1; });
}

Image& Image::operator+=(const float &value)
{
  return operator_assign(value, [](const float &val1, float &val2) {val2 += val1; });
}
Image& Image::operator-=(const float &value)
{
  return operator_assign(value, [](const float &val1, float &val2) {val2 -= val1; });
}
Image& Image::operator*=(const float &value)
{
  return operator_assign(value, [](const float &val1, float &val2) {val2 *= val1; });
}
Image& Image::operator/=(const float &value)
{
  return operator_assign(value, [](const float &val1, float &val2) {val2 /= val1; });
}



Image Image::operator_arithmetic(const Image& src, float(*op)(const float &val1, const float &val2))
{
  // map != NULL
  // src.map != NULL
  // size and channels are equal!
  if ((src.width == width) && (src.height == height) && (src.numChannels == numChannels))
  {
    Image result(width, height, numChannels);
    
    if (map && src.map)
    {
      auto mapsize = src.width * src.height * src.numChannels;
      
      for (auto i = 0; i < mapsize; i++)
        result = op(map[i], src.map[i]);
    }
    else if (map) result = *this;
    else result = src;
    
    return result;
  }
  else return Image();
}

Image Image::operator_arithmetic(const float *pixel, float(*op)(const float &val1, const float &val2))
{
  Image result(width, height, numChannels);
  
  // map != NULL
  // pixel != NULL
  if (map && pixel)
  {
    auto numpixels = width * height;
    unsigned long pos = 0;
    
    for(auto i=0; i < numpixels; i++)
      for(auto c=0; c < numChannels; c++)
      {
        result.map[pos] = op(map[pos], pixel[c]);
        pos++;
      }
  }
  else if(!map) result = pixel;
  else result = *this;
  
  return result;
}

Image Image::operator_arithmetic(const float &value, float(*op)(const float &val1, const float &val2))
{
  Image result(width, height, numChannels);
  
  // map != NULL
  if (map)
  {
    auto mapsize = width * height * numChannels;
    
    for(auto i=0; i < mapsize; i++)
      result.map[i] = op(map[i], value);
  }
  else result = value;
  
  return result;
}

// FIXME: can we use a macro with operators as argument, e.g. +?
Image Image::operator+(const Image& src)
{
  return operator_arithmetic(src, [](const float &val1, const float &val2) {return val2 + val1; });
}
Image Image::operator-(const Image& src)
{
  return operator_arithmetic(src, [](const float &val1, const float &val2) {return val2 - val1; });
}
Image Image::operator*(const Image& src)
{
  return operator_arithmetic(src, [](const float &val1, const float &val2) {return val2 * val1; });
}
Image Image::operator/(const Image& src)
{
  return operator_arithmetic(src, [](const float &val1, const float &val2) {return val2 / val1; });
}

Image Image::operator+(const float *pixel)
{
  return operator_arithmetic(pixel, [](const float &val1, const float &val2) {return val2 + val1; });
}
Image Image::operator-(const float *pixel)
{
  return operator_arithmetic(pixel, [](const float &val1, const float &val2) {return val2 - val1; });
}
Image Image::operator*(const float *pixel)
{
  return operator_arithmetic(pixel, [](const float &val1, const float &val2) {return val2 * val1; });
}
Image Image::operator/(const float *pixel)
{
  return operator_arithmetic(pixel, [](const float &val1, const float &val2) {return val2 / val1; });
}

Image Image::operator+(const float &value)
{
  return operator_arithmetic(value, [](const float &val1, const float &val2) {return val2 + val1; });
}
Image Image::operator-(const float &value)
{
  return operator_arithmetic(value, [](const float &val1, const float &val2) {return val2 - val1; });
}
Image Image::operator*(const float &value)
{
  return operator_arithmetic(value, [](const float &val1, const float &val2) {return val2 * val1; });
}
Image Image::operator/(const float &value)
{
  return operator_arithmetic(value, [](const float &val1, const float &val2) {return val2 / val1; });
}


// take abs of each pixel of this image
void Image::abs(void)
{
  if(map)
  {
    auto mapsize = width * height * numChannels;
    for(auto i=0; i < mapsize; i++)
      map[i] = (float)fabs(map[i]);
  }
}

// return image which has each pixel abs of this image
Image Image::iabs(void)
{
  if(map)
  {
    Image result(width, height, numChannels);
    auto mapsize = width * height * numChannels;
    for(auto i=0; i < mapsize; i++)
      result.map[i] = (float)fabs(map[i]);
    return result;
  }
  else return *this;
}

// truncate each pixel value of this inage between bottom and top value
void Image::trunc(const float &bottomvalue, const float &topvalue)
{
  if(map)
  {
    auto mapsize = width * height * numChannels;
    for(auto i=0; i < mapsize; i++)
      if(map[i] < bottomvalue) map[i] = bottomvalue;
      else if(map[i] > topvalue) map[i] = topvalue;
    //else map[i] = map[i];
  }
}

// return truncated Image in which  each pixel value of this inage between bottom and top value
Image Image::itrunc(const float &bottomvalue, const float &topvalue)
{
  if(map)
  {
    Image result(width, height, numChannels);
    auto mapsize = width * height * numChannels;
    for(auto i=0; i < mapsize; i++)
      if(map[i] < bottomvalue) result.map[i] = bottomvalue;
      else if(map[i] > topvalue) result.map[i] = topvalue;
      else result.map[i] = map[i];
    
    return result;
  }
  else return *this;
}

// convert image from summed area table to normal
void Image::UnSummedAreaTable(void)
{
  if(!map) return;
  IND pos = 0;
  
  fprintf(stderr, "Converting Image from Summed Area Table...");
  
  auto mapsize = width * height * numChannels;
  auto *map2 = new float[mapsize];
  
  for(auto y=0; y < height; y++)
    for(auto x=0; x < width; x++)
      for(auto c=0; c < numChannels; c++)
      {
        if((x != 0) && (y != 0))
          map2[pos] = map[pos] + map[pos - (numChannels*(width+1))] - map[pos - numChannels] - map[pos - (width*numChannels)];
        
        else if((x != 0) && (y == 0))
          map2[pos] = map[pos] - map[pos - numChannels];
        
        else if((x == 0) && (y != 0))
          map2[pos] = map[pos] - map[pos - (numChannels*width)];
        
        else map2[pos] = map[pos];
        
        pos++;
      }
  
  delete[] map;
  map = map2;
  fprintf(stderr, "Done.\n");
}

// convert image to summed area table
void Image::SummedAreaTable(void)
{
  if(!map) return;
  IND pos = 0;
  
  fprintf(stderr, "Converting Image to Summed Area Table...");
  
  for(auto y=0; y < height; y++)
  {
    for(auto x=0; x < width; x++)
    {
      for(auto c=0; c < numChannels; c++)
      {
        // if not first column and not first line
        if((x != 0) && (y != 0))
          // sum = pixel + sum_block_above + sum_block_before - sum_block_above_before
          map[pos] = map[pos] + map[pos - (numChannels*width)] + map[pos - numChannels] - map[pos - (numChannels*(width+1))];
        
        // if first line (but not first 0,0)
        else if((y == 0) && (x != 0))
          // sum = pixel + sum_block_before
          map[pos] = map[pos] + map[pos - numChannels];
        
        // first column (but not first 0,0)
        else if((x == 0) && (y != 0))
          // sum = pixel + sum_column_above
          map[pos] = map[pos] + map[pos - (numChannels*width)];
        
        // next pos
        pos++;
      }
    }
  }
  
  fprintf(stderr, "Done.\n");
}

// read past comments in ppm and pic files
// a comment is a line beginning with a # and ending with a newline (\n)
void Image::skipComment(FILE *file)
{
  char b = fgetc(file);
  
  while(b == '#')
  {
    while (b != '\n')
      b = fgetc(file);
    
    b = fgetc(file);
  }
  ungetc(b, file);
}

// read a RAW (P6) ppm file.
bool Image::loadPPM(FILE *file)
{
  unsigned int  colorsize;
  
  // read header (P6)
  fscanf(file, "P6\n");
  
  // skip comment (#)
  skipComment(file);
  
  // set nr of channels
  numChannels = 3;
  
  // read dimensions
  char dummy;
  IND r = fscanf(file, "%d %d\n%d%c", &width, &height, &colorsize, &dummy);
  if (r != 4) Error(NULL, (char*)"not a PPM (raw) file");
  
  // calc amount of memory needed and allocate it
  auto mapsize = width * height * numChannels;
  map = new float[mapsize];
  
  // load image
  // TODO: load all data at once
  for (unsigned int j=0; j<height; j++)
  {
    float *row = &(map[numChannels * width * j]);
    for(unsigned int i = 0; i < numChannels * width; i++)
      row[i] = (float)(fgetc(file)) / colorsize;
  }
  
  
  return true;
}

// save to a RAW (P6) ppm file
void Image::savePPM(FILE *file)
{
  
  const unsigned int colorsize = 255;
  
  // sanity check
  if(!map) Error(NULL, (char*)"can't write empty image");
  
  // write header
  fprintf(file, "P6\n%d %d\n%d\n", width, height, colorsize);
  
  auto numpixels = width * height;
  IND numChannels_ppm = 3;
  
  auto buffer = new char[sizeof(unsigned int) * numpixels * numChannels_ppm];
  
  // save image data
  // if less then 3 channels, add with 0-value chans
  // if more then 3 channe;s, drop excess channels
  for (auto j = 0; j < height; j++)
  {
    float *row = map + (numChannels * width * j);
    for (auto i = 0; i < width; i++)
    {
      for (auto c = 0; c < numChannels_ppm; c++)
      {
        if (c < numChannels)
        {
          unsigned int color = (unsigned int)(row[i * numChannels + c] * colorsize);
          if (color > colorsize) color = colorsize;
          
          fputc(color, file);
        }
        else {
          fputc(0, file);
        }
      }
    }
  }
    
  auto buffersize = numpixels * numChannels_ppm;
  
  fwrite(buffer, sizeof(IND), buffersize, file);
}

#define FORMAT_NAME "FORMAT"
#define FORMAT_TYPE "32-bit_rle_rgbe"
#define EXPOSURE "EXPOSURE"
#define MINLEN   (unsigned long)(8)
#define MAXLEN   (unsigned long)(32767)
#define MINRUN   (unsigned long)(4)

// given a real pixels value and its exponent (4th value) return its RGB component
//
// ABCE => R = RealPixel2RGB(A, E)
//         G = RealPixel2RGB(B, E)
//         B = RealPixel2RGB(C, E)
float Image::RealPixel2RGB(const IND &c, const IND &e)
{
  if(e == 0) return 0;
  else {
    float v = (float)(ldexp(1./256, e - 128));
    return (float)((c + .5)*v);
  }
}

// given an RGB triplet create a real pixel color and store in Color
//
// RGB2RealPixel(R, G, B, ABCE)
//
void Image::RGB2RealPixel(const float &r, const float &g, const float &b, unsigned char Color[4])
{
  auto max = r;
  if(g > max) max = g;
  if(b > max) max = b;
  
  if(max <= 1e-32) { Color[0] = Color[1] = Color[2] = Color[3] = 0; }
  else
  {
    int exp;
    max = (float)(frexp(max, &exp) * 255.9999 / max);
    
    Color[0] = (unsigned char)(r * max);
    Color[1] = (unsigned char)(g * max);
    Color[2] = (unsigned char)(b * max);
    Color[3] = (unsigned char)(exp + 128);
  }
}

// read a scanline from a PIC file into row with width.
bool Image::ReadPICscanline(FILE *file, float *row, const IND &_width)
{
  IND pos = 0;
  IND r, g, b, e, i;
  
  IND numChannels_pic = 3;
  while(pos < (numChannels_pic * width))
  {
    // read 4 bytes
    r = fgetc(file);
    g = fgetc(file);
    b = fgetc(file);
    e = fgetc(file);
    
    // check compresion method
    if((r == 1) && (g == 1) && (b == 1))           // Old Run-Length encoding
    {
      auto prev = pos - 3;
      auto l = e;
      decltype(l) t = 8;
      bool done = false;
      
      // stupidity check
      if(pos < numChannels_pic) Error(NULL, (char*)"PIC: Illegal Old RLE compression");
      
      // check l
      while(!done)
      {
        if(t > 32) Error(NULL, (char*)"PIC: Old RLE overflow");
        
        r = fgetc(file);
        g = fgetc(file);
        b = fgetc(file);
        e = fgetc(file);
        
        if(e == EOF) done = true;
        else if((r == 1) && (g == 1) && (b ==1))
        {
          l += (e << t);
          t += 8;
        }
        else done = true;
      }
      
      if(e != EOF)
      {
        ungetc(e, file);
        ungetc(b, file);
        ungetc(g, file);
        ungetc(r, file);
      }
      
      // repeat
      for(i=0; i < l; i++)
      {
        row[pos++] = row[prev + 0];
        row[pos++] = row[prev + 1];
        row[pos++] = row[prev + 2];
      }
    }
    else if((r == 2) && (g == 2))           // Run-Length encoding
    {
      if((b << 8 | e) != width) Error(NULL, (char*)"PIC: RLE length mismatch");
      
      auto *R = new IND[width];
      auto *G = new IND[width];
      auto *B = new IND[width];
      auto *E = new IND[width];
      IND *T = NULL;
      
      // repeat per channel
      for(i=0; i < 4; i++)
      {
        IND p = 0;
        if(i==0) T = R;
        if(i==1) T = G;
        if(i==2) T = B;
        if(i==3) T = E;
        
        while(p < width)
        {
          e = fgetc(file);
          if(p >= width) Error(NULL, (char*)"PIC: RLE out of bounds");
          if(e == EOF) Error(NULL, (char*)"PIC: RLE unexpected end");
          if(e > 128)
          {
            e &= 127;
            r = fgetc(file);
            
            for(auto j=0; j < e; j++) T[p++] = r;
          }
          else for(auto j=0; j < e; j++) T[p++] = fgetc(file);
        }
      }
      
      // copy & convert
      for(i=0; i < width; i++)
      {
        r = R[i];
        g = G[i];
        b = B[i];
        e = E[i];
        
        row[pos++] = RealPixel2RGB(r, e);
        row[pos++] = RealPixel2RGB(g, e);
        row[pos++] = RealPixel2RGB(b, e);
      }
      
      // free mem
      delete[] R;
      delete[] G;
      delete[] B;
      delete[] E;
    }
    else                           // Uncompressed
    {
      row[pos++] = RealPixel2RGB(r, e);
      row[pos++] = RealPixel2RGB(g, e);
      row[pos++] = RealPixel2RGB(b, e);
    }
  }
  
  
  return true;
}

// write a scanline from row with width to file
void Image::WritePICscanline(FILE *file, unsigned char *row, const IND &_width)
{
  // adapted from the incomprehensible radiance code
  
  // out of range => plain write
  if(_width < MINLEN || _width > MAXLEN)
  {
    fwrite(row, 4, _width, file);
    return;
  }
  
  // write compression header (2,2, high(width), low(width))  RLE COMPRESSION
  fputc(2, file);
  fputc(2, file);
  fputc(high(_width), file);
  fputc(low(_width), file);
  
  // threat every chan seperately
  for(IND c = 0; c < 4; c++)
  {
    // find next run, that is longer then MINLEN
    IND pos = 0;
    while(pos < _width)
    {
      IND begin = pos;
      IND runLength = 0;
      while((begin < _width) && (runLength < MINLEN))
      {
        begin += runLength;
        runLength = 1;
        while((runLength < 127) && (begin + runLength < _width) && (row[begin*4 + c] == row[(begin + runLength) * 4 + c]))
        {
          runLength++;
        }
      }
      
      // short run check (long run is proceeded by short run)
      auto start = begin - pos;
      if((start > 1) && (start < MINRUN))
      {
        auto shortpos = pos + 1;
        
        // check if present
        while((row[shortpos*4 + c] == row[pos*4 + c]) && (shortpos < begin)) shortpos++;
        
        // write out if present
        if(shortpos == begin)
        {
          fputc(128+start, file);
          fputc(row[pos*4 + c], file);
          pos = begin;
        }
      }
      
      // write out non-run
      while(pos < begin)
      {
        auto length = begin - pos;
        if(length > 128) length = 128;    // max non-run
        fputc(length, file);
        while(length--)
        {
          fputc(row[pos*4 + c], file);
          pos++;
        }
      }
      
      // if long-run was found...write it out
      if(runLength >= MINLEN)
      {
        fputc(128+runLength, file);
        fputc(row[begin*4 + c], file);
        pos += runLength;
      }
    }
  }
}

// load a PIC file from a file
bool Image::loadPIC(FILE *file)
{
  // Based on the original code for PIC readers.
  float exposure = 1.;
  bool done = false;
  char buf[80];
  
  // read header
  fscanf(file, "#?RADIANCE\n");
  
  while(!done)
  {
    skipComment(file);
    
    // read line
    fgets(buf, 80, file);
    
    // check format (only support rgbe format...NOT xyze)
    if(!strncmp(buf, FORMAT_NAME, strlen(FORMAT_NAME)))
    {
      char *string = strchr(buf, '=') + 1;
      if(strncmp(string, FORMAT_TYPE, strlen(FORMAT_TYPE))) Error(NULL, (char*)"Unsupported PIC format");
    }
    
    // read exposure
    else if(!strncmp(buf, EXPOSURE, strlen(EXPOSURE)))
    {
      char *value = strchr(buf, '=') + 1;
      exposure = (float)(1. / atof(value));
    }
    
    // end?
    else if(!strncmp(buf, "\n", strlen("\n"))) done = true;
    
    /* else skip line */
  }
  
  // get resolution (ignore orientation & sign)
  unsigned char sx, sy, X, Y;
  if( fscanf(file, "%c%c %d %c%c %d\n", &sy, &Y, &height, &sx, &X, &width) != 6)
  {
    fprintf(stderr, "PIC resolution error. (%c%c %u %c%c %u)", sy, Y, height, sx, X, width);
    Error(NULL, (char*)"PIC res error.");
  }
  
  numChannels = 3;
  
  // calc amount of memory needed and allocate it
  auto mapsize = width * height * 3;
  map = new float[mapsize];
  
  // read image (in top to bottom order)
  for(unsigned int y = 0; y < height; y++) {
    ReadPICscanline(file, map + numChannels*width*y, width);
  }
  
  // multiply read values with exposure
  float *m = map;
  for (unsigned int i=0; i<mapsize; i++, m++) {
    *m *= exposure;
  }
  
  return true;
}


// save as PIC
void Image::savePIC(FILE *file)
{
  // save data in PIC format (RLE compressed)
  
  // sanity check
  if(!map) Error(NULL, (char*)"can't write empty image");
  
  // scanline
  auto *Color  = new unsigned char[4 * width];
  
  // write header
  fprintf(file, "#?RADIANCE\n");
  fprintf(file, "%s=%s\n\n", FORMAT_NAME, FORMAT_TYPE);
  fprintf(file, "-Y %u +X %u\n", height, width);
  
  for(unsigned int y=0; y < height; y++)
  {
    // convert row into RGBE
    float *row = &(map[numChannels*width*y]);
    for(unsigned int x=0; x < width; x++)
    {
      float r = (numChannels > 0) ? row[x*numChannels + 0] : 0;
      float g = (numChannels > 1) ? row[x*numChannels + 1] : 0;
      float b = (numChannels > 2) ? row[x*numChannels + 2] : 0;
      RGB2RealPixel(r, g, b, &(Color[x*4]));
    }
    
    // compress and write out rgbe
    WritePICscanline(file, Color, width);
  }
  
  delete[] Color;
}

// load from RGBE file
bool Image::loadRGBE(FILE *file)
{
  float exposure = 1.;
  bool done = false;
  char buf[80];
  
  // read header
  fscanf(file, "#?RGBE\n");
  
  while(!done)
  {
    skipComment(file);
    
    // read line
    fgets(buf, 80, file);
    
    // check format (only support rgbe format...NOT xyze)
    if(!strncmp(buf, FORMAT_NAME, strlen(FORMAT_NAME)))
    {
      char *string = strchr(buf, '=') + 1;
      if(strncmp(string, FORMAT_TYPE, strlen(FORMAT_TYPE))) Error(NULL, (char*)"Unsupported RGBE format");
    }
    
    // read exposure
    else if(!strncmp(buf, EXPOSURE, strlen(EXPOSURE)))
    {
      char *value = strchr(buf, '=') + 1;
      exposure = (float)(1. / atof(value));
    }
    
    // end?
    else if(!strncmp(buf, "\n", strlen("\n"))) done = true;
    
    /* else skip line */
  }
  
  // get resolution (ignore orientation & sign)
  unsigned char sx, sy, X, Y;
  if( fscanf(file, "%c%c %d %c%c %d\n", &sy, &Y, &height, &sx, &X, &width) != 6)
  {
    fprintf(stderr, "RGBE resolution error. (%c%c %u %c%c %u)", sy, Y, height, sx, X, width);
    Error(NULL, (char*)"RGBE res error.");
  }
  
  
  numChannels = 3;
  
  // calc amount of memory needed and allocate it
  auto mapsize = width * height * numChannels;
  map = new float[mapsize];
  
  // read image (in top to bottom order)
  for(auto y=0; y < height; y++)
  {
    float *row = &(map[y*width*numChannels]);
    for(auto x=0; x < width; x++)
    {
      unsigned char r, g, b, e;
      fscanf(file, "%c%c%c%c", &r, &g, &b, &e);
      
      row[x*3 + 0] = RealPixel2RGB(r, e);
      row[x*3 + 1] = RealPixel2RGB(g, e);
      row[x*3 + 2] = RealPixel2RGB(b, e);
    }
  }
  
  // multiply read values with exposure
  float *m = map;
  for (unsigned int i=0; i<mapsize; i++, m++) {
    *m *= exposure;
  }
  
  
  return true;
}

// save to RGBE formated file
void Image::saveRGBE(FILE *file)
{
  // save data in RGBE format (uncompressed PIC)
  
  // sanity check
  if(!map) Error(NULL, (char*)"can't write empty image");
  
  // write header
  fprintf(file, "#?RGBE\n");
  fprintf(file, "%s=%s\n\n", FORMAT_NAME, FORMAT_TYPE);
  fprintf(file, "-Y %u +X %u\n", height, width);
  
  // write out uncompressed
  // drop chans above 3
  // fill chans under 3 if not exist
  for(auto y=0; y < height; y++)
  {
    float *row = &(map[width*numChannels*y]);
    for(auto x=0; x < width; x++)
    {
      // get first 3 channels
      float r = (numChannels > 0) ? row[x*numChannels + 0] : 0;
      float g = (numChannels > 1) ? row[x*numChannels + 1] : 0;
      float b = (numChannels > 2) ? row[x*numChannels + 2] : 0;
      
      // convert to realpixel format
      unsigned char Color[4];
      RGB2RealPixel(r, g, b, Color);
      
      // write out
      fwrite(Color, 4, 1, file);
    }
  }
  
}

// load image file
bool Image::load(const std::string &filename)
{
#define CHECK_FILE  if (!file) Error("Image::load()", "couldn't open image file");
  
  // clear data
  clear();
  
  FILE *file = NULL;
  
  
  auto format_tmp = format;
  if (format == FORMAT::AUTO) {
    // determine file type
    if (filename.find(".bmp") != std::string::npos)
      format_tmp = FORMAT::BMP_MONOCHROME;
    else if (filename.find(".pic") != std::string::npos)
      format_tmp = FORMAT::PIC;
    else if (filename.find(".ppm") != std::string::npos || filename.compare(".pnm") != std::string::npos)
      format_tmp = FORMAT::PPM;
    else if (filename.find(".rgbe") != std::string::npos)
      format_tmp = FORMAT::RGBE;
    else
    {
      printf("Unknown image format for '%s'", filename.c_str());
      return false;
    }
  }
  
  bool success = true;
  switch (format_tmp) {
    case FORMAT::PIC:
      file = fopen(filename.c_str(), "rb");
      success = loadPIC(file);
      fprintf(stderr, "PIC ");
      break;
    case FORMAT::PPM:
      file = fopen(filename.c_str(), "rb");
      success = loadPPM(file);
      fprintf(stderr, "PPM ");
      break;
    case FORMAT::RGBE:
      file = fopen(filename.c_str(), "rb");
      success = loadRGBE(file);
      fprintf(stderr, "RGBE ");
      break;
    default:
      printf("Image::load(): unknown image format for '%s'", filename.c_str());
      success = false;
  }
  
  
  // clean
  if (file) fclose(file);
  
  
  //PRINTF_INFO("read: %s: %ld KBytes, Size=%ldx%ld, Channels=%ld\n", filename, (unsigned long)(width * height * 3 * sizeof(float) >> 10), width, height, channels);
  
  return success;
  
#undef CHECK_FILE
}
bool Image::load(char *filename)
{
  return load(std::string(filename));
}

// save image
bool Image::save(const char *filename)
{
  
  // save buffer to image texture
  if (map == NULL) Error(NULL, (char*)"Image::save(): empty image buffer");
  
  FILE* file = fopen(filename, "wb");
  
  if (!file) {
    printf("REMARK: Couldn't create texture image file\n");
    return false;
  }
  
  // determine file type
  const char *name = filename;
  if(strstr(name, ".ppm") || strstr(name, ".pnm"))
  {
    savePPM(file);
    //fprintf(stderr, "PPM ");
  }
  else if(strstr(name, ".pic"))
  {
    savePIC(file);
    //fprintf(stderr, "PIC ");
  }
  else if(strstr(name, ".rgbe"))
  {
    saveRGBE(file);
    //fprintf(stderr, "RGBE ");
  }
  else
  {
    fprintf(stderr, "Image::save(): unknown image format for '%s'", name);
  }
  
  // close file
  fclose(file);
  
  // Display nice message
  //fprintf(stderr, "write: %s: %ld KBytes, Size=%dx%d, Channels=%u\n", filename, (unsigned long)(width * height * 3 * sizeof(float) >> 10), width, height, numChannels);
  
  return true;
}
