#include "GameStateMachine.h"
#include "GameStateFactory.h"
#include "FileManager.h"
#include "RTTI.h"
#include "Game.h"
#include "StringAlgorithms.h"

#include <assert.h>
#include <algorithm>

using namespace std;

GameStateMachine::GameStateMachine() : m_pCurrentState(nullptr) {}

GameStateMachine::~GameStateMachine()
{
	for(auto iter = m_states.begin(); iter != m_states.end(); ++iter)
	{
		delete iter->second;
	}

	// m_pCurrentState is inserted into the map, so it already got deleted
	m_pCurrentState = nullptr;
}

void GameStateMachine::AddState(const std::string& state)
{
	// ignore returned value from RAddState
	RAddState(state);
}

std::map<std::string,IGameState*>::iterator GameStateMachine::RAddState(const std::string& state)
{
	// find new state
	std::map<std::string,IGameState*>::iterator iter = m_states.find(state);

	// if the state is already loaded
	if(iter == m_states.end())
	{
		IGameState* pNewState = GameStateFactory::Instance().CreateState(state);
		iter = m_states.insert(make_pair(state,pNewState)).first;
	}

	return iter;
}

void GameStateMachine::SetState(const std::string& state, Game& game)
{
	// Update change to log
	char buffer[64];
	sprintf_s(buffer,"Changing state to: %s",state.c_str());
	FileManager::Instance().WriteToLog(buffer);

	// update window caption
	game.GetWindow().SetWinCaption(state);

	// remove current state
	RemoveState(game);

	// set new states
	m_pCurrentState = RAddState(state)->second;

	m_pCurrentState->Init(game);
	
}

void GameStateMachine::RemoveState(Game& game)
{
	// If there is a state, 
	if(m_pCurrentState)
	{
		m_stateStack.push(m_pCurrentState->GetType()->GetName());

		//m_states.push(m_pState->GetType()->GetName());
		m_pCurrentState->Destroy(game);

		m_pCurrentState = nullptr;
	}
}

void GameStateMachine::ClearAllStates(Game& game)
{
	for(auto iter = m_states.begin(); iter != m_states.end(); ++iter)
	{
		iter->second->Destroy(game);
		delete iter->second;
	}

	m_states.clear();

	m_pCurrentState = nullptr;
}

void GameStateMachine::LoadPreviousState(Game& game)
{
	if(!m_stateStack.empty())
	{
		GameStateFactory& gsf = GameStateFactory::Instance();

		if(m_pCurrentState->GetType()->GetName() != m_stateStack.top())
		{
			SetState(m_stateStack.top(),game);
			m_stateStack.pop();
		}
	}
}