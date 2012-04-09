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
	virtual ~Game();

	// Set state
	void SetStateFactory(GameStateFactory* pFactory);
	void SetState(int id);

	// Get Functions
	IRenderer* GetRenderer();
	IKMInput* GetInput();
	WindowManager* GetWindow();
	float GetDt() const;

	virtual int PlayGame();

protected:

	// Data members
	WindowManager m_window;
	PluginManager m_plugins;

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
	int m_iEventId;

	// helper functions
	void LoadAllDLL();

	void MsgProc(const struct MsgProcData& data);

	// Prevent copying
	Game(const Game&);
	Game& operator =(const Game&);

};

#endif _GAME_