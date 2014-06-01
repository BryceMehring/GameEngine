
#include "ResourceManager.h"
#include "Log.h"
#include <sstream>
#include <vector>
#include <cstring>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif
#include <stb_image.c>

Cursor::Cursor(int width, int height, unsigned char *img) : m_iWidth(width), m_iHeight(height), m_pImg(img)
{
}

Cursor::~Cursor()
{
	stbi_image_free(m_pImg);
}

void* Cursor::QueryInterface(ResourceType type) const
{
	if (type == ResourceType::Cursor)
	{
		return (void*)this;
	}

	return nullptr;
}

int Cursor::GetWidth() const
{
	return m_iWidth;
}

int Cursor::GetHeight() const
{
	return m_iHeight;
}

const unsigned char* Cursor::GetImgData() const
{
	return m_pImg;
}

OpenGLResource::OpenGLResource(GLuint id) : m_id(id)
{
}

Texture::Texture(GLuint i, unsigned char* pImg, int comp, int tw, int th, int cw, int ch) : OpenGLResource(i), m_iWidth(tw),
m_iHeight(th), m_iCellsWidth(cw), m_iCellsHeight(ch), m_iComp(comp), m_pImg(pImg)
{
}

Texture::~Texture()
{
	glDeleteTextures(1,&m_id);
	stbi_image_free(m_pImg);
}

void* Texture::QueryInterface(ResourceType type) const
{
	if (type == ResourceType::Texture)
	{
		return (void*)this;
	}

	return nullptr;
}

int Texture::GetWidth() const
{ 
	return m_iWidth;
}

int Texture::GetHeight() const
{ 
	return m_iHeight;
}

int Texture::GetCellsWidth() const
{
	return m_iCellsWidth;
}

int Texture::GetCellsHeight() const
{ 
	return m_iCellsHeight;
}

int Texture::GetComponents() const
{
	return m_iComp;
}

const unsigned char* Texture::GetImgData() const
{ 
	return m_pImg; 
}

Shader::Shader(GLuint i, GLuint MVP, GLuint color) : OpenGLResource(i), m_MVP(MVP), m_color(color), m_bUse(false)
{
}

Shader::~Shader()
{
	glDeleteProgram(m_id);
}

void* Shader::QueryInterface(ResourceType type) const
{
	if (type == ResourceType::Shader)
	{
		return (void*)this;
	}

	return nullptr;
}

void Shader::Bind()
{
	if(!m_bUse)
	{
		glUseProgram(m_id);
		m_bUse = true;
	}
}

void Shader::UnBind()
{
	if(m_bUse)
	{
		glUseProgram(0);
		m_bUse = false;
	}
}

bool Shader::IsBound() const
{
	return m_bUse;
}

void Shader::SetMVP(const glm::mat4& mvp) const
{
	if(m_bUse)
	{
		glUniformMatrix4fv(m_MVP,1,false,&mvp[0][0]);
	}
}

void Shader::SetColor(const glm::vec4& color) const
{
	if(m_bUse)
	{
		glUniform4fv(m_color, 1, &color[0]);
	}
}

void Shader::SetValue(int location, int v)
{
	if(m_bUse)
	{
		glUniform1i(location, v);
	}
}

void Shader::SetValue(int location, unsigned int v)
{
	if(m_bUse)
	{
		glUniform1ui(location, v);
	}
}

void Shader::SetValue(int location, float v)
{
	if(m_bUse)
	{
		glUniform1f(location, v);
	}
}

void Shader::SetValue(int location, const glm::vec2& v)
{
	if(m_bUse)
	{
		glUniform2fv(location, 1, &v[0]);
	}
}

void Shader::SetValue(int location, const glm::mat4& v)
{
	if(m_bUse)
	{
		glUniformMatrix4fv(location, 1, false, &v[0][0]);
	}
}

TexturedShader::TexturedShader(GLuint i, GLuint MVP, GLuint color, GLuint texID) : Shader(i, MVP, color), m_TextureSamplerID(texID)
{
}

void* TexturedShader::QueryInterface(ResourceType type) const
{
	if (type == ResourceType::TexturedShader)
	{
		return (void*)this;
	}

	return Shader::QueryInterface(type);
}

void TexturedShader::BindTexture(const OpenGLResource& texture) const
{
	if(IsBound())
	{
		glBindTexture(GL_TEXTURE_2D, texture.m_id);
		glUniform1i(m_TextureSamplerID, 0);
	}
}

Font::Font(GLuint i, unsigned char* pImg, int comp, int tw, int th) : Texture(i, pImg, comp, tw, th, 1, 1)
{
}

void* Font::QueryInterface(ResourceType type) const
{
	if (type == ResourceType::Font)
	{
		return (void*)this;
	}

	return nullptr;
}

unsigned int Font::GetLineHeight() const
{ 
	return m_LineHeight;

}
unsigned int Font::GetBase() const
{ 
	return m_Base;
}

unsigned int Font::GetPages() const
{ 
	return m_Pages;
}

const CharDescriptor& Font::GetCharDescriptor(char c) const
{ 
	return m_Chars[c];
}

bool Font::IsValidCharacter(char c) const
{ 
	return (c < (int)m_Chars.size()) && (c >= 0);
}

int Font::GetKerningPairOffset(unsigned int first, unsigned int second) const
{
	int offset = 0;

	auto firstIter = m_kerningPairs.find(first);
	if (firstIter != m_kerningPairs.end())
	{
		auto secondIter = firstIter->second.find(second);
		if (secondIter != firstIter->second.end())
		{
			offset = secondIter->second;
		}
	}

	return offset;
}

