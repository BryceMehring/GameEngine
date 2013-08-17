#ifndef _BASERESOURCEMANAGER_
#define _BASERESOURCEMANAGER_

#include "IResourceManager.h"
#include <fstream>
#include <sstream>
#include <cassert>

class BaseResourceManager : public IResourceManager
{
public:

    virtual void LoadResourceFile(const std::string& file)
	{
		std::fstream in(file,std::ios::in);

		assert(in.is_open());

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
					bSuccess = LoadTexture(id,fileName);
				}
				else if(type == "shader")
				{
					std::string fileName2;
					stream >> fileName2;

					bSuccess = LoadShader(id,fileName,fileName2);
				}
				assert(bSuccess);
			}
			
        }

        in.close();
	}

};



#endif // _BASETEXTUREMANAGER_
