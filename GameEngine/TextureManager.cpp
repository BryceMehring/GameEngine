#include "TextureManager.h"
#include "FileManager.h"
#include "StringAlgorithms.h"
#include <d3dx9.h>
#include <vector>
#include <cassert>

using namespace std;

TextureManager::TextureManager(IDirect3DDevice9* pDevice) : m_pDevice(pDevice)
{
}

TextureManager::~TextureManager()
{
	RemoveAllTextures();
	m_pDevice->Release();
}

void TextureManager::AddTexture(const std::string& name, const Texture& tex)
{
	m_textures.insert(make_pair(name,tex));
}

void TextureManager::LoadTexture(const std::string& file)
{
	// todo: I could divide this code up into smaller functions

	// load the texture
	// todo: I could call the rendering plugin here to load the texture
	IDirect3DTexture9* pTexture = nullptr;
	assert(D3DXCreateTextureFromFile(m_pDevice,file.c_str(),&pTexture) == D3D_OK);

	// Get texture info
	D3DSURFACE_DESC format;
	pTexture->GetLevelDesc(0,&format);

	// fill out texture structure 
	Texture tex = {pTexture,format.Width,format.Height,D3DXVECTOR3(format.Width / 2.0f, format.Height / 2.0f, 0.0f)};

	std::string name = GetFileNameFromPath(file);

	// convert to lower case
	StringToLower(name);

	// insert texture into data structure
	AddTexture(name,tex);
}

void TextureManager::LoadAllTexturesFromFolder(const std::string& folder)
{
	std::vector<string> textureFileNames;
	FileManager::Instance().LoadAllFilesFromDictionary(textureFileNames,folder,".png");

	// loop through each file, and create the texture
	for_each(textureFileNames.begin(),textureFileNames.end(),[&](string& file)
	{
		LoadTexture(file);
	});
}


const Texture* TextureManager::GetTexture(const std::string& name) const
{
	auto iter = m_textures.find(name);

	if(iter != m_textures.end())
	{
		return &iter->second;
	}

	return nullptr;
}

void TextureManager::RemoveTexture(const std::string& name)
{
	auto iter = m_textures.find(name);

	if(iter != m_textures.end())
	{
		iter->second.pTexture->Release();
		m_textures.erase(iter);
	}
}

void TextureManager::RemoveAllTextures()
{
	for_each(m_textures.begin(),m_textures.end(),[&](TextureMap::value_type& data)
	{
		data.second.pTexture->Release();
	});
}
