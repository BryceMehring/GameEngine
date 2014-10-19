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

	try
	{
		Game myGame;
		myGame.SetNextState(pState);
		return myGame.Run();
	}
	catch(std::exception& error)
	{
		Log::Instance().Write(error.what());
	}
	catch(std::string msg)
	{
		Log::Instance().Write(msg);
	}
	catch(...)
	{
		Log::Instance().Write("Unknown error, quitting.");
	}

	return 0;
}
