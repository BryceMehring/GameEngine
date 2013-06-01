#ifndef _GAMEOFLIFEMENU_
#define _GAMEOFLIFEMENU_

#define PLUGIN_EXPORTS

#include "GameState.h"
#include "Menu.h"

class GameOfLifeMenu : public IGameState
{
public:

	RTTI_DECL;

	GameOfLifeMenu(PluginManager&);
	virtual ~GameOfLifeMenu() {}

	virtual DLLType GetPluginType() const { return DLLType::GamePlugin; }
	virtual const char* GetName() const { return "GameOfLifeMenu"; }
	virtual int GetVersion() const { return 0; }

	// displays a info box about the plug-in
	virtual void About() const {}
	
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