// Programmed by Bryce Mehring

#ifndef _GAMESTATEMACHINE_
#define _GAMESTATEMACHINE_

#include "GameState.h"
#include <stack>


// This class manages setting and removing the current state
class GameStateMachine
{
public:

	GameStateMachine() : m_pState(nullptr) {}

	// Removes current state if there is one
	// And then initializes the new state
	void SetState(IGameState* pState, class Game* pGame);

	// Adds the current state string id to the stack
	// Destroy the current state
	void RemoveState(class Game* pGame);

	// Sets the last state that was called with RemoveState()
	void LoadPreviousState(class Game* pGame);

	IGameState* GetState() { return m_pState; }

private:

	IGameState* m_pState;

	std::stack<std::string> m_states;

};

#endif // _GAMESTATEMACHINE_