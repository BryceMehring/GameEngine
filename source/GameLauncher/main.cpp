#include "Game.h"
#include "Log.h"

#include <string>

int main(int size, char** cmd)
{
	const char* pState = "PluginLoader";

	if (size >= 2)
	{
		pState = cmd[1];
	}

	Game myGame;
	myGame.SetNextState(pState);
	return myGame.Run();

}
