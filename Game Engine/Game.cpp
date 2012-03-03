#include "Game.h"

Game::Game(HINSTANCE hInstance) : m_window(hInstance,"Game"), m_fDT(0.0f)
{
	LoadAllDLL();
}

void Game::LoadAllDLL()
{
	PluginManager& pm = ::PluginManager::Instance();
	pm.SetEngine(&m_window);
	pm.LoadAllPlugins("..\\Debug\\",".dll");
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