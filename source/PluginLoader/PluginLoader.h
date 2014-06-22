#ifndef _PLUGINLOADER_
#define _PLUGINLOADER_

#include "IGameState.h"
#include "GUI.h"
#include "Button.h"

class PluginLoader : public IGameState
{
public:

	PluginLoader();

	// IPlugin

	// returns the type of the plug-in
	virtual DLLType GetPluginType() const { return DLLType::Game; }

	// returns the name of the plug-in
	virtual const char* GetName() const { return "PluginLoader"; }

	// returns the version number of the plugin
	virtual int GetVersion() const { return 0; }

	// IGameState

	// Called only once when the plugin is created
	virtual void Init(class Game& game);

	// Called only once when the plugin is destroyed
	virtual void Destroy(class Game& game);

	// Called every frame to update the state of the game
	virtual void Update(class Game& game);

	// Called every frame to render the game
	virtual void Draw(class Game& game);

	void ButtonCallback(UI::Button& button);

private:

	void LoadDirectories(std::vector<std::string>& dirList) const;

	UI::GUI m_gui;
	UI::GUI::HANDLE m_rootNode;
	std::string m_buttonPressed;
};

#endif // _PLUGINLOADER_
