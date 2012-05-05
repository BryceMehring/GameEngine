#pragma once
#include "istatecreator.h"
#include "Menu.h"
#include "GameStateFactory.h"

class MenuCreator : public IStateCreator
{
public:


	MenuCreator()
	{
		GameStateFactory::Instance().Register(Menu::ID(),this);
	}

	virtual ~MenuCreator() {}
	virtual IGameState* Create(class Game* pGame) const
	{
		return new Menu(pGame);
	}

} instance;

