#ifndef _GAMEOFLIFEMENU_
#define _GAMEOFLIFEMENU_

#include "GameState.h"
#include "Menu.h"

class GameOfLifeMenu : public IGameState
{
public:

	virtual ~GameOfLifeMenu() {}
	virtual void Init(class Game* pGame);
	virtual void Destroy(class Game* pGame);
	virtual void Update(class Game* pGame);
	virtual void Draw(class Game* pGame);

private:

	UI* m_pUI;

};

#endif