//#include "StdAfx.h"
#include "Game.h"
#include "boost\bind.hpp"
#include <functional>

#pragma comment(lib,"Game Engine.lib")



void MyQuit()
{
	PostQuitMessage(0);
}

struct foo
{
	void doo() const
	{
		system("pause");
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	foo a;
	const foo* pA = &a;
	

	Delegate<void,void> callback(pA,&foo::doo);
	//callback.Bind(pA,&foo::doo);
	//callback.Bind(pA,&foo::doo);

	Game myGame(hInstance);
	myGame.SetNextState("GameOfLifeMenu");
	return myGame.PlayGame();
}
