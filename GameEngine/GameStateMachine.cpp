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
}

void GameStateMachine::SetState(const std::string& state, Game& game)
{
    string path = "/home/bryce/GameEngine/lib" + state + ".so";
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
    if(m_pCurrentState != nullptr)
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
