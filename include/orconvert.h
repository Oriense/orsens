#ifndef ORCONVERT_H_INCLUDED
#define ORCONVERT_H_INCLUDED

#include "orcv.h"

///TODO rename
void YUV2RGB(const unsigned char y, const unsigned char u, const unsigned char v, unsigned char* r,
             unsigned char* g, unsigned char* b);
void uyvy2rgb(char *YUV, char *RGB, int NumPixels);
void yuyv2rgb(char *YUV, char *RGB, int NumPixels);
void rgb242rgb(char *YUV, char *RGB, int NumPixels);
float ang2rad(float ang);
float rad2ang(float rad);

#endif // ORCONVERT_H_INCLUDED
