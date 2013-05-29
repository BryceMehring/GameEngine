#ifndef _GAMESTATE_
#define _GAMESTATE_
#include "Timer.h"
#include "IPlugin.h"


class IGameState : public IPlugin
{
public:

	IGameState() {}

	// Called only once when the plugin is created
	virtual void Init(class Game& game) = 0;

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game) = 0;

	// Called every frame to update the date of the game
	virtual void Update(class Game& game) = 0;

	// Called every frame to render the game
	virtual void Draw(class Game& game) = 0;

	virtual const class RTTI* GetType() const = 0;

protected:

	Timer m_timer; // keeps track of the time the game has been playing

	virtual ~IGameState() {}

};

#endif // _GAMESTATE_
