
#include "Common.h"
#include "GameDefines.h"
#include "GameVariables.h"

#include "Utils.h"
#include "FastMath.h"
#include "Vector.h"
#include "Timer.h"
#include "Parser.h"
#include "Files.h"
#include "Locale.h"
#include "Sound.h"
#include "TextureManager.h"
#include "Font.h"
#include "Sprites.h"
#include "GUIControls.h"
#include "ModelManager.h"
#include "Render.h"
#include "Level.h"
#include "Player.h"
#include "Monsters.h"
#include "Items.h"
#include "Weapons.h"
#include "Game.h"

GameControls gameControls;

Player player;

bool Game_LoadGameData(const char* config_filename);

void Game_Init()
{
	Game_LoadGameData("CHASM.INF");

	gameVariables.Reset();

	player.Reset();

	Level_Init();
	Level_Load();

	// TODO: plan
	//2) load car with animation, sound and so on
	//3) load game config
	//4) load all monsters from config
}

void Game_Release()
{
	Level_Release();

	// TODO: unload all info
}

bool Game_LoadGameData(const char* config_filename)
{
	FileHandler config_file;
	char str[1024];

	int count = 0;

	enum PARSING_GROUP { PG_NONE, PG_MONSTERS, PG_3D_OBJECTS, PG_WEAPONS, PG_ROCKETS, PG_GIBS, PG_BLOWS, PG_BMP_OBJECTS, PG_SOUNDS };
	int parsing_group = PG_NONE;

	if (!Parser_OpenFile(config_filename, "\n\r"))
	{
		LogPrint("Error: config '%s' not found!\n", config_filename);
		return false;
	}

	while (true)
	{
		trimwhitespace(str, Parser_GetString());

		if (str[0] == '*' || str[0] == ';')
		{
			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("END."))
		{
			break;
		}

		if (Parser_IsEqual("[MONSTERS]"))
		{
			parsing_group = PG_MONSTERS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: monsters count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[3D_OBJECTS]"))
		{
			parsing_group = PG_3D_OBJECTS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: 3d objects count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[WEAPONS]"))
		{
			parsing_group = PG_WEAPONS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: weapons count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[ROCKETS]"))
		{
			parsing_group = PG_ROCKETS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: projectiles count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[GIBS]"))
		{
			parsing_group = PG_GIBS;

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[BLOWS]"))
		{
			parsing_group = PG_BLOWS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: blows count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[BMP_OBJECTS]"))
		{
			parsing_group = PG_BMP_OBJECTS;

			if (!Parser_ReadLine())
				break;

			trimwhitespace(str, Parser_GetString());

			sscanf(str, "%d", &count); // TODO: bmp objects count

			if (!Parser_ReadLine())
				break;

			continue;
		}

		if (Parser_IsEqual("[SOUNDS]"))
		{
			parsing_group = PG_SOUNDS;

			if (!Parser_ReadLine())
				break;

			continue;
		}

		switch (parsing_group)
		{
			case PG_MONSTERS:
			{
				for (int i = 0; i < count; i ++)
				{
					char car_filename[32];
					int walk_radius;
					int attack_radius;
					int speed;
					int rotation_speed;
					int life;
					int kick;
					int rocket;
					int sep_limit;

					trimwhitespace(str, Parser_GetString());

					if (str[0] == '*' || str[0] == ';')
					{
						if (!Parser_ReadLine())
							break;

						continue;
					}

					sscanf(str, "%s %d %d %d %d %d %d %d %d", car_filename, &walk_radius, &attack_radius, &speed, &rotation_speed, &life, &kick, &rocket, &sep_limit);

					if (!Parser_ReadLine())
						break;
					
					Monsters_LoadInfo(i, car_filename, walk_radius, attack_radius, speed, rotation_speed, life, kick, rocket, sep_limit);
				}

				parsing_group = PG_NONE;

				break;
			}

			case PG_3D_OBJECTS:
			{
				for (int i = 0; i < count; i ++)
				{
					int shadow;
					int action_code; // TODO: action code
					char item_model[32];
					char item_animation[32];

					trimwhitespace(str, Parser_GetString());

					if (str[0] == '*' || str[0] == ';')
					{
						if (!Parser_ReadLine())
							break;

						continue;
					} 

					sscanf(str, "%*d %d %*d %*d %d %*d %*d %*d %s %s", &shadow, &action_code, item_model, item_animation);

					if (!Parser_ReadLine())
						break;
					
					Items_LoadInfo(i, item_model, item_animation, shadow, action_code);
				}

				parsing_group = PG_NONE;

				break;
			}

			case PG_WEAPONS:
			{
				for (int i = 0; i < count; i ++)
				{
					char weapon_model[32];
					char weapon_ani_idle[32];
					char weapon_ani_shoot[32];

					trimwhitespace(str, Parser_GetString());

					sscanf(str, "MODEL=%s", weapon_model);

					if (!Parser_ReadLine())
						break;


					trimwhitespace(str, Parser_GetString());

					sscanf(str, "STAT =%s", weapon_ani_idle);

					if (!Parser_ReadLine())
						break;


					trimwhitespace(str, Parser_GetString());

					sscanf(str, "SHOOT=%s", weapon_ani_shoot);

					if (!Parser_ReadLine())
						break;


					trimwhitespace(str, Parser_GetString());

					if (str[0] == '*' || str[0] == ';')
					{
						if (!Parser_ReadLine())
							break;

						continue;
					}

					int rocket_type, reload_time, y_shift, RZ0, ammo_pickup, ammo_limit, ammo_start, rockets_count;

					sscanf(str, "%d %d %d %d %d %d %d %d",
						&rocket_type,
						&reload_time,
						&y_shift,
						&RZ0,
						&ammo_pickup,
						&ammo_limit,
						&ammo_start,
						&rockets_count);

					if (!Parser_ReadLine())
						break;
					
					Weapons_LoadInfo(i, weapon_model, weapon_ani_idle, weapon_ani_shoot, rocket_type, reload_time, y_shift, RZ0, ammo_pickup, ammo_limit, ammo_start, rockets_count);
				}

				parsing_group = PG_NONE;

				break;
			}





			////// 
			/*8                ; RType, RldTm, YSh, RZ0, DAm, Limit, Start,   RCount
MODEL=ogun6.3o
STAT =ogun6.ani
SHOOT=ogatck.ani
                       3   86  32  180  20   120  25      1*/
		}
			
		if (!Parser_ReadLine())
			break;
	}

	Parser_CloseFile();

	return true;
}

void Game_Process()
{
	Level_Process();

	// Player controls
	Vector2D player_rotation;

	GUI_GetBackgroundMovements(&player_rotation.x, &player_rotation.y);
    
	player_rotation.x *= 0.1875f;
	player_rotation.y *= 0.1875f ;
	
	player.ProcessControls(gameControls.movement, player_rotation, coreVariables.delta_t);
	player.Prosess(coreVariables.delta_t);

	//
}

void Game_Render()
{
	float background_color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	Render_Clear(background_color[0], background_color[1], background_color[2], background_color[3]);

	Render_EnableDepthMask();
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

	Level_Render();
}

void Game_Pause()
{
}

