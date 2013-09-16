#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "IResourceManager.h"
#include <GL/glew.h>
#include <map>
#include <vector>

enum class ResourceType
{
	Texture,
	Shader,
	TexturedShader,
	Font
};

struct CharDescriptor
{
	CharDescriptor() : x( 0 ), y( 0 ), Width( 0 ), Height( 0 ), XOffset( 0 ), YOffset( 0 ),
		XAdvance( 0 ), Page( 0 ) {}

	unsigned short x, y;
	unsigned short Width, Height;
	unsigned short XOffset, YOffset;
	unsigned short XAdvance;
	unsigned short Page;
};

class IResource
{
public:
	IResource(GLuint i) : id(i) {}
	virtual ResourceType GetType() const = 0;

	const GLuint& GetID() const { return id; }

protected:

	virtual ~IResource() = 0;

private:

	friend class ResourceManager;

	GLuint id;

};

class Texture : public IResource
{
public:
	Texture(GLuint i, int tw, int th, int sw, int sh) : IResource(i), m_iWidth(tw), m_iHeight(th), m_iCellsWidth(sw), m_iCellsHeight(sh)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::Texture; }

	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	int GetCellsWidth() const { return m_iCellsWidth; }
	int GetCellsHeight() const { return m_iCellsHeight; }

protected:

	virtual ~Texture()
	{
		glDeleteTextures(1,&GetID());
	}

private:

	int m_iWidth;
	int m_iHeight;
	int m_iCellsWidth;
	int m_iCellsHeight;

	friend class ResourceManager;

};

class Shader : public IResource
{
public:

	typedef std::map<std::string,GLuint> UnifromMap;

	Shader(GLuint i, GLuint MVP, const UnifromMap& uniforms) : IResource(i), m_MVP(MVP), m_uniforms(uniforms)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::Shader; }

	GLuint GetMVP() const { return m_MVP; }

	const UnifromMap& GetUniforms() const { return m_uniforms; }

protected:

	virtual ~Shader()
	{
		glDeleteProgram(GetID());
	}

private:

	GLuint m_MVP;
	UnifromMap m_uniforms;
	//std::map<std::string,unsigned int> parameters;

	friend class ResourceManager;
};

class TexturedShader : public Shader
{
public:

	TexturedShader(GLuint i, GLuint MVP, GLuint texID, const UnifromMap& uniforms) : Shader(i,MVP,uniforms), m_TextureSamplerID(texID)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::TexturedShader; }

	GLuint GetTextureSamplerID() const { return m_TextureSamplerID; }

protected:

	virtual ~TexturedShader() {}

private:

	GLuint m_TextureSamplerID;
};

class Charset : public Texture
{
public:
	Charset(GLuint i, int tw, int th, int sw, int sh) : Texture(i,tw,th,sw,sh), m_Chars(256)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::Font; }

	unsigned int GetLineHeight() const { return m_LineHeight; }
	unsigned int GetBase() const { return m_Base; }
	unsigned int GetPages() const { return m_Pages; }
	const std::vector<CharDescriptor>& GetCharDescriptor() const { return m_Chars; }

protected:

	virtual ~Charset() {}

private:

	unsigned short m_LineHeight;
	unsigned short m_Base;
	unsigned short m_Pages;
	std::vector<CharDescriptor> m_Chars;

	friend class ResourceManager;
};


// Resource Manager implementation
class ResourceManager : public IResourceManager
{
public:

	ResourceManager();
	virtual ~ResourceManager();

	virtual bool LoadTexture(const std::string& id, const std::string& file);

	virtual bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag);

	virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const;

	virtual void Clear();

	// method only accessable in the OpenGL function to access OpenGL specific information about the resources
	IResource* GetResource(const std::string& name);
	const IResource* GetResource(const std::string& name) const;

private:

	typedef std::map<std::string,IResource*> ResourceMap;

	ResourceMap m_resources;

	void GetOpenGLFormat(int comp, GLenum& format, GLint& internalFormat);
	void ParseFont(std::ifstream& stream, Charset& CharsetDesc);

	bool CreateShaderInstance(const std::string& id, GLuint programID);
};

#endif // _OGLRESOURCEMANAGER_
