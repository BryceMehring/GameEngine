
#include "Game.h"
#include "Log.h"
#include "ResourceFileLoader.h"
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

Game::Game() : m_fDT(0.0), m_fTimeElapsed(0.0), m_uiFrameCounter(0), m_uiFPS(0), m_pRenderer(nullptr), m_pInput(nullptr), m_bDrawFPS(true)
{
	m_plugins.SetAS(m_vm.GetScriptEngine());
	LoadPlugins();

	LoadResourceFile("base.r",*this);

	RegisterScript();
}

Game::~Game()
{
	Log::Instance().Write("Shutting Down");

	m_StateMachine.RemoveState(*this);
}

std::string Game::GetCurrentStateName() const
{
	return m_StateMachine.GetState().GetName();
}

void Game::SetNextState(const std::string& state)
{
	// the next state can only be set once per frame
	if(m_NextState.empty())
	{
		m_NextState = state;
	}
}

void Game::LoadPlugins()
{
	m_plugins.FreeAllPlugins();

	IPlugin* pPlugin = m_plugins.LoadDLL("renderer");
	assert(pPlugin != nullptr);
	assert(pPlugin->GetPluginType() == DLLType::Rendering); // check to make sure the renderer is actually the renderer

	m_pRenderer = static_cast<IRenderer*>(pPlugin);

	pPlugin = m_plugins.LoadDLL("input");
	assert(pPlugin != nullptr);
	assert(pPlugin->GetPluginType() == DLLType::Input); // check to make sure the input is actually the input plugin

	m_pInput = static_cast<IInput*>(pPlugin);
}

IRenderer& Game::GetRenderer()
{
	return *m_pRenderer;
}
IInput& Game::GetInput()
{
	return *m_pInput;
}
PluginManager& Game::GetPM()
{
	return m_plugins;
}
asVM& Game::GetAs()
{
	return m_vm;
}
double Game::GetDt() const
{
	return m_fDT;
}

int Game::Run()
{
	Timer theTimer;

	// Loop while the use has not quit
	while(!m_pInput->KeyPress(GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(glfwGetCurrentContext()))
	{
		theTimer.Start();

		m_pInput->Poll();

		// Update the game
		Update();

		// Render the game
		Draw();

		m_fDT = theTimer.GetTime();
	}

	return 0;
}

void Game::Update()
{
	static bool bSync = false;

	// If There has been a state change,
	if(!m_NextState.empty())
	{
		// switch states
		m_StateMachine.SetState(m_NextState,*this);

		// Reset next state
		m_NextState.clear();
	}

	if(m_pInput->KeyPress(GLFW_KEY_F5))
	{
		m_pRenderer->EnableVSync(bSync);
		bSync = !bSync;
	}

	if(m_pInput->KeyPress(GLFW_KEY_F6))
	{
		m_bDrawFPS = !m_bDrawFPS;
	}

	if (m_bDrawFPS)
	{
		UpdateFPS();
	}

	m_StateMachine.GetState().Update(*this);

}

void Game::UpdateFPS()
{
	m_fTimeElapsed += m_fDT;
	++m_uiFrameCounter;

	if (m_fTimeElapsed > 0.99)
	{
		m_uiFPS = m_uiFrameCounter;
		m_uiFrameCounter = 0;
		m_fTimeElapsed = 0;
	}
}

void Game::Draw()
{
	m_StateMachine.GetState().Draw(*this);

	if(m_bDrawFPS)
	{
		DrawFPS();
	}

	m_pRenderer->Present();
}

void Game::DrawFPS()
{
	int width;
	int height;

	std::stringstream stream;
	stream << "FPS: " << m_uiFPS;
	m_pRenderer->GetDisplayMode(width,height);

	m_pRenderer->SetRenderSpace(RenderSpace::Screen);

	m_pRenderer->DrawString(stream.str().c_str(),glm::vec3(0.0f,height,-10.0f),0.5f);
}

void Game::RegisterScript()
{
	auto pEngine = m_vm.GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("Game",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("Game","void SetNextState(const string& in)",asMETHOD(Game,SetNextState),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("Game","string GetCurrentStateName() const",asMETHOD(Game,GetCurrentStateName),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("Game game",(void*)this));

	pEngine->Release();
}
