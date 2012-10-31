#pragma once
#include "istatecreator.h"

// What if these objects are singleton?
class GameOfLifeCreator : public IStateCreator
{
public:

	GameOfLifeCreator();
	virtual IGameState* Create() const;

private:



};

