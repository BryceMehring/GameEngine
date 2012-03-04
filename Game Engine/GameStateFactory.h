#ifndef _GAMESTATEFACTORY_
#define _GAMESTATEFACTORY_

#include "GameState.h"

class GameStateFactory
{
public:

	virtual ~GameStateFactory() {}
	virtual IGameState* GetState(int id) const = 0;
};

#endif _GAMESTATEFACTORY_