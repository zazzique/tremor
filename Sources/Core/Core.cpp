
#include "Common.h"

#include "FastMath.h"
#include "Vector.h"
#include "Files.h"
#include "Timer.h"
#include "Texture.h"
#include "Game.h"
#include "Core.h"

Core::Core(const ApplicationSettings& settings)
{
	appSettings = settings;

	debug_mode = false;

	viewport.width = ((float)appSettings.screen_width / appSettings.pixel_scale);
	viewport.height = ((float)appSettings.screen_height / appSettings.pixel_scale);

	viewport.center_x = viewport.width * 0.5f;
	viewport.center_y = viewport.height * 0.5f;

	for (int i = 0; i < 16; i ++)
	{
		time_delta_buffer[i] = 1.0f / (float)DESIRED_FPS;
	}

	Files_Init();

	renderer = Renderer::Create(appSettings);

	//TexManager_Init();
	//Font_Init();
	//Sprites_Init();

	Game_Init(this);
}

Core::~Core()
{
	Game_Release();

	//Sprites_Release();
	//Font_Release();
	//TexManager_Release();

	Texture::DeleteAll();

	if (renderer != nullptr) // TODO: template func for safe delete
	{
		delete renderer;
		renderer = nullptr;
	}

	Files_Release();
}

void Core::Process()
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
	
	delta_t = 0.0f;
		
	for (int i = 0; i < 16; i ++)
	{
		delta_t += time_delta_buffer[i];
	}
	
	delta_t *= (1.0f / 16.0f);

	Game_Process();
}

void Core::Render()
{
	Game_Render();
	
    //GUI_DrawControls();
    //Sprites_Render();
    //Font_Render();

	renderer->SwapBuffers();
}

void Core::Pause()
{
    Game_Pause();
}

void Core::RestoreResources()
{
	//
}

void Core::UnloadResources()
{
	//
}

void Core::InputTouchBegan(float x, float y)
{
	//GUI_TouchBegan(x, y);
}

void Core::InputTouchMoved(float x, float y)
{
    //int touch_id = GUI_GetTouchByLocation(x, y);
    
    //if (touch_id < 0)
    //    return;
    
	//GUI_TouchMoved(touch_id, x, y);
}

void Core::InputTouchEnded(float x, float y)
{
    //int touch_id = GUI_GetTouchByLocation(x, y);
    
    //if (touch_id < 0)
    //    return;
    
	//GUI_TouchEnded(touch_id, x, y);
}

void Core::TouchesReset()
{
	//GUI_TouchesReset();
}


