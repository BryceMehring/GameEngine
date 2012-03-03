#include "GameOfLife.h"

GameOfLife::GameOfLife(HINSTANCE hInstance) : Game(hInstance)
{
}
GameOfLife::~GameOfLife()
{
}

int GameOfLife::PlayGame()
{
	PluginManager& pm = ::PluginManager::Instance();
	IKMInput* pInput = static_cast<IKMInput*>(pm.GetPlugin(Input));

	while(pInput->MouseClick(0) == false)
	{
		pInput->Poll();
		m_window.Update();
	}

	return 0;
}