#include "GameOfLifeFactory.h"
#include "GameOfLifeMenu.h"
#include "GameOfLife.h"

IGameState* GameOfLifeFactory::GetState(int id) const
{
	IGameState* pState = nullptr;

	// todo: could make this dynamic... 
	switch(id)
	{
	case MENU:
		pState = new GameOfLifeMenu();
		break;

	case GAME:
		pState = new ::GameOfLife();
		break;
	default:

		break;

		// todo: error

	};

	return pState;
}