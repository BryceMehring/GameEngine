//#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Game.h"
#include "FileManager.h"

#pragma comment(lib,"Game Engine.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	unsigned int seed = FileManager::Instance().GetSeedFromLog();
	// seed the random number generator 
	srand (seed);

	// Write the seed to the log
	char buffer[64];
	sprintf_s(buffer,"The random number generator seed is: %u",seed);
	FileManager::Instance().WriteToLog(buffer);

	// start the game
	Game myGame(hInstance);
	myGame.SetNextState("GameOfLifeMenu");
	return myGame.PlayGame();
}
