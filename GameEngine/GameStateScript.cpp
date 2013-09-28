#include "GameStateScript.h"
#include "Game.h"
#include "FileManager.h"

void GameStateScript::Exec(Game& game, const std::string& file)
{
	std::ifstream stream;
	stream.open(file.c_str());

	if(stream.is_open())
	{
		std::string line;
		while(std::getline(stream,line))
		{
			game.GetAs().ExecuteScript(line,0);
		}
	}
	else
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
