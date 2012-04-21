#ifndef _GAMEOFLIFEMENU_
#define _GAMEOFLIFEMENU_

#include "GameState.h"
#include "Menu.h"

class GameOfLifeMenu : public IGameState
{
public:

	GameOfLifeMenu(class Game* pGame);
	virtual ~GameOfLifeMenu() {}
	
	virtual void Init();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

	virtual const char* GetName() const { return "GameOfLifeMenu"; }

private:

	UI m_ui;

	void BuildResolutionMenu(class Game* pGame, Menu* pMenu);

};

#endif