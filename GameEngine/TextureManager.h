#ifndef _TEXTUREMANAGER_
#define _TEXTUREMANAGER_

#include <map>
#include <string>
#include <d3dx9.h>

struct Texture
{
	/*friend class DX9Render;
	friend class TextureManager;

private:
	IDirect3DTexture9* pTexture;
public:*/
	IDirect3DTexture9* pTexture;
	unsigned int uiWidth;
	unsigned int uiHeight;
	D3DXVECTOR3 center;
};

// todo: where should this class be located?
class TextureManager
{
public:

	TextureManager(IDirect3DDevice9* pDevice);
	~TextureManager();

	void LoadAllTexturesFromFolder(const std::string& folder);
	void LoadTexture(const std::string& file);

	const Texture* GetTexture(const std::string& name) const;
	void RemoveTexture(const std::string& name);
	void RemoveAllTextures();

private:

	void AddTexture(const std::string& name, const Texture& tex);

	typedef std::map<std::string,Texture> TextureMap;
	TextureMap m_textures;
	IDirect3DDevice9* m_pDevice;

};

#endif //_TEXTUREMANAGER_