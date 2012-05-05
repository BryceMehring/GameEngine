#ifndef _GAMEOFLIFEMENU_
#define _GAMEOFLIFEMENU_

#include "GameState.h"
#include "Menu.h"

class GameOfLifeMenu : public IGameState
{
public:

	RTTI_DECL;

	GameOfLifeMenu();
	virtual ~GameOfLifeMenu() {}
	
	virtual void Init(class Game*);
	virtual void Destroy(class Game*);
	virtual void Update(class Game*);
	virtual void Draw(class Game*);

private:

	GUI m_gui;

	void BuildResolutionMenu(class Game* pGame, Menu* pMenu);
	void BuildQuitButton(class Game* pGame, Menu* pMenu);

};

#endif