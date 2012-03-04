#include "GameStateMachine.h"



void GameStateMachine::SetState(IGameState* pState, Game* pGame, int stateId)
{
	if(m_pState)
	{
		RemoveState(pGame);
	}

	m_pState = pState;

	if(m_pState)
	{
		m_pState->Init(pGame);
		m_pState->m_iStateId = stateId;
	}
}

void GameStateMachine::RemoveState(Game* pGame)
{
	if(m_pState)
	{
		m_pState->Destroy(pGame);
		delete m_pState;
		m_pState = nullptr;
	}
}