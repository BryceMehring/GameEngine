#include "GameStateScript.h"
#include "Game.h"
#include "FileManager.h"

void GameStateScript::Exec(Game& game, const std::string& file)
{
	bool bExists = FileManager::Instance().ProccessFileByLine(file.c_str(),[&](const std::string& line)
	{
		game.GetAs().ExecuteScript(line,0);
	});

	if(!bExists)
	{
		game.GetAs().SendMessage("Could not open: " + file);
	}
}

std::string GameStateScript::GetPathName(const std::string& file) const
{
    return (std::string("..//Scripts/") + (this->GetName()) + '/' + file);
}

void GameStateScript::Init(Game& game)
{
	Exec(game,GetPathName("init.as"));
}


void GameStateScript::Destroy(Game& game)
{
	Exec(game,GetPathName("destroy.as"));
}
