#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"

class IGameState
{
public:

	friend class GameStateMachine;

	IGameState() { m_timer.Start(); }
	virtual ~IGameState() {}

	virtual void Init(class Game* pGame) = 0;
	virtual void Destroy(class Game* pGame) = 0;
	virtual void Update(class Game* pGame) = 0;
	virtual void Draw(class Game* pGame) = 0;

	int GetStateId() { return m_iStateId; }

protected:

	int m_iStateId;

	Timer m_timer;

};

#endif // _GAMESTATE_