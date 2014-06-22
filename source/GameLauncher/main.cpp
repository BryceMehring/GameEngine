#include "Game.h"

#include <iostream>
#include <ctime>

using namespace std;

int main(int size, char** cmd)
{
	const char* pState = "PluginLoader";

	if (size >= 2)
	{
		pState = cmd[1];
	}

	// seed the random number generator
	srand ((unsigned int)time(0));

	Game myGame;
	myGame.SetNextState(pState);
	return myGame.Run();
}
