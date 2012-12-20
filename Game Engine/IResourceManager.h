#ifndef _IRESOURCEMANAGER_
#define _IRESOURCEMANAGER_

#include <string>

struct TextureInfo
{
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned int uiCells;
};

class IResourceManager
{
public:

	virtual ~IResourceManager() {}

	virtual void LoadResourceFile(const std::string& file) = 0;
	virtual void LoadAllTexturesFromFolder(const std::string& folder) = 0;
	virtual void LoadTexture(const std::string& file) = 0;
	virtual void LoadShader(const std::string& file) = 0;
	virtual bool GetTextureInfo(const std::string& name, TextureInfo& out) const = 0;
	virtual void RemoveTexture(const std::string& name) = 0;
	virtual void RemoveAllTextures() = 0;
	virtual void RemoveAllShaders() = 0;

};



#endif //_IRESOURCEMANAGER_