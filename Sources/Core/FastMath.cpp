
#include "Common.h"
#include "FastMath.h"

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


