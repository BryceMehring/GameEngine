
#include "SingleInstance.h"
#include "FileManager.h"
#include "Game.h"

SingleInstance::SingleInstance() : m_handle(CreateEvent(NULL, TRUE, FALSE, "BEngine"))
{

}

SingleInstance::~SingleInstance()
{
	CloseHandle(m_handle);
}

int SingleInstance::Run(HINSTANCE h,const char* pState)
{
	unsigned int uiSuccess = 1;

	switch (GetLastError())
	{
		// app is already running
		case ERROR_ALREADY_EXISTS:
		{
			Error();
			break;
		}

		// this is the first instance of the app
		case ERROR_SUCCESS:
		{
			uiSuccess = RunProgram(h,pState);
			break;
		}
	}

	return uiSuccess;
}

void SingleInstance::Error()
{
	MessageBox(0,"Only one instance of the program can run",0,0);
}

int SingleInstance::RunProgram(HINSTANCE h,const char* pState)
{
	// global event created and new instance of app is running,
	// continue on, don't forget to clean up m_hEvent on exit

	unsigned int seed = FileManager::Instance().GetSeedFromLog();

	// seed the random number generator 
	srand (seed);

	// Write the seed to the log
	char buffer[64];
	sprintf_s(buffer,"The random number generator seed is: %u",seed);
	FileManager::Instance().WriteToLog(buffer);

	// start the game
	Game myGame(h);
	myGame.SetNextState(pState);
	return myGame.PlayGame();
}