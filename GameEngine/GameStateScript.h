#pragma once

#include "GameState.h"

#include <string>

class GameStateScript : public IGameState
{
public:

	virtual void Init(class Game& game);
	virtual void Destroy(class Game& game);

private:

	// helper functions
	std::string GetPathName(const std::string& file) const;
	void Exec(class Game& game, const std::string& file);

};

