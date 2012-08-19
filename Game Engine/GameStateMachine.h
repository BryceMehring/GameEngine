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
	~GameStateMachine() { delete m_pState; }

	// Removes current state if there is one
	// And then initializes the new state
	void SetState(IGameState* pState, class Game&);

	// Adds the current state string id to the stack
	// Destroy the current state
	void RemoveState(class Game&);

	// Sets the last state that was called with RemoveState()
	void LoadPreviousState(class Game&);

	bool HasState() const { return m_pState != nullptr; }

	IGameState& GetState() { return *m_pState; }
	const IGameState& GetState() const { return *m_pState; }

private:

	IGameState* m_pState;

	std::stack<std::string> m_states;

};

#endif // _GAMESTATEMACHINE_