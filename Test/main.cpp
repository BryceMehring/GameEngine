//#include "StdAfx.h"
#include "Game.h"
#include <ctime>

#pragma comment(lib,"Game Engine.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	srand((unsigned int)time(0));

	Game myGame(hInstance);
	myGame.SetNextState("GameOfLifeMenu");
	return myGame.PlayGame();
}
