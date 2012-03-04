#ifndef _GAME_
#define _GAME_

#include "WindowManager.h"
#include "Timer.h"
#include "GameStateFactory.h"
#include "GameStateMachine.h"
#include "IKMInput.h"
#include "IRenderer.h"

class Game
{
public:

	Game(HINSTANCE hInstance);
	virtual ~Game() {}

	void SetStateFactory(GameStateFactory* pFactory);
	void SetState(int id);

	IRenderer* GetRenderer();
	IKMInput* GetInput();
	WindowManager* GetWindow();


	virtual int PlayGame();

protected:

	// Data members
	WindowManager m_window;
	GameStateMachine m_StateMachine;
	GameStateFactory* m_pFactory;
	float m_fDT;

	// Interfaces
	IRenderer* m_pRenderer;
	IKMInput* m_pInput;

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