#include "TextureManager.h"
#include "FileManager.h"
#include "StringAlgorithms.h"
#include "gassert.h"
#include <vector>
#include <cassert>

#include <d3dx9.h>

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
	gassert(D3DXCreateTextureFromFile(m_pDevice,file.c_str(),&pTexture) == D3D_OK,"Texture could not be loaded");

	// Get texture info
	D3DSURFACE_DESC format;
	pTexture->GetLevelDesc(0,&format);

	// try to read number of cells in the animation
	std::fstream in(file + ".txt",ios::in);
	unsigned int cells = 1;

	if(in)
	{
		in >> cells;
		in.close();
	}

	// fill out texture structure 
	Texture tex = {pTexture,format.Width,format.Height,cells};

	std::string name = GetFileNameFromPath(file);

	// convert to lower case
	StringToLower(name);

	//ExtractNormals(tex);

	// insert texture into data structure
	AddTexture(name,tex);
}

float ClampColor(unsigned int c)
{
	return (c * 2.0f/255.0f) - 1.0f;
}

D3DXVECTOR2 ConvertColorToVec(unsigned int c)
{
	unsigned int r = ( c >> 16 ) & 255;
	unsigned int g = ( c >> 8 ) & 255;
	return D3DXVECTOR2(ClampColor(r), ClampColor(g));
}

void TextureManager::ExtractNormals(Texture& texture)
{
	IDirect3DTexture9* pTexture = texture.pTexture;

	D3DLOCKED_RECT R;
	pTexture->LockRect(0,&R,NULL,D3DLOCK_READONLY);

	/*DWORD* imgData = (DWORD*)R.pBits;
	for(unsigned int i = 0; i < texture.uiHeight; ++i)
	{
		for(unsigned int j = 0; j < texture.uiWidth; ++j)
		{
			int index = i * R.Pitch / 4 + j;
			DWORD color = imgData[index];

			if(!(color & 0xff000000))
			{
				texture.normals.insert(make_pair(D3DXVECTOR2(j,i),D3DXVECTOR2(ConvertColorToVec(color))));
			}
		}
	}*/

	pTexture->UnlockRect(0);
}

void TextureManager::LoadAllTexturesFromFolder(const std::string& folder)
{
	std::vector<string> textureFileNames;
	FileManager::Instance().LoadAllFilesFromDictionary(textureFileNames,folder,".png .jpg");

	// loop through each file, and create the texture
	for_each(textureFileNames.begin(),textureFileNames.end(),[&](string& file)
	{
		LoadTexture(file);
	});
}


bool TextureManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
{
	auto iter = m_textures.find(name);

	bool success = iter != m_textures.end();

	if(success)
	{
		const Texture& tex = iter->second;
		const char* pStart = (char*)&tex + sizeof(void*);
		const unsigned int uiSize = sizeof(TextureInfo) - sizeof(void*);
		memcpy_s((void*)&out,sizeof(TextureInfo),(void*)pStart,uiSize);
		//memcpy((void*)&out,(void*)pStart,uiSize);
	}

	return success;
}

bool TextureManager::GetTexture(const std::string& name, TextureManager::Texture& out)
{
	auto iter = m_textures.find(name);

	bool success = iter != m_textures.end();

	if(success)
	{
		out = iter->second;
	}

	return success;
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
