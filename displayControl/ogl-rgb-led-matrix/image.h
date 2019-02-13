// NOTE: taken from Pieter Peers website, thank you Pieter
// raw PPM support (read & write)
// RGBE (uncompressed PIC) support (read & write)
// PIC support (adapted from original code) (read & write)
//
//  image.h
//  rgb_matrix
//
//  Created by Christian Vessaz on 12.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#ifndef image_h
#define image_h

// std
#include <string.h>
#include <stdio.h>
#include <math.h>

class Pixel
{
  typedef float REAL;
  
  //!< Mode
  enum class MODE {
    INTEGER,
    REAL
  };
  
  REAL x[4];   //!< rgba values
  
public:
  
  //! Constructor
  Pixel() { for (int i = 0; i < 4; i++) x[i] = 0.; }
  //! Constructor
  Pixel(const REAL &r, const REAL &g, const REAL &b) { set_rgb(r, g, b); x[3] = 1.; }
  //! Destructor
  ~Pixel() {};
  
  //! Set rgb
  void set_rgb(const REAL &r, const REAL &g, const REAL &b) {
    x[0] = r;
    x[1] = g;
    x[2] = b;
  }
  
  //! Get red value
  REAL r() { return x[0]; }
  //! Get green value
  REAL g() { return x[1]; }
  //! Get blue value
  REAL b() { return x[2]; }
  //! Get opacity value
  REAL a() { return x[3]; }
  
};

class Image {
public:
  typedef unsigned int IND;
public:
  //!< Image format
  enum class FORMAT {
    AUTO,
    BMP_MONOCHROME,
    BMP_RGB,
    PIC,
    PPM,
    RGBE,
    SIZE
  };
private:
  
  float        *map;            //!< Image buffer storage in float (FIXME: use only the image buffer?)
  char         *imageBuffer;    //!< Image buffer storage
  IND           numChannels;    //!< rgb channels
  IND           width;          //!< Image width
  IND           height;         //!< Image height
  FORMAT        format;         //!< Image format
  
private:
  void skipComment(FILE *file);
  bool loadPPM(FILE *file);
  void savePPM(FILE *file);
  
  float RealPixel2RGB(const IND &c, const IND &e);
  void RGB2RealPixel(const float &r, const float &g, const float &b, unsigned char Color[4]);
  bool ReadPICscanline(FILE *file, float *row, const IND &_width);
  void WritePICscanline(FILE *file, unsigned char *row, const IND &_width);
  
  bool loadPIC(FILE *file);
  void savePIC(FILE *file);
  
  bool loadRGBE(FILE *file);
  void saveRGBE(FILE *file);
  
  
public:
  //! Constructor
  Image();
  //! Constructor
  Image(const unsigned int &_width, const IND &_height, const IND &_numChannels);
  //! Constructor
  Image(char *filename);
  
  //! Destructor
  ~Image();
  
  
  //! Throw error
  void Error(char *where, char *msg);
  
  
  //! Clear all
  void clear();
  //! Load image from file
  bool load(char *filename);
  //! Load image from file
  bool load(const std::string &filename);
  //! Save image to file
  bool save(const char *filename);
  
  //! Get width
  inline const int get_width() const { return width; }
  //! Get height
  inline const int get_height() const { return height; }
  //! Get number of channels
  inline int get_numChannels() const { return numChannels; }
  //! Get number of pixels
  inline int get_numPixels() const { return width * height; }
  
  //! Get all pixels
  float* pixel(const IND &index) { return &map[index * numChannels]; }
  float* pixel(const IND &col, const IND &row) { return &map[(col + row*width) * numChannels]; }
  float get_pixel(const IND &index, const IND &channel) { return pixel(index)[channel]; }
  float get_pixel(const IND &col, const IND &row, const IND &channel) { return (pixel(col, row))[channel]; }
  void set_pixel(const IND &col, const IND &row, float *color) { memcpy(&map[(col + row*width) * numChannels], color, sizeof(float) * numChannels); }
  void set_pixel(const IND &col, const IND &row, const IND &channel, const float &color) { map[(col + row*width) * numChannels + channel] = color; }
  
  //! Get all pixels for a specific channel (r,g,b, or a)
  Image  get_channel(const IND &channel);
  inline Image  r() { return get_channel(0); }
  inline Image  g() { return get_channel(1); }
  inline Image  b() { return get_channel(2); }
  inline Image  a() { return get_channel(3); }
  
  void   abs();
  Image  iabs();
  void   trunc(const float &bottomvalue, const float &topvalue);
  Image  itrunc(const float &bottomvalue, const float &topvalue);
  
  //! operators
  Image& operator=(Image const & src);
  Image& operator=(const float* pixel);
  Image& operator=(const float &value);
  bool   operator==(const Image& src);
  bool   operator!=(const Image& src);
  
  Image& operator_assign(const Image& src, void(*op)(const float &val1, float &val2));
  Image& operator_assign(const float* pixel, void(*op)(const float &val1, float &val2));
  Image& operator_assign(const float &value, void(*op)(const float &val1, float &val2));
  
  Image operator_arithmetic(const Image& src, float(*op)(const float &val1, const float &val2));
  Image operator_arithmetic(const float* pixel, float(*op)(const float &val1, const float &val2));
  Image operator_arithmetic(const float &value, float(*op)(const float &val1, const float &val2));
  
  Image& operator+=(const Image& src);
  Image& operator-=(const Image& src);
  Image& operator*=(const Image& src);
  Image& operator/=(const Image& src);
  
  Image& operator+=(const float* pixel);
  Image& operator-=(const float* pixel);
  Image& operator*=(const float* pixel);
  Image& operator/=(const float* pixel);
  
  Image& operator+=(const float &value);
  Image& operator-=(const float &value);
  Image& operator*=(const float &value);
  Image& operator/=(const float &value);
  
  Image  operator+(const Image& src);
  Image  operator-(const Image& src);
  Image  operator*(const Image& src);
  Image  operator/(const Image& src);
  
  Image  operator+(const float* pixel);
  Image  operator-(const float* pixel);
  Image  operator*(const float* pixel);
  Image  operator/(const float* pixel);
  
  Image  operator+(const float &value);
  Image  operator-(const float &value);
  Image  operator*(const float &value);
  Image  operator/(const float &value);
  
  
  void SummedAreaTable(void);
  void UnSummedAreaTable(void);
  float summedPixel(const IND &col1, const IND &row1, const IND &col2, const IND &row2, const IND &channel)
  {
    float result = get_pixel(col2, row2, channel);
    float area = (float)fabs((float)((col2 - col1 + 1) * (row2 - row1 + 1)));
    
    auto col1_tmp = col1-1;    auto row1_tmp = row1-1;
    result = result - get_pixel(col1_tmp, row2, channel);
    result = result - get_pixel(col2, row1_tmp, channel);
    result = result + get_pixel(col1_tmp, row1_tmp, channel);
    
    return result / area;
  }
  
};

#endif /* image_h */
