#include "AstroidsCreator.h"
#include "Astroids.h"
#include "GameStateFactory.h"
#include "GameOfLife.h"

// global instance of AstroidsCreator
AstroidsCreator AstroidsCreatorinstance;

AstroidsCreator::AstroidsCreator()
{
	GameStateFactory::Instance().Register(Astroids::s_rtti.GetName(),this);
}

IGameState* AstroidsCreator::Create() const
{
	return new Astroids();
}