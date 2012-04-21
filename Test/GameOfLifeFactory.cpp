#include "GameOfLifeFactory.h"
#include "GameOfLifeMenu.h"
#include "GameOfLife.h"

IGameState* GameOfLifeFactory::GetState(int id, Game* pGame) const
{

	IGameState* pState = nullptr;

	// todo: could make this dynamic... 
	switch(id)
	{
	case MENU:
		pState = new GameOfLifeMenu(pGame);
		break;

	case GAME:
		pState = new ::GameOfLife(pGame);
		break;
	default:

		break;

		// todo: error

	};

	return pState;
}