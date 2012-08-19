#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"

class IGameState
{
public:

	IGameState();
	virtual ~IGameState() {}

	virtual void Init(class Game& game) = 0;
	virtual void Destroy(class Game& game) = 0;
	virtual void Update(class Game& game) = 0;
	virtual void Draw(class Game& game) = 0;
	virtual const class RTTI* GetType() const = 0;

protected:

	Timer m_timer;

};

#endif // _GAMESTATE_