#include "PongCreator.h"
#include "Pong.h"

PongCreator PongCreatorInstance;

PongCreator::PongCreator()
{
	GameStateFactory::Instance().Register(Pong::s_rtti.GetName(),this);
}

IGameState* PongCreator::Create() const
{
	return new Pong();
}