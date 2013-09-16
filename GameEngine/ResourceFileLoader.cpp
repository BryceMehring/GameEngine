#include "ResourceFileLoader.h"
#include "Game.h"

#include <fstream>
#include <sstream>

void LoadResourceFile(const std::string& file, Game& game)
{
	std::ifstream in(file);

	assert(in.is_open());

	IResourceManager& gfxResourceManager = game.GetRenderer().GetResourceManager();

	std::string line;
	while(std::getline(in,line))
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

	}

	in.close();
}