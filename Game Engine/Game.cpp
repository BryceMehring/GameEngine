
#include "Game.h"
#include "FileManager.h"
#include "Heap.h"

using namespace std;

Game::Game(HINSTANCE hInstance) : m_window(hInstance,"Game"), m_fDT(0.0f), m_fFPSTime(10000.0f), 
m_pRenderer(nullptr), m_pInput(nullptr)
{
	LoadAllDLL();

	m_iEventId = m_window.AddMsgListener(WindowManager::MsgDelegate(this,&Game::MsgProc));

	VertexDeclaration decl;

	//UINT iVertexDecl = m_pRenderer->CreateVertexDecl(decl);
}

Game::~Game()
{
	m_window.RemoveListener(m_iEventId);
	delete m_pFactory;
}

void Game::SetStateFactory(GameStateFactory* pFactory)
{
	m_pFactory = pFactory;
}

void Game::SetState(int id)
{
	IGameState* pState = m_StateMachine.GetState();
	if(pState == nullptr || pState->GetStateId() != id)
	{
		IGameState* pState = m_pFactory->GetState(id,this);
		m_StateMachine.PushState(pState,this,id);
		//m_StateMachine.SetState(pState,this,id);

		char buffer[64];
		sprintf_s(buffer,"Changing state to: %s",pState->GetName());
		FileManager::Instance().WriteToLog(buffer);
	}
}

void Game::PopState()
{
	m_StateMachine.Pop();
}
void Game::PushState()
{
	m_StateMachine.Push();
}

int Game::PlayGame()
{
	// init the first state
	m_StateMachine.GetState()->Init();

	// Loop while the use has not quit
	while((StartTimer(),m_window.Update()))
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
	/*if(m_pInput->KeyDown(KeyCode::SPACE))
	{
		// Start
		PopState();
	}
	else*/ if(m_pInput->KeyDown(KeyCode::ESCAPE))
	{
		// Back
		PushState();
	}

	m_StateMachine.GetState()->Update();
}

void Game::Draw()
{
	// tell the renderer that we are starting
	m_pRenderer->Begin();

	// render fps overlay
	DrawFPS();

	// render the current state
	m_StateMachine.GetState()->Draw();

	// end rendering
	m_pRenderer->End();

	// Present the screen
	m_pRenderer->Present();
}

void Game::DrawFPS()
{
	static char buffer[64];
	POINT P = {650,0};

	m_fFPSTime += GetDt();

	if(m_fFPSTime > 300.0f)
	{
		m_fFPSTime = 0.0f;

		
		sprintf_s(buffer,"FPS: %f\nMemory Usage:%f",GetFps(),Heap::Instance().GetMemoryUsageInKb());
	}

	m_pRenderer->DrawString(buffer,P,0xffffffff);
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

float Game::GetDt() const
{
	return m_fDT;
}
float Game::GetFps() const
{
	return 1000.0f / GetDt();
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
	m_timer.Reset();
	m_timer.Start();
}

void Game::EndTimer()
{
	m_timer.Stop();
	m_fDT = (float)m_timer.GetTimeInMilliseconds();
}

void Game::MsgProc(const MsgProcData& data)
{
	switch(data.msg)
	{
	case WM_ACTIVATE:
		if( LOWORD(data.wParam) == WA_INACTIVE )
		{
			this->m_pRenderer->OnLostDevice();
		}
		else
		{
			this->m_pRenderer->OnResetDevice();
		}
		break;
	}
}