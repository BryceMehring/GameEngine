#include "ResourceFileLoader.h"
#include "Game.h"
#include "Log.h"

#include <sstream>

void LoadResourceFile(const std::string& file, Game& game, const std::string& folder)
{
	IResourceManager& gfxResourceManager = game.GetRenderer().GetResourceManager();

	std::ifstream stream;
	stream.open((folder + '/' + file).c_str());

	// If the resource file does not exist, quit silently
	if (!stream.is_open())
		return;

	std::string line;
	while(std::getline(stream,line))
	{
		std::stringstream stream(line);

		std::string type;
		stream >> type;

		std::string id;
		stream >> id;

		std::string fileName;
		stream >> fileName;

		if(!fileName.empty())
		{
			fileName = folder + '/' + fileName;
			Log::Instance().Write("Loading: " + fileName);

			bool bSuccess = true;
			if(type == "texture")
			{
				bSuccess = gfxResourceManager.LoadTexture(id,fileName);
			}
			else if(type == "animation")
			{
				bSuccess = gfxResourceManager.LoadAnimation(id,fileName);
			}
			else if(type == "font")
			{
				bSuccess = gfxResourceManager.LoadFont(id,fileName);
			}
			else if(type == "shader")
			{
				std::string frag;
				stream >> frag;

				frag = folder + '/' + frag;

				bSuccess = gfxResourceManager.LoadShader(id,fileName,frag);
			}
			else if (type == "sound")
			{

			}
			else
			{
				Log::Instance().Write("invalid resource type");
				bSuccess = false;
			}

			assert(bSuccess);
		}
	}
}
