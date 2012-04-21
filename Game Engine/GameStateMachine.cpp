#include "GameStateMachine.h"
#include <assert.h>
#include <algorithm>

struct DeleteThreadInfo
{
	Game* pGame;
	IGameState* pState;
};

DWORD WINAPI DeleteThread(void* p)
{
	// cast parameter into a IGameState pointer
	DeleteThreadInfo* pInfo = (DeleteThreadInfo*)p;
	Game* pGame = pInfo->pGame;
	IGameState* pState = pInfo->pState;

	// Sleep until the state is doing nothing
	while(pState->GetState() != IGameState::NOTHING) { Sleep(100); }

	// once the state is doing nothing, calling delete is not a race condition
	// because we already changed the state of GameStateMachine, all we need to do now is
	// delete the object

	pState->Destroy();

	// then delete the state
	delete pState;
	delete pInfo;

	// free thread handle
	assert(CloseHandle(GetCurrentThread()) <= 0);

	// close thread
	return 0;
}

GameStateMachine::~GameStateMachine()
{
	m_States.top()->Destroy();

	DestroyStack(m_States);
	DestroyStack(m_StatesPoped);
}

void GameStateMachine::DestroyStack(GameStateMachine::StackType& stack)
{
	while(!stack.empty())
	{
		delete stack.top();
		stack.pop();
	}
}

void GameStateMachine::PushState(IGameState* pState, Game* pGame, int stateId)
{
	pState->m_iStateId = stateId;

	m_States.push(pState);
}

void GameStateMachine::Push()
{
	if(!m_StatesPoped.empty())
	{
		IGameState* pNewState = m_StatesPoped.top();

		pNewState->Init();
		m_States.top()->Destroy();

		m_States.push(pNewState);
		m_StatesPoped.pop();
	}
}

void GameStateMachine::Pop()
{
	if(m_States.size() > 1)
	{
		IGameState* pPopedState = m_States.top();
		pPopedState->Destroy();

		m_StatesPoped.push(pPopedState);
		m_States.pop();

		m_States.top()->Init();
	}
}

IGameState* GameStateMachine::GetState()
{
	if(m_States.empty()) { return nullptr; }

	return m_States.top();
}


/*void GameStateMachine::SetState(IGameState* pState, Game* pGame, int stateId)
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
		DeleteThreadInfo* pThreadInfo = new DeleteThreadInfo;
		pThreadInfo->pGame = pGame;
		pThreadInfo->pState = m_pState;

		// Delete the state once the state is ready to be deleted.
		CreateThread(0,0,DeleteThread,pThreadInfo,0,0);
		m_pState = nullptr;
	}
}*/