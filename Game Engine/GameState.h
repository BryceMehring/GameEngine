#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"

class IGameState
{
public:

	enum State
	{
		NOTHING,
		_DESTROYING,
		UPDATING,
		RENDERING,
		
	};

	friend class GameStateMachine;

	IGameState(class Game* pGame) : m_pGame(pGame), m_state(NOTHING), m_iStateId(0) { m_timer.Start(); }
	virtual ~IGameState() {}

	virtual void Init() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual const char* GetName() const = 0;
	//virtual void OnLostDevice() = 0;
	//virtual void OnResetDevice() = 0;

	int GetStateId() const { return m_iStateId; }
	State GetState() const { return m_state; }

protected:

	class Game* m_pGame;
	int m_iStateId;
	State m_state;
	Timer m_timer;

};

#endif // _GAMESTATE_