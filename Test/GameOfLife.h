#ifndef _GAMEOFLIFE_
#define _GAMEOFLIFE_

#include "Game.h"

class GameOfLife : public Game
{
public:

	GameOfLife(HINSTANCE hInstance);
	virtual ~GameOfLife();

	virtual int PlayGame();

private:

};

#endif _GAMEOFLIFE_