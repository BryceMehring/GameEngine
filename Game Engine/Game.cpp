
#include "Game.h"
#include "FileManager.h"
#include "Heap.h"
#include "RTTI.h"
#include "Menu.h"
#include <string>
#include "gassert.h"
#include <ctime>
#include <iomanip>
//#include "MenuCreator.h"

using namespace std;

Game::Game(HINSTANCE hInstance) : m_window(hInstance), m_fDT(0.0f), m_pRenderer(nullptr), m_pInput(nullptr), m_bConsoleEnabled(false)
{
	LoadAllDLL();

	float width = 90;
	float height = 90;
	D3DXVECTOR3 pos(0.0f,0.0f,0.0f);
	
	m_pConsole = new ScriptingConsole(&m_vm,"Scripting Console",pos,width,height);

	m_vm.RegisterScript(m_pConsole);

	m_pConsole->RegisterScript();
	m_window.RegisterScript(m_vm.GetScriptEngine());

	RegisterScript();

	m_iEventId = m_window.AddMsgListener(WindowManager::MsgDelegate(this,&Game::MsgProc));
}

Game::~Game()
{
	// destroy the current state
	//m_StateMachine.RemoveState(*this);

	/// remove the listener from the window
	m_window.RemoveListener(m_iEventId);

	delete m_pConsole;

	// run the garbage collector before we destroy
	asIScriptEngine* pEngine = m_vm.GetScriptEngine();
	pEngine->GarbageCollect();
	pEngine->Release();
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

int Game::PlayGame()
{
	//m_PrevTime = m_timer.GetTimeInSeconds();
	//QueryPerformanceCounter((LARGE_INTEGER*)&m_PrevTime);

	// Loop while the use has not quit
	while(StartTimer(),m_window.Update())
	{
		// Update the game
		Update();

		// Render the game
		Draw();

		m_pInput->Reset();

		// End timer 
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

		//m_window.SetWinCaption(m_pNextState->GetType()->GetName());

		// Reset next state
		m_NextState.clear();
	}
	else if(!m_bConsoleEnabled && m_pInput->KeyDown(B))
	{
		m_StateMachine.LoadPreviousState(*this);
	}


	if(m_pInput->GetKeyDown() == 96)
	{
		// todo: implement
		//m_StateMachine.SetState("ScriptConsole",*this);

		m_bConsoleEnabled = !m_bConsoleEnabled;
		m_pInput->Reset();
	}

	if(m_bConsoleEnabled)
	{
		m_pConsole->Update(*m_pInput,m_fDT);
	}
	else
	{
		m_StateMachine.GetState().Update(*this);
	}
}

void Game::Draw()
{
	// tell the renderer that we are starting
	m_pRenderer->Begin();

	// render fps overlay
	DrawFPS();

	if(m_bConsoleEnabled)
	{
		m_pConsole->Render(*m_pRenderer);
	}
	else
	{
		// render the current state
		m_StateMachine.GetState().Draw(*this);
	}

	// end rendering
	m_pRenderer->End();

	//Sleep(2);

	// Present the screen
	m_pRenderer->Present();
}

void Game::DrawFPS()
{
	static float x = 0;
	static float y = 0;

	RECT windowR;
	::GetWindowRect(this->m_window.GetWindowHandle(),&windowR);

	::std::ostringstream out;
	out<<"FPS: " << GetFps() << endl << "Dt: " << setprecision (4) << GetDt() << endl;
	out<<m_pInput->GetTransformedMousePos().x << " " << m_pInput->GetTransformedMousePos().y  << endl;

	//sprintf_s(buffer,"FPS: %f\nMemory Usage:%f",GetFps(),Heap::Instance().GetMemoryUsageInKb());


	//RECT stringR = {0,0,0,0};
	//this->m_pRenderer->GetStringRec(str.c_str(),stringR);

	POINT point = {windowR.right - windowR.left - 100,0};
	m_pRenderer->Get2DRenderer().DrawString(out.str().c_str(),point,0xffffffff);
}

IRenderer& Game::GetRenderer()
{
	return (*m_pRenderer);
}
IKMInput& Game::GetInput()
{
	return (*m_pInput);
}
WindowManager& Game::GetWindow()
{
	return m_window;
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
#ifdef DEBUG
	ReloadPlugins("..\\Debug\\");
#else
	ReloadPlugins("..\\Release\\");
#endif
}

void Game::ReloadPlugins(const std::string& file)
{
	m_plugins.FreeAllPlugins();

	if(m_plugins.LoadAllPlugins(file,".dll"))
	{
		m_pRenderer = static_cast<IRenderer*>(m_plugins.GetPlugin(RenderingPlugin));
		gassert(m_pRenderer != nullptr,"No Rendering plugin was found");

		m_pInput = static_cast<IKMInput*>(m_plugins.GetPlugin(InputPlugin));
		gassert(m_pRenderer != nullptr,"No Input plugin was found");
	}
}

void Game::LoadAllDLL()
{
	m_plugins.SetGame(this);
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

void Game::MsgProc(const MsgProcData& data)
{
	switch(data.msg)
	{
	case WM_ACTIVATE:
		if( LOWORD(data.wParam) == WA_INACTIVE )
		{
			m_timer.Stop();
			m_pRenderer->OnLostDevice();
		}
		else
		{
			m_pRenderer->OnResetDevice();
			m_timer.Start();
		}
		break;
	}
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
