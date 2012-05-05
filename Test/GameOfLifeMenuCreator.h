#pragma once
#include "istatecreator.h"
#include "GameStateFactory.h"

class GameOfLifeMenuCreator : public IStateCreator
{
public:

	GameOfLifeMenuCreator();
	virtual IGameState* Create() const;
	
};

