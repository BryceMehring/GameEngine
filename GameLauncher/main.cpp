#include "Game.h"

#include <iostream>
#include <ctime>

using namespace std;

int main(int size, char** cmd)
{
	if(size < 2)
	{
		cout<<"Invalid Number of command line arguments"<<endl;
		return 1;
	}

	const char* pState = cmd[1];

	// seed the random number generator
	srand ((unsigned int)time(0));

	Game myGame;
	//myGame.SetNextState(pState);
	//return myGame.Run();
	return 0;
}
