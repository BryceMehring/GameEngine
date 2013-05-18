#ifndef _BASERESOURCEMANAGER_
#define _BASERESOURCEMANAGER_

#include "IResourceManager.h"
#include <fstream>
#include <sstream>

class BaseResourceManager : public IResourceManager
{
public:

    virtual bool LoadResourceFile(const std::string& file)
	{
		std::fstream in(file,std::ios::in);

        if(!in.is_open())
            return false;

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

            if(type == "texture")
            {
                LoadTexture(id,fileName);
            }
            else if(type == "shader")
            {
                std::string fileName2;
                stream >> fileName2;

                LoadShader(id,fileName,fileName2);
            }
        }

        in.close();

        return true;

	}

};



#endif // _BASETEXTUREMANAGER_
