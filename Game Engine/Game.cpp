
#include "Game.h"
#include "FileManager.h"
#include "Heap.h"
#include "CreatorId.h"
#include "RTTI.h"
#include "Menu.h"
#include <string>
#include "gassert.h"
#include <ctime>
//#include "MenuCreator.h"

using namespace std;

Game::Game(HINSTANCE hInstance) : m_window(hInstance), m_fDT(0.0f), 
m_pRenderer(nullptr), m_pInput(nullptr), m_pNextState(nullptr), m_bConsoleEnabled(false)
{
	LoadAllDLL();

	RECT R = {50,50,750,550};
	//GetWindowRect(m_window.GetWindowHandle(),&R);
	m_pConsole = new ScriptingConsole(&m_vm,"Scripting Console",R);

	m_vm.SetTextBox(m_pConsole);
	m_vm.RegisterScript();

	m_pConsole->RegisterScript();

	RegisterScript();

	m_iEventId = m_window.AddMsgListener(WindowManager::MsgDelegate(this,&Game::MsgProc));

	//m_timer.Start();
}

Game::~Game()
{
	delete m_pNextState;
	delete m_pConsole;

	// run the garbage collector before we destroy
	asIScriptEngine* pEngine = m_vm.GetScriptEngine();
	pEngine->GarbageCollect();
	pEngine->Release();

	// destroy the current state
	m_StateMachine.GetState()->Destroy(this);

	// remove the listener from the window
	m_window.RemoveListener(m_iEventId);
}

const std::string& Game::GetCurrentState() const
{
	return m_StateMachine.GetState()->GetType()->GetName();
}

void Game::SetNextState(const std::string& name)
{
	// todo: fix this so that 
	if(m_pNextState == nullptr)
	{
		// get the current state
		IGameState* pState = m_StateMachine.GetState();

		// If the current state is null, or if the new state is different than the current
		if((pState == nullptr) || (pState->GetType()->GetName() != name))
		{
			
			// Create new state, but do not use it until we are out of the update/render phase
			m_pNextState = GameStateFactory::Instance().CreateState(name);
			
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
	if(m_pNextState != nullptr)
	{
		// switch states
		m_StateMachine.SetState(m_pNextState,this);

		//m_window.SetWinCaption(m_pNextState->GetType()->GetName());

		// Reset next state
		m_pNextState = nullptr;
	}
	else if(!m_bConsoleEnabled && m_pInput->KeyDown(B))
	{
		m_StateMachine.LoadPreviousState(this);
	}


	if(m_pInput->GetKeyDown() == 96)
	{
		m_bConsoleEnabled = !m_bConsoleEnabled;
		m_pInput->Reset();
	}

	if(m_bConsoleEnabled)
	{
		m_pConsole->Update(m_pInput,m_fDT);

		// update garbage collection
		////asIScriptEngine* pEngine = m_vm.GetScriptEngine();
		//DBAS(pEngine->GarbageCollect(asGC_ONE_STEP));
		//pEngine->Release();
	}
	else
	{
		m_StateMachine.GetState()->Update(this);
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
		m_pConsole->Render(m_pRenderer);
	}
	else
	{
		// render the current state
		m_StateMachine.GetState()->Draw(this);
	}

	// end rendering
	m_pRenderer->End();

	//Sleep(2);

	// Present the screen
	m_pRenderer->Present();
}

void Game::DrawFPS()
{
	static char buffer[128];
	static int i = 0;
	POINT P = {700,0};

	::std::ostringstream out;
	out<<"FPS: " << GetFps() << endl << "Dt: " << GetDt();
	//sprintf_s(buffer,"FPS: %f\nMemory Usage:%f",GetFps(),Heap::Instance().GetMemoryUsageInKb());
	
	m_pRenderer->DrawString(out.str().c_str(),P,0xffffffff);
}

IRenderer* Game::GetRenderer()
{
	return m_pRenderer;
}
IKMInput* Game::GetInput()
{
	return m_pInput;
}
WindowManager* Game::GetWindow()
{
	return &m_window;
}
asVM* Game::GetAs()
{
	return &m_vm;
}
const PluginManager* Game::GetPluginManager() const
{
	return &m_plugins;
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
	ReloadPlugins("..\\Debug\\");
}

void Game::ReloadPlugins(const std::string& file)
{
	m_plugins.FreeAllPlugins();

	if(m_plugins.LoadAllPlugins(file,".dll"))
	{
		m_pRenderer = static_cast<IRenderer*>(m_plugins.GetPlugin(Rendering));
		gassert(m_pRenderer != nullptr,"No Rendering plugin was found");

		m_pInput = static_cast<IKMInput*>(m_plugins.GetPlugin(Input));
		gassert(m_pRenderer != nullptr,"No Input plugin was found");
	}
}

void Game::ReloadPluginsFromUserFolder()
{
	string folder;
	if(FileManager::Instance().GetFolder(folder))
	{
		ReloadPlugins(folder);
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
