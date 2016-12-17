//
//  FastMath.h
//  Carnivores
//
//  Created by Dmitry Nechay on 18.08.09.
//  Copyright 2009 Action Forms. All rights reserved.
//

#ifndef _UTILS_H_
#define _UTILS_H_

#include "Common.h"

inline U16 BYTESWAP(U16 x)
{
	U8 hi = (x & 0xff00) >> 8;
	U8 lo = (x & 0xff);
	return lo << 8 | hi;
}

inline I16 BYTESWAP(I16 x)
{
	U8 hi = (x & 0xff00) >> 8;
	U8 lo = (x & 0xff);
	return lo << 8 | hi;
}

void RGB2HSV(float *color_hsv, float *color_rgb);
void HSV2RGB(float *color_rgb, float *color_hsv);

float RandFloat();

int strcicmp(char const* a, char const* b);
void trimwhitespace(char* out, char const* in);

#endif /* _UTILS_H_ */
