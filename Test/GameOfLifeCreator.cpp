#include "GameOfLifeCreator.h"
#include "GameOfLife.h"

// global instance of GameOfLifeCreator creator
GameOfLifeCreator GameOfLifeGameCreatorinstance;

GameOfLifeCreator::GameOfLifeCreator()
{
	GameStateFactory::Instance().Register(GOL::s_rtti.GetName(),this);
}

IGameState* GameOfLifeCreator::Create() const
{
	return new GOL();
}