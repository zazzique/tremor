
#include "Common.h"
#include "FastMath.h"
#include "Utils.h"

void RGB2HSV(float *color_hsv, float *color_rgb)
{
	float cmin, cmax, cdelta;
	
	cmin = MIN(MIN(color_rgb[0], color_rgb[1]), color_rgb[2]);
	cmax = MAX(MAX(color_rgb[0], color_rgb[1]), color_rgb[2]);
	
	color_hsv[2] = cmax;
	
	cdelta = cmax - cmin;
	
	if (cmax != 0)
		color_hsv[1] = cdelta / cmax;
	else
	{
		color_hsv[1] = 0;
		color_hsv[0] = -1.0f;
		return;
	}
	
	if (color_rgb[0] == cmax)
	{
		color_hsv[0] = (color_rgb[1] - color_rgb[2]) / cdelta;
	}
	else if (color_rgb[1] == cmax)
	{
		color_hsv[0] = 2 + (color_rgb[2] - color_rgb[0]) / cdelta;
	}
	else
	{
		color_hsv[0] = 4 + (color_rgb[0] - color_rgb[1]) / cdelta;
	}
	
	color_hsv[0] *= 60.0f;
	
	if (color_hsv[0] < 0.0f)
		color_hsv[0] += 360.0f;
}


void HSV2RGB(float *color_rgb, float *color_hsv)
{
	float f, p, q, t;
	int i;
	
	if (color_hsv[1] == 0)
	{
		color_rgb[0] = color_hsv[2];
		color_rgb[1] = color_hsv[2];
		color_rgb[2] = color_hsv[2];
		return;
	}
	
	color_hsv[0] /= 60.0f;
	i = (int)floorf(color_hsv[0]);
	f = color_hsv[0] - i;
	p = color_hsv[2] * (1.0f - color_hsv[1]);
	q = color_hsv[2] * (1.0f - color_hsv[1] * f);
	t = color_hsv[2] * (1.0f - color_hsv[1] * (1.0f - f));
	
	switch (i)
	{
		case 0:
			color_rgb[0] = color_hsv[2];
			color_rgb[1] = t;
			color_rgb[2] = p;
			break;
		case 1:
			color_rgb[0] = q;
			color_rgb[1] = color_hsv[2];
			color_rgb[2] = p;
			break;
		case 2:
			color_rgb[0] = p;
			color_rgb[1] = color_hsv[2];
			color_rgb[2] = t;
			break;
		case 3:
			color_rgb[0] = p;
			color_rgb[1] = q;
			color_rgb[2] = color_hsv[2];
			break;
		case 4:
			color_rgb[0] = t;
			color_rgb[1] = p;
			color_rgb[2] = color_hsv[2];
			break;
		default:
			color_rgb[0] = color_hsv[2];
			color_rgb[1] = p;
			color_rgb[2] = q;
			break;
	}
}

float RandFloat()
{
	return (float)rand()/(float)RAND_MAX;
}

int strcicmp(char const* a, char const* b)
{
    for (;; a++, b++)
	{
        int d = tolower(*a) - tolower(*b);
        if (d != 0 || !*a)
            return d;
    }
}

void trimwhitespace(char* out, char const* in)
{
	char* c = out;

	for (int i = 0; i < strlen(in) + 1; i ++)
	{
        if (in[i] == ' ' || in[i] == '\t')
		{
			if (i == 0)
				continue;

			if (in[i - 1] == ' ' || in[i - 1] == '\t')
				continue;
		}

		*c = in[i];
		c ++;
    }
}


