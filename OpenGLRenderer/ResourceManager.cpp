﻿
#include "ResourceManager.h"
#include "Log.h"
#include <sstream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
#include <stb_image.c>

class KerningPairFunctor
{
public:

	bool operator()(const KerningPair& a, const KerningPair& b) const
	{
		return a.first < b.first;
	}
};

std::istream& operator >>(std::istream& stream, Charset& CharsetDesc)
{
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;
	unsigned int index = 0;
	while( !stream.eof() )
	{
		std::stringstream LineStream;
		std::getline( stream, Line );
		LineStream << Line;

		//read the line's type
		LineStream >> Read;
		if( Read == "common" )
		{
			//this holds common data
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "lineHeight" )
					Converter >> CharsetDesc.m_LineHeight;
				else if( Key == "base" )
					Converter >> CharsetDesc.m_Base;
				else if( Key == "pages" )
					Converter >> CharsetDesc.m_Pages;
			}
		}
		else if( Read == "char" )
		{
			//this is data for a specific char
			unsigned short CharID = 0;

			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "id" )
					Converter >> CharID;
				else if( Key == "x" )
					Converter >> CharsetDesc.m_Chars[CharID].x;
				else if( Key == "y" )
					Converter >> CharsetDesc.m_Chars[CharID].y;
				else if( Key == "width" )
					Converter >> CharsetDesc.m_Chars[CharID].Width;
				else if( Key == "height" )
					Converter >> CharsetDesc.m_Chars[CharID].Height;
				else if( Key == "xoffset" )
					Converter >> CharsetDesc.m_Chars[CharID].XOffset;
				else if( Key == "yoffset" )
					Converter >> CharsetDesc.m_Chars[CharID].YOffset;
				else if( Key == "xadvance" )
					Converter >> CharsetDesc.m_Chars[CharID].XAdvance;
				else if( Key == "page" )
					Converter >> CharsetDesc.m_Chars[CharID].Page;
			}
		}
		else if(Read.find("kerning") != std::string::npos)
		{
			while(!LineStream.eof())
			{
				std::stringstream Converter;
				LineStream >> Read;

				i = Read.find('=');
				Key = Read.substr(0,i);
				Value = Read.substr(i + 1);

				Converter << Value;
				if(Key == "count")
				{
					unsigned int count = 0;
					Converter >> count;

					CharsetDesc.m_kerningPairs.resize(count);
				}
				else if(Key == "first")
				{
					Converter >> CharsetDesc.m_kerningPairs[index].first;
				}
				else if(Key == "second")
				{
					Converter >> CharsetDesc.m_kerningPairs[index].second;
				}
				else if(Key == "amount")
				{
					Converter >> CharsetDesc.m_kerningPairs[index].amount;
					index++;

					break;
				}
			}
		}
	}

	std::sort(CharsetDesc.m_kerningPairs.begin(),CharsetDesc.m_kerningPairs.end(),KerningPairFunctor());

	return stream;
}

IResource::~IResource()
{
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	Clear();
}

Texture::~Texture()
{
	glDeleteTextures(1,&GetID());
	stbi_image_free(m_pImg);
}

int Charset::GetKerningPairOffset(unsigned int first, unsigned int second) const
{
	int offset = 0;
	KerningPair searchValue = {first,0,0};

	auto iterPair = std::equal_range(m_kerningPairs.begin(),m_kerningPairs.end(),searchValue,KerningPairFunctor());

	while((offset == 0) && (iterPair.first != iterPair.second))
	{
		if(iterPair.first->second == second)
		{
			offset = iterPair.first->amount;
		}

		++iterPair.first;
	}

	return offset;
}

void ResourceManager::GetOpenGLFormat(int comp, GLenum& format, GLint& internalFormat)
{
	switch(comp)
	{
	case 1:
		format = GL_RED;
		internalFormat = GL_RED;
		break;
	case 2:
		format = GL_RG;
		internalFormat = GL_RG;
		break;

	case 3:
		format = GL_RGB;
		internalFormat = GL_RGB;
		break;

	case 4:
		format = GL_RGBA;
		internalFormat = GL_RGBA; 
		break;
	}
}

