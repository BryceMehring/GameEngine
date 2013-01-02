#include "ResourceManager.h"
#include "FileManager.h"
#include "StringAlgorithms.h"
#include "gassert.h"
#include <vector>
#include <cassert>

using namespace std;

ResourceManager::ResourceManager(IDirect3DDevice9* pDevice) : m_pDevice(pDevice)
{
}

ResourceManager::~ResourceManager()
{
	RemoveAllTextures();
	RemoveAllShaders();
	m_pDevice->Release();
}

void ResourceManager::AddResource(const std::string& name, const Texture& tex)
{
	m_textures.insert(make_pair(name,tex));
}

void ResourceManager::AddResource(const std::string& name, const Shader& shad)
{
	m_shaders.insert(make_pair(name,shad));
}

void ResourceManager::LoadResourceFile(const std::string& file)
{
	FileManager::Instance().ProccessFileByLine(file.c_str(),[&](const string& line)
	{
		stringstream stream(line);

		string id;
		stream >> id;

		string fileName;
		stream >> fileName;

		if(id == "texture")
		{
			LoadTexture(fileName);
		}
		else if(id == "shader")
		{
			LoadShader(fileName);
		}
	});
}

void ResourceManager::LoadTexture(const std::string& file)
{
	std::string name = GetFileNameFromPath(file);
	StringToLower(name); // convert name to lower case

	auto iter = m_textures.find(name);

	// If the texture is already loaded, stop loading
	if(iter != m_textures.end())
		return;

	// todo: I could divide this code up into smaller functions

	// load the texture
	// todo: I could call the rendering plugin here to load the texture
	IDirect3DTexture9* pTexture = nullptr;
	gassert(D3DXCreateTextureFromFile(m_pDevice,file.c_str(),&pTexture) == D3D_OK,"Texture could not be loaded");

	// try to read number of cells in the animation
	unsigned int width = 1;
	unsigned int height = 1;

	std::fstream in(file + ".txt",ios::in);
	if(in)
	{
		in >> width >> height;
		in.close();
	}

	// Get texture info
	D3DSURFACE_DESC format;
	pTexture->GetLevelDesc(0,&format);

	// fill out texture structure 
	Texture tex = {pTexture,format.Width,format.Height,width,height};

	// insert texture into data structure
	AddResource(name,tex);
}

void ResourceManager::LoadShader(const std::string& file)
{
	ID3DXEffect* pEffect = nullptr;
	ID3DXBuffer* pBuffer = nullptr;

#ifdef _DEBUG
	D3DXCreateEffectFromFile(m_pDevice,file.c_str(),0,0,D3DXSHADER_DEBUG,0,&pEffect,&pBuffer);
#else
	D3DXCreateEffectFromFile(m_pDevice,file.c_str(),0,0,0,0,&pEffect,&pBuffer);
#endif

	// todo: create better error handling
	if(pBuffer != nullptr)
	{
		MessageBox(0,(char*)pBuffer->GetBufferPointer(),0,0);
		pBuffer->Release();
	}
	else if(pEffect == nullptr)
	{
		string errorMsg = "Could not load: " + file;

		MessageBox(0,errorMsg.c_str(),0,0);
	}
	else
	{
		Shader newShader;
		newShader.pEffect = pEffect;

		std::string name = GetFileNameFromPath(file);

		BuildHandleVectors(file.c_str(),pEffect,newShader.parameters,newShader.tech);

		// convert to lower case
		StringToLower(name);

		AddResource(name,newShader);
		
	}
}

void ResourceManager::BuildHandleVectors(const char* file, ID3DXEffect* pEffect, std::vector<D3DXHANDLE>& paramaters, std::vector<D3DXHANDLE>& tech)
{
	fstream in(file);

	if(in.is_open())
	{
		string line;

		while(getline(in,line) && line.find("Variables Start") == string::npos)
		{
		}

		while(getline(in,line) && line.find("Variables End") == string::npos)
		{
			if(line.find("//") == string::npos)
			{
				int pos = line.find_last_of(' ');
				if(pos != string::npos)
				{
					string temp = line.substr(pos+1,line.size() - pos - 2);
					paramaters.push_back(pEffect->GetParameterByName(NULL,temp.c_str()));
				}
			}
		}

		while(getline(in,line) && line.find("Tech Start") == string::npos)
		{
		}

		while(getline(in,line) && line.find("Tech End") == string::npos)
		{
			if(line.find("//") == string::npos)
			{
				if(line.find("technique") != string::npos)
				{
					int pos = line.find_last_of(' ');
					if(pos != string::npos)
					{
						tech.push_back(pEffect->GetTechniqueByName(line.substr(pos+1,line.size() - pos - 1).c_str()));
					}
				}
			}
		}

		in.close();
	}
}

void ResourceManager::LoadAllTexturesFromFolder(const std::string& folder)
{
	std::vector<string> textureFileNames;
	FileManager::Instance().LoadAllFilesFromDirectory(textureFileNames,folder,".png .jpg .dds");

	// loop through each file, and create the texture
	for(auto iter = textureFileNames.begin(); iter != textureFileNames.end(); ++iter)
	{
		LoadTexture(*iter);
	}
}


bool ResourceManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
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

bool ResourceManager::GetResource(const std::string& name, ResourceManager::Texture& out)
{
	auto iter = m_textures.find(name);

	bool success = iter != m_textures.end();

	if(success)
	{
		out = iter->second;
	}

	return success;
}

bool ResourceManager::GetResource(const std::string& name, ResourceManager::Shader& out)
{
	auto iter = m_shaders.find(name);

	bool success = iter != m_shaders.end();

	if(success)
	{
		out = iter->second;
	}

	return success;
}

void ResourceManager::OnLostDevice()
{
	for(auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter)
	{
		ID3DXEffect* pEffect = iter->second.pEffect;
		pEffect->OnLostDevice();
	}
}
void ResourceManager::OnResetDevice()
{
	for(auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter)
	{
		ID3DXEffect* pEffect = iter->second.pEffect;
		pEffect->OnResetDevice();
	}
}

void ResourceManager::RemoveTexture(const std::string& name)
{
	auto iter = m_textures.find(name);

	if(iter != m_textures.end())
	{
		iter->second.pTexture->Release();
		m_textures.erase(iter);
	}
}

void ResourceManager::RemoveAllTextures()
{
	for(auto iter = m_textures.begin(); iter != m_textures.end(); ++iter)
	{
		iter->second.pTexture->Release();
	}

	m_textures.clear();
}

void ResourceManager::RemoveAllShaders()
{
	for(auto iter = m_shaders.begin(); iter != m_shaders.end(); ++iter)
	{
		iter->second.pEffect->Release();
	}

	m_shaders.clear();
}
