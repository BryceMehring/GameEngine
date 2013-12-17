
#include "Game.h"
#include "Log.h"
#include "ResourceFileLoader.h"
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

Game::Game() : m_fDT(0.0f), m_pRenderer(nullptr), m_pInput(nullptr), m_bDrawFPS(true)
{
	m_plugins.SetAS(m_vm.GetScriptEngine());
	LoadPlugins();

	LoadResourceFile("base.r",*this);
	//m_pInput->LoadKeyBindFile("bind.txt");

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

int Game::Run()
{

	// Loop while the use has not quit
	while(!m_pInput->KeyPress(GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(glfwGetCurrentContext()))
	{
		glfwSetTime(0.0);

		m_pInput->Poll();

		// Update the game
		Update();

		// Render the game
		Draw();

		m_fDT = glfwGetTime();
	}

	return 0;
}

void Game::Update()
{
	static bool bSync = false;
	m_info.Update(m_fDT);

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


	m_StateMachine.GetState().Update(*this);

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
	stream << "FPS: " << m_info.GetFPS();
	m_pRenderer->GetDisplayMode(width,height);

	m_pRenderer->SetRenderSpace(RenderSpace::Screen);

	m_pRenderer->DrawString(stream.str().c_str(),glm::vec3(0.0f,height,-10.0f),0.5f);
}

IRenderer& Game::GetRenderer()
{
	return *m_pRenderer;
}
IKMInput& Game::GetInput()
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
unsigned int Game::GetFps() const
{
	return m_info.GetFPS();
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

	m_pInput = static_cast<IKMInput*>(pPlugin);
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

GameInfo::GameInfo() : m_fTimeElapsed(0.0), m_uiFrames(0), m_uiFPS(0)
{

}

unsigned int GameInfo::GetFPS() const
{
	return m_uiFPS;
}

void GameInfo::Update(double dt)
{
	UpdateFPS(dt);
}

void GameInfo::UpdateFPS(double dt)
{
	static double tim1 = 0;
	static double tim2 = 0;
	static double diff = 0;

	m_fTimeElapsed += dt;
	++m_uiFrames;

	if ( m_uiFrames % 61 == 1 )
	{
		tim1 = m_fTimeElapsed;
	}
	else if(m_uiFrames % 61 == 0)
	{
		tim1 = tim2;
		tim2 = m_fTimeElapsed;
		diff = abs(tim1 - tim2);
	}

	m_uiFPS = (unsigned int)((61 / diff)+0.5f);
}