bool ResourceManager::CreateOpenGLTexture(const std::string& file, int& width, int& height, unsigned char** pImgData, GLuint& textureId)
{
	if(pImgData == nullptr)
		return false;

	int comp;
	*pImgData = stbi_load(file.c_str(),&width,&height,&comp,0);

	// check if the image can be loaded
	if(pImgData == nullptr)
		return false;

	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum format;
	GLint internalFormat;
	GetOpenGLFormat(comp,format,internalFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, (void*)(*pImgData));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

bool ResourceManager::LoadTexture(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		if(iter->second->GetType() == ResourceType::Texture)
			return true; //texture already loaded

		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	bool success;
	if((success = CreateOpenGLTexture(file, width, height, &pImg,textureID)))
	{
		m_resources.insert(std::make_pair(id,new Texture(textureID,pImg,width,height)));
	}
	return success;
}

bool ResourceManager::LoadAnimation(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		if(iter->second->GetType() == ResourceType::Animation)
			return true; //Animation already loaded

		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	bool success;
	if((success = CreateOpenGLTexture(file, width, height, &pImg,textureID)))
	{
		std::ifstream in(file + ".txt");
		if((success = in.is_open()))
		{
			int spriteWidth, spriteHeight;
			in >> spriteWidth >> spriteHeight;
			m_resources.insert(std::make_pair(id,new Texture(textureID,pImg,width,height,spriteWidth,spriteHeight)));
		}
	}
	return success;
}

bool ResourceManager::LoadFont(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		if(iter->second->GetType() == ResourceType::Font)
			return true; //Font already loaded

		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	bool success;
	if((success = CreateOpenGLTexture(file, width, height, &pImg,textureID)))
	{
		std::ifstream in(file + ".fnt");
		if((success = in.is_open()))
		{
			Charset* pCharset = new Charset(textureID,pImg,width,height);
			in >> (*pCharset);

			m_resources.insert(std::make_pair(id,pCharset));
		}
	}
	return success;
}

bool ResourceManager::LoadShader(const std::string& id, const std::string& vert, const std::string& frag)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end() && iter->second->GetType() == ResourceType::Shader)
	{
		// shader already loaded
		return true;
	}

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vert.c_str(), std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(frag.c_str(), std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	Log::Instance().Write(std::string("Compiling shader: ") + std::string(vert.c_str()));
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);

	if(VertexShaderErrorMessage.size() > 1)
		Log::Instance().Write((char*)&VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	Log::Instance().Write(std::string("Compiling shader: ") + std::string(frag.c_str()));
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);

	if(FragmentShaderErrorMessage.size() > 1)
		Log::Instance().Write(&FragmentShaderErrorMessage[0]);

	// Link the program
	Log::Instance().Write("Linking Program");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);

	if(ProgramErrorMessage.size() > 1)
		Log::Instance().Write(&ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ((Result == GL_TRUE) && CreateShaderInstance(id,ProgramID));
}

bool ResourceManager::CreateShaderInstance(const std::string& id, GLuint programID)
{
	bool bFoundMVP = false;
	GLuint uiMVPLocation = 0;

	bool bFoundTexture = false;
	GLuint uiTextureLocation = 0;

	Shader::UnifromMap uniforms;

	// Get a list of all the uniform variables in the shader
	GLint uniformCount = 0;
	glGetProgramiv( programID, GL_ACTIVE_UNIFORMS, &uniformCount );
	for(GLint i = 0; i < uniformCount; ++i)
	{
		char name[128];
		int iNameLength = 0;
		int iSize = 0;
		GLenum type;

		glGetActiveUniform(programID,i,sizeof(name) - 1,&iNameLength,&iSize,&type,name);
		name[iNameLength] = 0;

		GLuint location = glGetUniformLocation( programID, name );

		if (type == GL_SAMPLER_2D)
		{
			uiTextureLocation = location;
			bFoundTexture = true;
		}
		else if(std::strcmp(name,"MVP") == 0 && type == GL_FLOAT_MAT4)
		{
			uiMVPLocation = location;
			bFoundMVP = true;
		}
		else
		{
			uniforms.insert(std::make_pair(name,location));
		}
	}

	// Each shader must at least have a MVP matrix
	if(bFoundMVP)
	{
		Shader* pShader = nullptr;
		if(bFoundTexture)
		{
			pShader = new TexturedShader(programID,uiMVPLocation,uiTextureLocation,std::move(uniforms));
		}
		else
		{
			pShader = new Shader(programID,uiMVPLocation,std::move(uniforms));
		}

		m_resources.insert(std::make_pair(id,pShader));
	}

	return bFoundMVP;
}

bool ResourceManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
{
	auto iter = m_resources.find(name);

	if(iter == m_resources.end())
		return false;

	const IResource* pResource = iter->second;

	if(pResource->GetType() != ResourceType::Texture)
		return false;

	const Texture* pTex = static_cast<const Texture*>(pResource);

	out.uiCellsHeight = pTex->m_iCellsHeight;
	out.uiCellsWidth = pTex->m_iCellsWidth;
	out.uiHeight = pTex->m_iHeight;
	out.uiWidth = pTex->m_iWidth;
	out.pImg = pTex->m_pImg;

	return true;
}
void ResourceManager::Clear()
{
	for(auto iter = m_resources.begin(); iter != m_resources.end(); ++iter)
	{
		delete iter->second;
	}

	m_resources.clear();
}

IResource* ResourceManager::GetResource(const std::string& name)
{
	auto iter = m_resources.find(name);

	if(iter != m_resources.end())
	{
		return iter->second;
	}
	return nullptr;
}

const IResource* ResourceManager::GetResource(const std::string& name) const
{
	auto iter = m_resources.find(name);

	if(iter != m_resources.end())
	{
		return iter->second;
	}
	return nullptr;
}
