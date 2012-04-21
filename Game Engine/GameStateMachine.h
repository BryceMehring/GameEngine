#ifndef _GAMESTATEMACHINE_
#define _GAMESTATEMACHINE_

#include "GameState.h"
#include <stack>

class GameStateMachine
{
public:

	~GameStateMachine();

	//void SetState(IGameState* pState, class Game* pGame, int stateId);
	//void RemoveState(class Game* pGame);

	IGameState* GetState();
	void PushState(IGameState* pState, Game* pGame, int stateId);
	void Push();
	void Pop();

private:

	typedef std::stack<IGameState*> StackType;

	StackType m_States;
	StackType m_StatesPoped;

	void DestroyStack(StackType& stack);

};

#endif // _GAMESTATEMACHINE_