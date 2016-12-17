
#include "Common.h"

#include "FastMath.h"
#include "Vector.h"
#include "Files.h"
#include "Timer.h"
#include "Render.h"
#include "TextureManager.h"
#include "Font.h"
#include "Sprites.h"
#include "GUIControls.h"
#include "ModelManager.h"
#include "Sound.h"
#include "Game.h"
#include "Core.h"

float time_delta_buffer[16];

void Core_Init(int init_screen_width, int init_screen_height, float init_pixel_scale, U32 init_screen_size)
{
	coreVariables.Reset();

	coreVariables.debug_mode = false;

	coreVariables.pixel_scale = init_pixel_scale;
	coreVariables.screen_size = init_screen_size;

	coreVariables.screen_width = (int)((float)init_screen_width / init_pixel_scale);
	coreVariables.screen_height = (int)((float)init_screen_height / init_pixel_scale);

	coreVariables.screen_center_x = (float)coreVariables.screen_width * 0.5f;
	coreVariables.screen_center_y = (float)coreVariables.screen_height * 0.5f;

	if (coreVariables.screen_size >= SCREEN_SIZE_LARGE)
		coreVariables.tablet = true;
	else
		coreVariables.tablet = false;

	for (int i = 0; i < 16; i ++)
	{
		time_delta_buffer[i] = 1.0f / (float)DESIRED_FPS;
	}

	Files_Init();
	//FastMath_Init();
    //Sound_Init();
	Render_Init(init_screen_width, init_screen_height);
	TexManager_Init();
	Font_Init();
	Sprites_Init();
	GUI_Init();
	//ModelManager_Init();

	Game_Init();
}


void Core_Process()
{
	double prev_time;
    static bool init_game = true;
	double frame_time;
    
	if (init_game)
	{
		Timer_Start();

		frame_time = 0.0;

        init_game = false;
	}
	else
	{
		frame_time = Timer_GetCurrentTime();

		Timer_Start();
	}
    
	for (int i = 0; i < 15; i ++)
	{
		time_delta_buffer[i] = time_delta_buffer[i + 1];
	}
	
	time_delta_buffer[15] = (float)frame_time;
	
	if (time_delta_buffer[15] <= 0.0f)
		time_delta_buffer[15] = 1.0f / (float)DESIRED_FPS;
	if (time_delta_buffer[15] > 0.25f)
		time_delta_buffer[15] = 0.25f;
	
	coreVariables.delta_t = 0.0f;
		
	for (int i = 0; i < 16; i ++)
	{
		coreVariables.delta_t += time_delta_buffer[i];
	}
	
	coreVariables.delta_t *= (1.0f / 16.0f);

	Game_Process();
}

void Core_Render()
{
	Game_Render();
	
    GUI_DrawControls();
    Sprites_Render();
    Font_Render();
	GUI_RenderFade();
}


void Core_Release()
{
	Game_Release();

	ModelManager_Release();
	GUI_Release();
	Sprites_Release();
	Font_Release();
	TexManager_Release();
	Render_Release();
    Sound_Release();
	Files_Release();
}

void Core_Pause()
{
    Game_Pause();
}

void Core_RestoreResources()
{
	Render_Init((U32)(coreVariables.screen_width * coreVariables.pixel_scale), (U32)(coreVariables.screen_height * coreVariables.pixel_scale));
	TexManager_LoadAll();
}

void Core_UnloadResources()
{
	TexManager_UnloadAll();
	Render_Release();
}

void Core_InputTouchBegan(float x, float y)
{
	GUI_TouchBegan(x, y);
}

void Core_InputTouchMoved(float x, float y)
{
    int touch_id;
    
    touch_id = GUI_GetTouchByLocation(x, y);
    
    if (touch_id < 0)
        return;
    
	GUI_TouchMoved(touch_id, x, y);
}

void Core_InputTouchEnded(float x, float y)
{
    int touch_id;
    
    touch_id = GUI_GetTouchByLocation(x, y);
    
    if (touch_id < 0)
        return;
    
	GUI_TouchEnded(touch_id, x, y);
}

void Core_TouchesReset()
{
	GUI_TouchesReset();
}


