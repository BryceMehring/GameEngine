#ifndef _RESOURCEMANAGER_
#define _RESOURCEMANAGER_

#include "IResourceManager.h"
#include <map>

class ResourceManager : public IResourceManager
{
public:

	// todo: create a polymorphic data type?
	struct Texture
	{
		struct IDirect3DTexture9* pTexture;
		unsigned int uiWidth;
		unsigned int uiHeight;
		unsigned int uiCellsWidth;
		unsigned int uiCellsHeight;
	};

	struct Shader
	{
		explicit Shader(struct ID3DXEffect* effect = nullptr) : pEffect(effect) {}

		struct ID3DXEffect* pEffect;

		// todo: add more
	};

	ResourceManager(struct IDirect3DDevice9* pDevice);
	virtual ~ResourceManager();

	virtual void LoadResourceFile(const std::string& file);

	virtual void LoadAllTexturesFromFolder(const std::string& folder);
	virtual void LoadTexture(const std::string& file);

	virtual void LoadShader(const std::string& file);

	virtual bool GetTextureInfo(const std::string& name, TextureInfo& out) const;
	virtual void RemoveTexture(const std::string& name);
	virtual void RemoveAllTextures();
	virtual void RemoveAllShaders();

	bool GetResource(const std::string& name, Texture& out);
	bool GetResource(const std::string& name, Shader& out);

	void OnLostDevice();
	void OnResetDevice();

private:

	// data members

	typedef std::map<std::string,Texture> TextureMap;
	typedef std::map<std::string,Shader> ShaderMap;

	IDirect3DDevice9* m_pDevice;

	TextureMap m_textures;
	ShaderMap m_shaders;

	// helper functions
	void AddResource(const std::string& name, const Texture& tex);
	void AddResource(const std::string& name, const Shader& tex);

	void ExtractNormals(Texture& texture);

};

#endif // _RESOURCEMANAGER_