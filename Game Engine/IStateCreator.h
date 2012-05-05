#pragma once

#include "GameState.h"

class IStateCreator
{
public:

	virtual ~IStateCreator() {}
	virtual IGameState* Create() const = 0;
};