std::istream& operator >>(std::istream& stream, Font& CharsetDesc)
{
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;
	unsigned int first = 0;
	unsigned int second = 0;
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
				if(Key == "first")
				{
					Converter >> first;
				}
				else if(Key == "second")
				{
					Converter >> second;
				}
				else if(Key == "amount")
				{
					Converter >> CharsetDesc.m_kerningPairs[first][second];
					
					break;
				}
			}
		}
	}

	return stream;
}

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	Clear();
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

bool ResourceManager::CreateTexture(const std::string& file, int& width, int& height, int& comp, unsigned char** pImgData)
{
	if(pImgData == nullptr)
		return false;

	*pImgData = stbi_load(file.c_str(),&width,&height,&comp,0);

	// check if the image can be loaded
	if(*pImgData == nullptr)
		return false;

	return true;
}

bool ResourceManager::CreateOpenGLTexture(const std::string& file, int& width, int& height, int& comp, unsigned char** pImgData, GLuint& textureId)
{
	CreateTexture(file, width, height, comp, pImgData);

	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	GLenum format;
	GLint internalFormat;
	GetOpenGLFormat(comp,format,internalFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, (void*)(*pImgData));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

bool ResourceManager::LoadCursor(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		// ID is taken
		return false;
	}

	unsigned char* pImg;
	int width, height;
	int comp;
	bool success;
	if ((success = CreateTexture(file, width, height, comp, &pImg)))
	{
		m_resources.emplace(id, new Cursor(width, height, pImg));
	}
	return success;
}

bool ResourceManager::LoadTexture(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	int comp;
	bool success;
	if ((success = CreateOpenGLTexture(file, width, height, comp, &pImg, textureID)))
	{
		m_resources.emplace(id, new Texture(textureID, pImg, comp, width, height));
	}
	return success;
}

bool ResourceManager::LoadAnimation(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	int comp;
	bool success;
	if ((success = CreateOpenGLTexture(file, width, height, comp, &pImg, textureID)))
	{
		std::ifstream in(file + ".txt");
		if((success = in.is_open()))
		{
			int spriteWidth, spriteHeight;
			in >> spriteWidth >> spriteHeight;
			m_resources.emplace(id, new Texture(textureID, pImg, comp, width, height, spriteWidth, spriteHeight));
		}
	}
	return success;
}

bool ResourceManager::LoadFont(const std::string& id, const std::string& file)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		// ID is taken
		return false;
	}

	GLuint textureID;
	unsigned char* pImg;
	int width, height;
	int comp;
	bool success;
	if ((success = CreateOpenGLTexture(file, width, height, comp, &pImg, textureID)))
	{
		std::ifstream in(file + ".fnt");
		if((success = in.is_open()))
		{
			Font* pCharset = new Font(textureID, pImg, comp, width, height);
			in >> (*pCharset);

			m_resources.emplace(id, pCharset);
		}
	}
	return success;
}

bool ResourceManager::LoadShader(const std::string& id, const std::string& vert, const std::string& frag)
{
	auto iter = m_resources.find(id);
	if(iter != m_resources.end())
	{
		// ID is taken
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
	int mvpIndex = -1;
	int uniformColorIndex = -1;
	int textureSamplerIndex = -1;

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

		if (std::strcmp(name, "MVP") == 0)
		{
			mvpIndex = location;
		}
		else if (std::strcmp(name, "uniformColor") == 0)
		{
			uniformColorIndex = location;
		}
		else if (std::strcmp(name, "textureSampler") == 0)
		{
			textureSamplerIndex = location;
		}
	}

	bool bSuccess = false;

	// Each shader must at least have a MVP matrix and a color vector
	if (bSuccess = ((mvpIndex != -1) && (uniformColorIndex != -1)))
	{
		Shader* pShader = nullptr;
		if (textureSamplerIndex != -1)
		{
			pShader = new TexturedShader(programID, mvpIndex, uniformColorIndex, textureSamplerIndex);
		}
		else
		{
			pShader = new Shader(programID, mvpIndex, uniformColorIndex);
		}

		m_resources.emplace(id, pShader);
	}

	return bSuccess;
}

bool ResourceManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
{
	auto iter = m_resources.find(name);

	if(iter == m_resources.end())
		return false;

	Texture* pTexture = static_cast<Texture*>(iter->second->QueryInterface(ResourceType::Texture));

	if (pTexture == nullptr)
		return false;

	out = { pTexture->GetHeight(),
			pTexture->GetWidth(),
			pTexture->GetCellsWidth(),
			pTexture->GetCellsHeight(),
			pTexture->GetComponents(),
			pTexture->GetImgData()
		  };

	return true;
}

void ResourceManager::Clear()
{
	for(auto& iter : m_resources)
	{
		delete iter.second;
	}

	m_resources.clear();
}

IResource* ResourceManager::GetResource(const std::string& name, ResourceType type)
{
	IResource* pResource = GetResource(name);
	if(pResource == nullptr)
	{
		return pResource;
	}

	return static_cast<IResource*>(pResource->QueryInterface(type));
}

const IResource* ResourceManager::GetResource(const std::string& name, ResourceType type) const
{
	const IResource* pResource = GetResource(name);
	if(pResource == nullptr)
	{
		return pResource;
	}

	return static_cast<const IResource*>(pResource->QueryInterface(type));
}

IResource* ResourceManager::GetResource(const std::string& name)
{
	auto iter = m_resources.find(name);

	if(iter == m_resources.end())
	{
		return nullptr;
	}

	return iter->second;
}

const IResource* ResourceManager::GetResource(const std::string& name) const
{
	auto iter = m_resources.find(name);

	if(iter == m_resources.end())
	{
		return nullptr;
	}

	return iter->second;
}
