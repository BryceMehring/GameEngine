#ifndef _GAMESTATESCRIPT_
#define _GAMESTATESCRIPT_

#include "IGameState.h"

#include <string>

// GameStateScript loads angelscript scipts during the loading and destroying of each state
class GameStateScript : public IGameState
{
public:

	// Loads init script into AngelSctipt(init.as)
	virtual void Init(class Game& game);

	// Loads destroy script into AngelScript(destroy.as)
	virtual void Destroy(class Game& game);

private:

	// helper functions
	std::string GetPathName(const std::string& file) const;
	void Exec(class Game& game, const std::string& file);

};

#endif // _GAMESTATESCRIPT_

