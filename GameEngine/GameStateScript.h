#ifndef _GAMESTATESCRIPT_
#define _GAMESTATESCRIPT_

// todo: this code does not work with OpenGL and GLFW

#include "IGameState.h"

#include <string>


class GameStateScript : public IGameState
{
public:

	// Loads init script into AngelSctipt
	virtual void Init(class Game& game);

	// Loads destroy script into AngelScript
	virtual void Destroy(class Game& game);

private:

	// helper functions
	std::string GetPathName(const std::string& file) const;
	void Exec(class Game& game, const std::string& file);

};

#endif // _GAMESTATESCRIPT_

