
#include "Common.h"

#include "Utils.h"
#include "FastMath.h"
#include "Vector.h"
#include "Timer.h"
#include "Files.h"
#include "Locale.h"
#include "Texture.h"
#include "Font.h"
#include "Sprites.h"
#include "Renderer.h"
#include "Game.h"

void Game_Init()
{
	//
}

void Game_Release()
{
	//
}

void Game_Process()
{
	//
}

void Game_Render()
{
	//float background_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	//Render_Clear(background_color[0], background_color[1], background_color[2], background_color[3]);

	/*Render_EnableDepthMask();
    Render_EnableDepthTest();
	Render_EnableFaceCulling();
	Render_EnableAlphaTest();
	Render_SetBlendFunc(TR_SRC_ALPHA, TR_ONE_MINUS_SRC_ALPHA);
    Render_EnableBlend();

	Camera camera;
	player.GetCamera(&camera);
		
	const float z_near = 8.0f;
	const float z_far = 8192.0f;
	
	Render_SetProjectionFrustum(z_near, z_far, 90.0f, 60.0f);
    
	Render_MatrixRotate(-camera.gamma, 0.0f, 0.0f, 1.0f);
	Render_MatrixRotate(-camera.beta, 1.0f, 0.0f, 0.0f);
	Render_MatrixRotate(camera.alpha, 0.0f, 1.0f, 0.0f);

	Render_MatrixTranslate(-camera.x, -camera.y, -camera.z);

	Level_Render();*/
}

void Game_Pause()
{
	//
}

