
#pragma once

#include "Common.h"
#include "Core.h"

void Game_Init(Core *core);
void Game_Release();

void Game_Process();
void Game_Render();

void Game_Pause();

