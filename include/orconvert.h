#ifndef ORCONVERT_H_INCLUDED
#define ORCONVERT_H_INCLUDED

#include "orcv.h"

void yuyv2rgb(char *YUV, char *RGB, int NumPixels);
float ang2rad(float ang);
float rad2ang(float rad);

#endif // ORCONVERT_H_INCLUDED
