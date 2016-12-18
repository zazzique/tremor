
#pragma once

#include "Common.h"

#define DEG2RAD(a) ((a) * 0.01745329251994329576f)
#define RAD2DEG(a) ((a) * 57.2957795130823208767f)

#define pi 3.1415926536f

#ifndef SQR
    #define SQR(x)     ((x) * (x))
#endif

#ifndef SGN
    #define SGN(x)     ((x) < 0 ? -1 : 1)
#endif

#ifndef MAX
    #define MAX(a, b)  ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
    #define MIN(a, b)  ((a) < (b) ? (a) : (b))
#endif

#ifndef ROUNDF
    #define ROUNDF(x)  (floorf((x) + 0.5f))
#endif

#ifndef LERP
    #define LERP(a, b, k)  ((a) + (k) * ((b) - (a)))
#endif

#ifndef CLAMP
    #define CLAMP(x, min, max)  ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif

void DeltaFunc(float *a, float b, float d);

float MathWave(float a);
