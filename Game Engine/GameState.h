#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"

class IGameState
{
public:

	IGameState();
	virtual ~IGameState() {}

	virtual void Init(class Game* pGame) = 0;
	virtual void Destroy(class Game* pGame) = 0;
	virtual void Update(class Game* pGame) = 0;
	virtual void Draw(class Game* pGame) = 0;
	virtual const class RTTI* GetType() const = 0;

protected:

	Timer m_timer;

};

#endif // _GAMESTATE_