#include "ResourceFileLoader.h"
#include "Game.h"
#include "FileManager.h"

void LoadResourceFile(const std::string& file, Game& game)
{
	IResourceManager& gfxResourceManager = game.GetRenderer().GetResourceManager();

	bool success = FileManager::Instance().ProccessFileByLine(file.c_str(),[&](const std::string& line) -> void
	{
		std::stringstream stream(line);

		std::string type;
		stream >> type;

		std::string id;
		stream >> id;

		std::string fileName;
		stream >> fileName;

		if(fileName.size() > 0)
		{
			bool bSuccess = true;
			if(type == "texture")
			{
				bSuccess = gfxResourceManager.LoadTexture(id,fileName);
			}
			else if(type == "font")
			{
				bSuccess = gfxResourceManager.LoadFont(id,fileName);
			}
			else if(type == "shader")
			{
				std::string frag;
				stream >> frag;

				bSuccess = gfxResourceManager.LoadShader(id,fileName,frag);
			}
			else if (type == "sound")
			{

			}
			assert(bSuccess);
		}
	});
	assert(success);
}
