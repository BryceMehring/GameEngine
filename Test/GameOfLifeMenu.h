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
	
	virtual void Init(class Game&);
	virtual void Destroy(class Game&);
	virtual void Update(class Game&);
	virtual void Draw(class Game&);

private:

	GUI m_gui;

	void BuildResolutionMenu(class Game&, Menu* pMenu);
	void BuildQuitButton(class Game&, Menu* pMenu);
	void BuildTextBoxButton(class Game&, Menu* pMenu);
	void BuildPluginViewMenu(class Game&, Menu* pMenu);

};

#endif