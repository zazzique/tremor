
#pragma once

#include "Common.h"
#include "Renderer.h"

class Core
{
public:
	ApplicationSettings appSettings;

	struct Viewport
	{
		float width, height;
		float center_x, center_y;
	};
	Viewport viewport;


	bool debug_mode = false;

private:

	float delta_t = 1.0f / DESIRED_FPS;
	float time_delta_buffer[16];

public:
	
	Renderer* renderer = nullptr;

public:

	Core(const ApplicationSettings& appSettings);
	~Core();

	void Process();
	void Render();

	void Pause();

	void RestoreResources();
	void UnloadResources();

	void InputTouchBegan(float x, float y);
	void InputTouchMoved(float x, float y);
	void InputTouchEnded(float x, float y);
	void TouchesReset();
};
