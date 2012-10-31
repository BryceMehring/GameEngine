#ifndef _TEXTUREMANAGER_
#define _TEXTUREMANAGER_

#include "ITextureManager.h"

class TextureManager : public ITextureManager
{
public:

	struct Texture
	{
		IDirect3DTexture9* pTexture;
		unsigned int uiWidth;
		unsigned int uiHeight;
		unsigned int uiCells;
		//std::map<D3DXVECTOR2,D3DXVECTOR2> normals;
	};

	TextureManager(IDirect3DDevice9* pDevice);
	virtual ~TextureManager();

	virtual void LoadAllTexturesFromFolder(const std::string& folder);
	virtual void LoadTexture(const std::string& file);

	virtual bool GetTextureInfo(const std::string& name, TextureInfo& out) const;
	virtual void RemoveTexture(const std::string& name);
	virtual void RemoveAllTextures();

	bool GetTexture(const std::string& name, Texture& out);

private:

	// data members

	typedef std::map<std::string,Texture> TextureMap;
	TextureMap m_textures;
	IDirect3DDevice9* m_pDevice;

	// helper functions

	void AddTexture(const std::string& name, const Texture& tex);

	void ExtractNormals(Texture& texture);

};

#endif // _TEXTUREMANAGER_