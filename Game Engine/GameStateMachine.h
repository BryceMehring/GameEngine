#ifndef _GAMESTATEMACHINE_
#define _GAMESTATEMACHINE_

#include "GameState.h"

class GameStateMachine
{
public:

	GameStateMachine() : m_pState(nullptr) {}
	void SetState(IGameState* pState, class Game* pGame, int stateId);
	void RemoveState(class Game* pGame);

	IGameState* GetState() { return m_pState; }

private:

	IGameState* m_pState;

};

#endif // _GAMESTATEMACHINE_