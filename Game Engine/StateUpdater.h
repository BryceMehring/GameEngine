#ifndef _STATE_UPDATER_
#define _STATE_UPDATER_

#include "GameState.h"

class StateUpdater
{
public:

	StateUpdater(IGameState::State& rEnum, IGameState::State state) : m_rEnum(rEnum)
	{
		m_rEnum = state;
	}
	~StateUpdater()
	{
		m_rEnum = IGameState::NOTHING;
	}

private:

	IGameState::State& m_rEnum;

};

#endif