
#include "SingleInstance.h"
#include "FileManager.h"
#include "Game.h"

struct GameThreadInfo
{
	HINSTANCE h;
	std::string nextState;
};

DWORD WINAPI GameThread(void* p)
{
	GameThreadInfo* pInfo = (GameThreadInfo*)p;

	Game myGame(pInfo->h);
	myGame.SetNextState(pInfo->nextState);
	return myGame.PlayGame();
}

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
	std::ostringstream stream;
	stream << "The random number generator seed is: "<< seed << std::endl;
	FileManager::Instance().WriteToLog(stream);

	// start the game
	GameThreadInfo info = {h,pState};
	HANDLE hGame = CreateThread(0,0,GameThread,&info,0,0);

	// This thread should pump the msgs from the queue, and once the 

	WaitForSingleObject(hGame,INFINITE);

	CloseHandle(hGame);

	return 0;
}