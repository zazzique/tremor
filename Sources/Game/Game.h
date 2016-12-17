
#ifndef _GAME_H_
#define _GAME_H_

#include "Common.h"

struct GameControls
{
	Vector2D movement;

	GameControls()
	{
		movement.x = 0;
		movement.y = 0;
	}
};

extern GameControls gameControls;

void Game_Init();
void Game_Release();

void Game_Process();
void Game_Render();

void Game_Pause();

#endif /* _GAME_H_ */
