#ifndef _GAMESTATE_
#define _GAMESTATE_

#include "IPlugin.h"
#include "IGame.h"

// Base Game plugin
class IGameState : public IPlugin
{
public:

	// Called only once when the plugin is created
	virtual void Init(IGame& game) = 0;

	// Called only once when the plugin is destroyed
	virtual void Destroy(IGame& game) = 0;

	// Called every frame to update the date of the game
	virtual void Update(IGame& game) = 0;

	// Called every frame to render the game
	virtual void Draw(IGame& game) = 0;

	virtual const class RTTI* GetType() const = 0;

protected:

	virtual ~IGameState() {}

};

#endif // _GAMESTATE_
