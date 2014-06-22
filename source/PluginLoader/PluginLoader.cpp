#include "PluginLoader.h"
#include "Game.h"
#include "GUIFactory.h"
#include <dirent.h>

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new PluginLoader();
}

PluginLoader::PluginLoader()
{

}

void PluginLoader::Init(Game& game)
{
	std::vector<std::string> dirList;
	LoadDirectories(dirList);

	m_rootNode = m_gui.CreateNode();
	m_gui.SetNode(m_rootNode);

	int width, height;
	game.GetRenderer().GetDisplayMode(&width, &height);
	glm::vec2 startingPos(width / 2.0f, height);
	for (const std::string& currentDir : dirList)
	{
		using namespace std::placeholders;

		auto pButton = UI::GUIFactory<UI::Button>::CreateElement(game,
			startingPos,
			glm::vec3(1.0f),
			glm::vec3(1.0f, 0.0f, 0.0f), 50.0f,
			currentDir,
			std::bind(&PluginLoader::ButtonCallback, this, _1));

		m_gui.AddElement(m_rootNode, pButton);

		startingPos.y -= 100.0f;
	}
	
	game.EnableEventWaiting(true);	
}

void PluginLoader::Destroy(Game& game)
{
	game.EnableEventWaiting(false);
}

void PluginLoader::Update(Game& game)
{
	IInput& input = game.GetInput();
	m_gui.Update(input, game.GetDt());

	if (!m_buttonPressed.empty())
	{
		game.SetNextState(m_buttonPressed);
	}
}

void PluginLoader::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();
	m_gui.Render(renderer);
}

void PluginLoader::ButtonCallback(UI::Button& button)
{
	m_buttonPressed = button.GetText();
}

void PluginLoader::LoadDirectories(std::vector<std::string>& dirList) const
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./plugin")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			std::string currentDir = ent->d_name;
			if (currentDir != "." && currentDir != ".." && currentDir != GetName())
			{
				dirList.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
}
