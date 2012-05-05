
#include "Game.h"
#include "FileManager.h"
#include "Heap.h"
#include "CreatorId.h"
#include "RTTI.h"
//#include "MenuCreator.h"

using namespace std;

Game::Game(HINSTANCE hInstance) : m_window(hInstance,"Game"), m_fDT(0.0f), 
m_pRenderer(nullptr), m_pInput(nullptr), m_pNextState(nullptr)
{
	LoadAllDLL();

	m_iEventId = m_window.AddMsgListener(WindowManager::MsgDelegate(this,&Game::MsgProc));

	//m_timer.Start();
}

Game::~Game()
{
	m_StateMachine.RemoveState(this);
	m_window.RemoveListener(m_iEventId);
}

void Game::SetNextState(const std::string& name)
{
	// get the current state
	IGameState* pState = m_StateMachine.GetState();

	// If the current state is null, or if the new state is different than the current
	if((pState == nullptr) || (pState->GetType()->GetName() != name))
	{
		// If this is the first SetNextState call in this update loop
		if(m_pNextState == nullptr)
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
	while(StartTimer(),(m_window.Update()))
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

		// Update change to log
		char buffer[64];
		sprintf_s(buffer,"Changing state to: %s",m_pNextState->GetType()->GetName().c_str());
		FileManager::Instance().WriteToLog(buffer);

		// Reset next state
		m_pNextState = nullptr;
	}
	else if(m_pInput->KeyDown(B))
	{
		m_StateMachine.LoadPreviousState(this);
	}

	m_StateMachine.GetState()->Update(this);
}

void Game::Draw()
{
	// tell the renderer that we are starting
	m_pRenderer->Begin();

	// render fps overlay
	DrawFPS();

	// render the current state
	m_StateMachine.GetState()->Draw(this);

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
	out<<"FPS: " << GetFps() << endl << "Dt: " << GetDt() << endl;
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
		m_pInput = static_cast<IKMInput*>(m_plugins.GetPlugin(Input));
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
	m_plugins.SetEngine(&m_window);
	ReloadPlugins();
}

void Game::StartTimer()
{
	m_timer.Start();
}

void Game::EndTimer()
{
	m_timer.Stop();
	m_fDT =  m_timer.GetTime();
}

void Game::MsgProc(const MsgProcData& data)
{
	switch(data.msg)
	{
	case WM_ACTIVATE:
		if( LOWORD(data.wParam) == WA_INACTIVE )
		{
			m_pRenderer->OnLostDevice();
		}
		else
		{
			m_pRenderer->OnResetDevice();
		}
		break;
	}
}

GameInfo::GameInfo() : m_fTimeElapsed(0.0f), m_uiFrames(0), m_uiFPS(0)
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

	if(m_fTimeElapsed >= 1.0f)
	{
		m_uiFPS = m_uiFrames;
		m_uiFrames = 0;
		m_fTimeElapsed = 0.0f;
	}
}
