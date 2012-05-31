#pragma once
#include "istatecreator.h"
#include "GameStateFactory.h"

class PongCreator : public IStateCreator
{
public:

	PongCreator();
	virtual IGameState* Create() const;

} ;