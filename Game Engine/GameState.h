#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"

class IGameState
{
public:

	enum State
	{
		NOTHING,
		// updating
		UPDATING,
		RENDERING,
		
	};

	friend class GameStateMachine;

	IGameState() : m_state(NOTHING), m_iStateId(0) { m_timer.Start(); }
	virtual ~IGameState() {}

	virtual void Init(class Game* pGame) = 0;
	virtual void Destroy(class Game* pGame) = 0;
	virtual void Update(class Game* pGame) = 0;
	virtual void Draw(class Game* pGame) = 0;
	//virtual void OnLostDevice() = 0;
	//virtual void OnResetDevice() = 0;

	int GetStateId() const { return m_iStateId; }
	State GetState() const { return m_state; }

protected:

	int m_iStateId;
	State m_state;
	Timer m_timer;

};

#endif // _GAMESTATE_