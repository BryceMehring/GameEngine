#include "Game.h"
#include "FileManager.h"
#include "RTTI.h"
#include "StringAlgorithms.h"

#include <cassert>
#include <algorithm>

#define GLFW_NO_GLU
#include <GL/glfw.h>

using namespace std;

GameStateMachine::GameStateMachine() : m_pCurrentState(nullptr) {}

GameStateMachine::~GameStateMachine()
{
	/*for(auto iter = m_states.begin(); iter != m_states.end(); ++iter)
	{
		delete iter->second;
	}*/

	// m_pCurrentState is inserted into the map, so it already got deleted
	//m_pCurrentState = nullptr;
}

/*void GameStateMachine::AddState(const std::string& state)
{
	// ignore returned value from RAddState
	RAddState(state);
}*/

/*std::map<std::string,IGameState*>::iterator GameStateMachine::RAddState(const std::string& state)
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
}*/

void GameStateMachine::SetState(const std::string& state, Game& game)
{
	string path = "..\\Game Plugins\\" + state + ".extension";
	//if(game.GetPM().Good(path.c_str()))
	{
		// remove current state
		RemoveState(game);

		IPlugin* pPlugin = game.GetPM().LoadDLL(path.c_str());

		string resourcePath = "..\\" + string(pPlugin->GetName()) + "\\resource.r";
		game.GetRenderer().GetResourceManager().LoadResourceFile(resourcePath);

		m_pCurrentState = static_cast<IGameState*>(pPlugin);
		m_pCurrentState->Init(game);

		// Update change to log
		string buffer = "Changing state to: " + state;
		FileManager::Instance().WriteToLog(buffer);

		// update window caption
		glfwSetWindowTitle(state.c_str());
	}
}

void GameStateMachine::RemoveState(Game& game)
{
	// If there is a state, 
	if(m_pCurrentState)
	{
		m_stateStack.push(m_pCurrentState->GetType()->GetName());

		m_pCurrentState->Destroy(game);
		game.GetPM().FreePlugin(DLLType::GamePlugin);

		// todo: need to mark some textures as being perminate between dlls
		game.GetRenderer().GetResourceManager().RemoveAllTextures();
	}
}

void GameStateMachine::ClearAllStates(Game& game)
{
	/*for(auto iter = m_states.begin(); iter != m_states.end(); ++iter)
	{
		iter->second->Destroy(game);
		delete iter->second;
	}

	m_states.clear();

	m_pCurrentState = nullptr;*/
}

void GameStateMachine::LoadPreviousState(Game& game)
{
	/*if(!m_stateStack.empty())
	{
		GameStateFactory& gsf = GameStateFactory::Instance();

		if(m_pCurrentState->GetType()->GetName() != m_stateStack.top())
		{
			SetState(m_stateStack.top(),game);
			m_stateStack.pop();
		}
	}*/
}
