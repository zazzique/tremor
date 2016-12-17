
#pragma once

#include "Common.h"
#include "CommonTypes.h"

struct CoreVariables
{
	bool tablet;

	int screen_width, screen_height;
	float screen_center_x, screen_center_y;
	float pixel_scale;
	U32 screen_size;

	bool debug_mode;

	float delta_t;

	CoreVariables()
	{
		Reset();
	}

	void Reset()
	{
		tablet = false;
		screen_width = 0;
		screen_height = 0;
		screen_center_x = 0;
		screen_center_y = 0;
		pixel_scale = 0;
		screen_size = SCREEN_SIZE_NORMAL;
		debug_mode = false;
		delta_t = 0;
	}
};

extern CoreVariables coreVariables;


