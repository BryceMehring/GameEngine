#include "GameOfLifeMenuCreator.h"
#include "GameOfLifeMenu.h"

GameOfLifeMenuCreator GameOfLifeMenuCreatorinstance;

GameOfLifeMenuCreator::GameOfLifeMenuCreator()
{
	GameStateFactory::Instance().Register(GameOfLifeMenu::s_rtti.GetName(),this);
}

IGameState* GameOfLifeMenuCreator::Create() const
{
	return new GameOfLifeMenu();
}