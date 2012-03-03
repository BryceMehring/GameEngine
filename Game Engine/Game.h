#ifndef _GAME_
#define _GAME_

#include "WindowManager.h"
#include "Timer.h"

class Game
{
public:

	Game(HINSTANCE hInstance);
	virtual ~Game() {}

	virtual int PlayGame() = 0;

protected:

	// Data members
	WindowManager m_window;
	float m_fDT;

	// helper functions
	void StartTimer();
	void EndTimer();

private:

	Timer m_timer;

	// helper functions
	void LoadAllDLL();

	// Prevent copying
	Game(const Game&);
	Game& operator =(const Game&);

};

#endif _GAME_