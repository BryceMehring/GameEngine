
#include "Game.h"
#include "FileManager.h"
#include "Heap.h"
#include "RTTI.h"
#include "Menu.h"
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>

#define GLFW_NO_GLU
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


using namespace std;

Game::Game() : m_pConsole(nullptr), m_bConsoleEnabled(false),
 m_fDT(0.0f), m_fEscTime(0.0), m_pRenderer(nullptr), m_pInput(nullptr)
{
	
	// todo: reorganize this ctor

	// Initialize
	if(glfwInit() < 1)
	{
		throw string("Failed to init glfw");
	}

    m_plugins.SetAS(m_vm.GetScriptEngine());
    LoadAllDLL();
	
    //glfwDisable(GLFW_MOUSE_CURSOR);
    glfwDisable(GLFW_AUTO_POLL_EVENTS);

	// todo: move this code somewhere else
    m_pRenderer->GetResourceManager().LoadResourceFile("base.r");

	// todo: init the scripting console somewhere else
    //float width = 90;
    //float height = 90;
    //glm::vec2 pos(0.0f,0.0f);
	
    //m_pConsole = new ScriptingConsole(&m_vm,"Scripting Console",pos,width,height);

    //m_vm.RegisterScript(m_pConsole);

    //m_pConsole->RegisterScript();



	RegisterScript();
}

Game::~Game()
{
    m_StateMachine.RemoveState(*this);
	glfwTerminate();
}

const std::string& Game::GetCurrentState() const
{
	return m_StateMachine.GetState().GetType()->GetName();
}

void Game::SetNextState(const std::string& state)
{
	if(m_NextState.empty())
	{
		// If the current state is null, or if the new state is different than the current
		if(!m_StateMachine.HasState() || m_StateMachine.GetState().GetType()->GetName() != state)
		{
            m_NextState = state;
		}
	}
}

int Game::Run()
{
	// Loop while the use has not quit
	while(glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS && glfwGetWindowParam( GLFW_OPENED ))
	{
        glfwPollEvents();
		StartTimer();

		// Update the game
        Update();

        // Render the game
        Draw();

		m_pInput->Reset();

		EndTimer();
	}

	return 0;
}

void Game::Update()
{
	m_info.Update(m_fDT);

	// If There has been a state change, 
	if(!m_NextState.empty())
	{
		// switch states
		m_StateMachine.SetState(m_NextState,*this);

		// Reset next state
		m_NextState.clear();
	}

	/*if(m_pInput->KeyDown(KeyCode::F11))
	{
		m_pRenderer->ToggleFullscreen();
	}

	if(m_pInput->GetKeyDown() == KeyCode::TILDE)
	{
		m_bConsoleEnabled = !m_bConsoleEnabled;
		m_pInput->Reset();
	}

	if(m_pInput->KeyDown(KeyCode::ESCAPE,false))
	{
		m_fEscTime += m_fDT;

		if(m_fEscTime > 1.5)
		{
			// todo: fix this somehow
			//PostQuitMessage(0);
		}
	}
	else
	{
		m_fEscTime = 0.0;
	}*/

	if(m_bConsoleEnabled)
	{
		//m_pConsole->Update(*m_pInput,m_fDT);
	}
	else
	{
       m_StateMachine.GetState().Update(*this);
	}
}

void Game::Draw()
{
    //DrawFPS();
    DrawSelectionRect();
    DrawCursor();
		
	if(m_bConsoleEnabled)
	{
        //m_pConsole->Render(*m_pRenderer);
	}
	else
	{
		// render the current state
        m_StateMachine.GetState().Draw(*this);
	}


	// Present the screen
	m_pRenderer->Present();
}

void Game::DrawFPS()
{
	std::ostringstream out;
	out<<"FPS: " << GetFps();

	m_pRenderer->DrawString(out.str().c_str(),::glm::vec2(-49,49));
}

void Game::DrawCursor()
{
	const ::glm::vec2& pos = m_pInput->GetTransformedMousePos();

	glm::mat4 T = glm::translate(pos.x,pos.y,0.0f);
	glm::mat4 S = glm::scale(6.0f,6.0f,1.0f);

    m_pRenderer->DrawSprite(S*T,"industry");
}

void Game::DrawSelectionRect()
{
	Math::AABB R;
	if(m_pInput->GetSelectedRect(R))
	{
		glm::vec3 pos[] =
		{
			glm::vec3(R.min.x,R.min.y,0.0f),
			glm::vec3(R.min.x,R.max.y,0.0f),
			glm::vec3(R.max.x,R.max.y,0.0f),
			glm::vec3(R.max.x,R.min.y,0.0f),
			glm::vec3(R.min.x,R.min.y,0.0f)
		};

		glm::mat4 I(1.0f);
		glm::vec4 color(0.0f,1.0f,0.0f,1.0f);

		m_pRenderer->DrawLine(pos,sizeof(pos) / sizeof(glm::vec3));
	}
}

IRenderer& Game::GetRenderer()
{
	return (*m_pRenderer);
}
IKMInput& Game::GetInput()
{
	return (*m_pInput);
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

void Game::ReloadPlugins()
{
/*#ifdef _DEBUG
	ReloadPlugins("..\\Debug\\");
#else*/
    ReloadPlugins("/");
//#endif
}

void Game::ReloadPlugins(const std::string& path)
{
    m_plugins.FreeAllPlugins();

    string renderer = "./";
    string input = renderer;

#ifdef _WIN32
    renderer += "renderer";
    input += "input";
#else
    renderer += "librenderer";
    input += "libinput";
#endif


    IPlugin* pPlugin = m_plugins.LoadDLL(renderer);
	assert(pPlugin != nullptr);
	assert(pPlugin->GetPluginType() == DLLType::RenderingPlugin); // check to make sure the renderer is actually the renderer

    m_pRenderer = static_cast<IRenderer*>(pPlugin);

    pPlugin = m_plugins.LoadDLL(input);
	assert(pPlugin != nullptr);
	assert(pPlugin->GetPluginType() == DLLType::InputPlugin); // check to make sure the input is actually the input plugin

    m_pInput = static_cast<IKMInput*>(pPlugin);
}

void Game::LoadAllDLL()
{
	ReloadPlugins();
}

void Game::StartTimer()
{
	m_timer.Reset();
	m_timer.Start();
}

void Game::EndTimer()
{
	m_timer.Stop();
	m_fDT =  m_timer.GetTime();
}

void Game::RegisterScript()
{
	auto pEngine = m_vm.GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("Game",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("Game","void SetNextState(const string& in)",asMETHOD(Game,SetNextState),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("Game","const string& GetCurrentState() const",asMETHOD(Game,GetCurrentState),asCALL_THISCALL));
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
	m_fTimeElapsed += dt;
	m_uiFrames += 1;

	if(m_fTimeElapsed >= 1.0)
	{
		m_uiFPS = m_uiFrames;
		m_uiFrames = 0;
		m_fTimeElapsed = 0.0;
	}
}
