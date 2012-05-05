#include "GameOfLifeCreator.h"

GameOfLifeCreator GameOfLifeGameCreatorinstance;

GameOfLifeCreator::GameOfLifeCreator()
{
	GameStateFactory::Instance().Register(GameOfLife::s_rtti.GetName(),this);
}

IGameState* GameOfLifeCreator::Create() const
{
	return new GameOfLife();
}