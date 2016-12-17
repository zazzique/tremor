
#pragma once

#include <stdint.h>

#define U8  uint8_t
#define U16 uint16_t
#define U32 uint32_t
#define U64 uint64_t
#define I8  int8_t
#define I16 int16_t
#define I32 int32_t
#define I64 int64_t


enum ScreenSize
{
	SCREEN_SIZE_SMALL = 0,
	SCREEN_SIZE_NORMAL = 1,
	SCREEN_SIZE_LARGE = 2,
	SCREEN_SIZE_XLARGE = 3
};

struct ApplicationSettings
{
	int screen_width = 800;
	int screen_height = 600;
	float pixel_scale = 1.0f;
	ScreenSize screen_size = SCREEN_SIZE_NORMAL;
	bool tablet = false;

#ifdef _WIN32
		HWND hWnd = nullptr;
#endif
};
