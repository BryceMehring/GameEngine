#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "BaseResourceManager.h"
#include <map>
#include <vector>

class ResourceManager : public BaseResourceManager
{
public:

	enum ResourceType
	{
		Tex,
		Shad
	};

	struct IResource
	{
		IResource(int i) : id(i) {}
		virtual ~IResource() {}
		virtual ResourceType GetType() const = 0;

		int id;
	}; 

	struct Texture : IResource
	{
		Texture(int i, int tw, int th, int sw, int sh) : IResource(i), iWidth(tw), iHeight(th), iCellsWidth(sw), iCellsHeight(sh)
		{
		}

		virtual ResourceType GetType() const { return Tex; }

		int iWidth;
		int iHeight;
		int iCellsWidth;
		int iCellsHeight;
	};

	struct Shader : IResource
	{
		explicit Shader(int i = 0) : IResource(i)
		{
		}

		virtual ResourceType GetType() const { return Shad; }

		std::map<std::string,unsigned int> uniforms;
		//std::map<std::string,unsigned int> parameters;
	};

	ResourceManager();
	virtual ~ResourceManager();

	virtual void LoadTexture(const std::string& id, const std::string& file);

	virtual void LoadShader(const std::string& id, const std::string& vert, const std::string& frag);

	virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const;
	virtual void RemoveTexture(const std::string& id);
	virtual void RemoveAllTextures();
	virtual void RemoveAllShaders();

	IResource& GetResource(const std::string& name);

private:

	typedef std::map<std::string,IResource*> ResourceMap;

	ResourceMap m_resources;
};

#endif // _OGLRESOURCEMANAGER_