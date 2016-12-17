
#ifndef _GAME_VARIABLES_H_
#define _GAME_VARIABLES_H_

struct GameVariables
{
	bool tmp;

	GameVariables()
	{
		Reset();
	}

	void Reset()
	{
		tmp = false;
	}
};

extern GameVariables gameVariables;

#endif /* _GAME_VARIABLES_H_ */

