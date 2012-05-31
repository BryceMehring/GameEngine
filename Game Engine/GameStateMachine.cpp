#include "GameStateMachine.h"
#include "GameStateFactory.h"
#include "FileManager.h"
#include "RTTI.h"
#include <assert.h>

void GameStateMachine::SetState(IGameState* pState, Game* pGame)
{
	// Update change to log
	char buffer[64];
	sprintf_s(buffer,"Changing state to: %s",pState->GetType()->GetName().c_str());
	FileManager::Instance().WriteToLog(buffer);

	RemoveState(pGame);

	m_pState = pState;

	if(m_pState)
	{
		m_pState->Init(pGame);
	}
}

void GameStateMachine::RemoveState(Game* pGame)
{
	// If there is a state, 
	if(m_pState)
	{
		m_states.push(m_pState->GetType()->GetName());

		// Delete the state once the state is ready to be deleted.
		m_pState->Destroy(pGame);
		delete m_pState;
		m_pState = nullptr;
	}
}

void GameStateMachine::LoadPreviousState(Game* pGame)
{
	if(m_states.empty() == false)
	{
		GameStateFactory& gsf = GameStateFactory::Instance();

		IGameState* pState = gsf.CreateState(m_states.top());
		m_states.pop();

		SetState(pState,pGame);
	}
}