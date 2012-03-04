#ifndef _GAMESTATE_
#define _GAMESTATE_
#include <ctime>

class IGameState
{
public:

	friend class GameStateMachine;

	IGameState() { m_time = clock(); }
	virtual ~IGameState() {}

	virtual void Init(class Game* pGame) = 0;
	virtual void Destroy(class Game* pGame) = 0;
	virtual void Update(class Game* pGame) = 0;
	virtual void Draw(class Game* pGame) = 0;

	int GetStateId() { return m_iStateId; }

protected:

	int m_iStateId;
	clock_t m_time; // time in ms since 

};

#endif // _GAMESTATE_