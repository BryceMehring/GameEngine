#include "GameStateMachine.h"
#include "GameStateFactory.h"
#include "FileManager.h"
#include "RTTI.h"
#include "Game.h"
#include <assert.h>

void GameStateMachine::SetState(IGameState* pState, Game& game)
{
	// Update change to log
	char buffer[64];
	sprintf_s(buffer,"Changing state to: %s",pState->GetType()->GetName().c_str());
	FileManager::Instance().WriteToLog(buffer);

	game.GetWindow().SetWinCaption(pState->GetType()->GetName());

	RemoveState(game);

	m_pState = pState;

	if(m_pState)
	{
		m_pState->Init(game);
	}
}

void GameStateMachine::RemoveState(Game& game)
{
	// If there is a state, 
	if(m_pState)
	{
		// todo: I could add this line of code to SetState, to fix the code
		m_states.push(m_pState->GetType()->GetName());

		// Delete the state once the state is ready to be deleted.
		m_pState->Destroy(game);
		delete m_pState;
		m_pState = nullptr;
	}
}

void GameStateMachine::LoadPreviousState(Game& game)
{
	if(m_states.empty() == false)
	{
		GameStateFactory& gsf = GameStateFactory::Instance();

		IGameState* pState = gsf.CreateState(m_states.top());
		m_states.pop();

		SetState(pState,game);
	}
}