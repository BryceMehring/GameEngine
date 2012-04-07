#include "GameStateMachine.h"
#include <assert.h>

DWORD WINAPI DeleteThread(void* p)
{
	// cast parameter into a IGameState pointer
	IGameState* pState = (IGameState*)p;

	// Sleep until the state is doing nothing
	while(pState->GetState() != IGameState::NOTHING) { Sleep(100); }

	// then delete the state
	delete pState;

	// free thread handle
	assert(CloseHandle(GetCurrentThread()) <= 0);

	// close thread
	return 0;
}

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
		CreateThread(0,0,DeleteThread,m_pState,0,0);
		m_pState = nullptr;
	}
}