#include "PluginLoader.h"
#include "Game.h"
#include <filesystem>
#include <thread>
#include "Log.h"


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

	glm::vec2 startingPos(width / 2.0f, height / 2.0f + (dirList.size() * 100) / 2.0f);
	for (const std::string& currentDir : dirList)
	{
		using namespace std::placeholders;

		auto pButton = std::make_shared<UI::Button>(
			game,
			startingPos,
			glm::vec4(0.05f, 0.0f, 0.0f, 1.0f),
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 50.0f,
			currentDir,
			std::bind(&PluginLoader::ButtonCallback, this, _1)
		);

		m_gui.AddElement(m_rootNode, pButton);

		startingPos.y -= 100.0f;
	}

	m_gui.Init(game.GetInput());

	game.EnableEventWaiting(true);

}

void PluginLoader::Destroy(Game& game)
{
	game.EnableEventWaiting(false);
}

void PluginLoader::Update(Game& game)
{
}

void PluginLoader::Draw(Game& game)
{
	IRenderer& renderer = game.GetRenderer();

	int width, height;
	renderer.GetDisplayMode(&width, &height);
	renderer.DrawString("Choose a plugin to start",
						glm::vec3(width / 2.0f, 10.0f * height / 11.0f, 0),
						glm::vec4(1.0f), 50.0f, nullptr, FontAlignment::Center);

	m_gui.Render(renderer);
}

void PluginLoader::ButtonCallback(UI::Button& button)
{
	m_buttonPressed = button.GetText();
}

void PluginLoader::LoadDirectories(std::vector<std::string>& dirList) const
{
	auto pluginPath = std::filesystem::current_path() / "bin" / "plugin";
	try {
		for (auto& p : std::filesystem::directory_iterator(pluginPath, std::filesystem::directory_options::skip_permission_denied)) {
			dirList.push_back(p.path().filename().string());

		}
	}
	catch (std::exception e) {
		Log::Instance().Write("cannot load dir: " + pluginPath.string() + " " + std::string(e.what()));
	}
}
