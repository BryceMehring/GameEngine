#pragma once
#include "istatecreator.h"

// What if these objects are singleton?
class AstroidsCreator : public IStateCreator
{
public:

	AstroidsCreator();
	virtual IGameState* Create() const;

};
