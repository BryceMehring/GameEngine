#ifndef _GAMEOFLIFEFACTORY_
#define _GAMEOFLIFEFACTORY_

#include "GameStateFactory.h"

enum GameOfLifeState
{
	MENU,
	GAME,
};

class GameOfLifeFactory : public GameStateFactory
{
public:

	virtual ~GameOfLifeFactory() {}
	virtual IGameState* GetState(int id, class Game* pGame) const;

};


#endif