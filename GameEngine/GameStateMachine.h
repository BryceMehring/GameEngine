// Programmed by Bryce Mehring

#ifndef _GAMESTATEMACHINE_
#define _GAMESTATEMACHINE_

#include "GameState.h"
#include <stack>
#include <map>


// This class manages setting and removing the current state
class GameStateMachine
{
public:

	GameStateMachine();
	~GameStateMachine();

	// Removes current state if there is one
	// And then initializes the new state
	void SetState(const std::string& state, class Game&);

	//void AddState(const std::string& state);

	// Adds the current state string id to the stack
	// Temporarily stops the current state
	void RemoveState(class Game&);

	// Deletes all of the states(frees all memory)
	void ClearAllStates(class Game&);

	// Sets the last state that was called with RemoveState()
	void LoadPreviousState(class Game&);

	// todo: this is kind of bugged here
	bool HasState() const { return m_pCurrentState != nullptr; }

	IGameState& GetState() { return *m_pCurrentState; }
	const IGameState& GetState() const { return *m_pCurrentState; }

private:

	// todo: create a vector of states
	IGameState* m_pCurrentState;

	std::map<std::string,IGameState*> m_states; // a list of all the states
	std::stack<std::string> m_stateStack;

	std::map<std::string,IGameState*>::iterator RAddState(const std::string& state);

};

#endif // _GAMESTATEMACHINE_