#ifndef _GAMESTATE_
#define _GAMESTATE_

#include "IPlugin.h"

// Base Game plugin
class IGameState : public IPlugin
{
public:

	// Called only once when the plugin is created
	virtual void Init(class Game& game) = 0;

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game) = 0;

	// Called every frame to update the date of the game
	virtual void Update(class Game& game) = 0;

	// Called every frame to render the game
	virtual void Draw(class Game& game) = 0;

protected:

	virtual ~IGameState() {}

};

#endif // _GAMESTATE_
