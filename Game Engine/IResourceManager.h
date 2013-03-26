#ifndef _IRESOURCEMANAGER_
#define _IRESOURCEMANAGER_

#include <string>

struct TextureInfo
{
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned int uiCellsWidth;
	unsigned int uiCellsHeight;
};

class __declspec(novtable) IResourceManager
{
public:

	virtual ~IResourceManager() {}

	virtual void LoadResourceFile(const std::string& file) = 0;
	virtual void LoadTexture(const std::string& id, const std::string& file) = 0;
	virtual void LoadShader(const std::string& id, const std::string& vert, const std::string& frag) = 0;
	virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const = 0;
	virtual void RemoveTexture(const std::string& id) = 0;
	virtual void RemoveAllTextures() = 0;
	virtual void RemoveAllShaders() = 0;

};



#endif //_IRESOURCEMANAGER_