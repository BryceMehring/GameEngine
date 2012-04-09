#include "Game.h"

Game::Game(HINSTANCE hInstance) : m_window(hInstance,"Game"), m_fDT(0.0f)
{
	LoadAllDLL();

	m_iEventId = m_window.AddMsgListener(WindowManager::MsgDelegate(this,&Game::MsgProc));

	VertexDeclaration decl;

	//UINT iVertexDecl = m_pRenderer->CreateVertexDecl(decl);
}

Game::~Game()
{
	m_window.RemoveListener(m_iEventId);

	m_StateMachine.RemoveState(this);
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
		IGameState* pState = m_pFactory->GetState(id);
		m_StateMachine.SetState(pState,this,id);
	}
}

int Game::PlayGame()
{
	while((StartTimer(),m_window.Update()))
	{
		m_StateMachine.GetState()->Update(this);
		m_StateMachine.GetState()->Draw(this);

		m_pInput->Reset();

		EndTimer();
	}

	return 0;
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

void Game::LoadAllDLL()
{
	m_plugins.SetEngine(&m_window);
	if(m_plugins.LoadAllPlugins("..\\Debug\\",".dll"))
	{
		assert(m_pRenderer = static_cast<IRenderer*>(m_plugins.GetPlugin(Rendering)));
		assert(m_pInput = static_cast<IKMInput*>(m_plugins.GetPlugin(Input)));

		m_pRenderer->EnumerateDisplayAdaptors();
	}
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