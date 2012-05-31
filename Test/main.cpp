//#include "StdAfx.h"
#include "Game.h"

#pragma comment(lib,"Game Engine.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Game myGame(hInstance);
	myGame.SetNextState("GameOfLife");
	return myGame.PlayGame();
}
