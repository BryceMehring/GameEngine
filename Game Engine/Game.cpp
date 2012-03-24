#include "Game.h"

Game::Game(HINSTANCE hInstance) : m_window(hInstance,"Game"), m_fDT(0.0f)
{
	LoadAllDLL();

	VertexDeclaration decl;

	//UINT iVertexDecl = m_pRenderer->CreateVertexDecl(decl);
}

Game::~Game()
{
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
	while(m_window.Update())
	{
		m_pInput->Poll();

		m_StateMachine.GetState()->Update(this);
		m_StateMachine.GetState()->Draw(this);
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


void Game::LoadAllDLL()
{
	m_plugins.SetEngine(&m_window);
	if(m_plugins.LoadAllPlugins("..\\Debug\\",".dll"))
	{
		assert(m_pRenderer = static_cast<IRenderer*>(m_plugins.GetPlugin(Rendering)));
		assert(m_pInput = static_cast<IKMInput*>(m_plugins.GetPlugin(Input)));
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
	m_fDT = (float)m_timer.GetTimeInSeconds();
}