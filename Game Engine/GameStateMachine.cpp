#include "GameStateMachine.h"
#include <assert.h>

DWORD WINAPI DeleteThread(void* p)
{
	// cast parameter into a IGameState pointer
	IGameState* pState = (IGameState*)p;

	// Sleep until the state is doing nothing
	while(pState->GetState() != IGameState::NOTHING) { Sleep(100); }

	// once the state is doing nothing, calling delete is not a race condition
	// because we already changed the state of GameStateMachine, all we need to do now is
	// delete the object

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
	// If there is a state, 
	if(m_pState)
	{
		// Destroy is
		m_pState->Destroy(pGame);

		// Delete the state once the state is ready to be deleted.
		CreateThread(0,0,DeleteThread,m_pState,0,0);
		m_pState = nullptr;
	}
}