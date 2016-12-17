//
//  FastMath.m
//  Carnivores
//
//  Created by Dmitry Nechay on 18.08.09.
//  Copyright 2009 Action Forms. All rights reserved.
//

#include "Common.h"
#include "FastMath.h"

float AngleDifference(float a, float b)
{
	a -= b;
	if (a < 0)
		a = -a;
	if (a > pi)
		a = 2.0f * pi - a;
	return a;
}

void DeltaFunc(float *a, float b, float d)
{
	if (b > *a)
	{
		*a += d;
		
		if (*a > b)
			*a = b;
	}
	else
	{
		*a -= d;
		
		if (*a < b)
			*a = b;
	}
}

float MathWave(float a)
{
	return (-cosf(a * pi) + 1.0f) * 0.5f;
}


