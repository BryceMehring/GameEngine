// Programmed by Bryce Mehring

#ifndef _GAMESTATEMACHINE_
#define _GAMESTATEMACHINE_

#include "interfaces/IGameState.h"

// Manages setting and removing the current state
class GameStateMachine
{
public:

	GameStateMachine();

	// Removes current state if there is one
	// And then initializes the new state
	void SetState(const std::string& state, class Game&);

	// Unloads the old state
	void RemoveState(class Game&);

	bool HasState() const { return m_pCurrentState != nullptr; }

	IGameState& GetState() { return *m_pCurrentState; }
	const IGameState& GetState() const { return *m_pCurrentState; }

private:

	IGameState* m_pCurrentState;

};

#endif // _GAMESTATEMACHINE_
