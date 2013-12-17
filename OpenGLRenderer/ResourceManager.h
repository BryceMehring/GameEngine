#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "IResourceManager.h"
#include <GL/glew.h>
#include <map>
#include <array>
#include <vector>

enum class ResourceType
{
	Texture,
	Animation,
	Font,
	Shader,
	TexturedShader
};

// Resource Interface
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
	Texture(GLuint i, unsigned char* pImg, int tw, int th, int sw = 1, int sh = 1) : IResource(i), m_iWidth(tw), m_iHeight(th), m_iCellsWidth(sw), m_iCellsHeight(sh), m_pImg(pImg)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::Texture; }

	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	int GetCellsWidth() const { return m_iCellsWidth; }
	int GetCellsHeight() const { return m_iCellsHeight; }
	const unsigned char* GetImgData() const { return m_pImg; }

protected:

	virtual ~Texture();

private:

	int m_iWidth;
	int m_iHeight;
	int m_iCellsWidth;
	int m_iCellsHeight;
	unsigned char* m_pImg;

	friend class ResourceManager;

};

class Shader : public IResource
{
public:

	typedef std::map<std::string,GLuint> UnifromMap;

	Shader(GLuint i, GLuint MVP, UnifromMap&& uniforms) : IResource(i), m_MVP(MVP), m_uniforms(uniforms)
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

	friend class ResourceManager;
};

class TexturedShader : public Shader
{
public:

	TexturedShader(GLuint i, GLuint MVP, GLuint texID, UnifromMap&& uniforms) : Shader(i,MVP,std::move(uniforms)), m_TextureSamplerID(texID)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::TexturedShader; }

	GLuint GetTextureSamplerID() const { return m_TextureSamplerID; }

protected:

	virtual ~TexturedShader() {}

private:

	GLuint m_TextureSamplerID;
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

struct KerningPair
{
	unsigned int first;
	unsigned int second;
	int amount;
};

class Charset : public Texture
{
public:

	typedef std::array<CharDescriptor,256> FontArray;

	Charset(GLuint i, unsigned char* pImg, int tw, int th, int sw = 1, int sh = 1) : Texture(i,pImg,tw,th,sw,sh)
	{
	}

	virtual ResourceType GetType() const { return ResourceType::Font; }

	unsigned int GetLineHeight() const { return m_LineHeight; }
	unsigned int GetBase() const { return m_Base; }
	unsigned int GetPages() const { return m_Pages; }
	const FontArray& GetCharDescriptor() const { return m_Chars; }
	int GetKerningPairOffset(unsigned int first, unsigned int second) const;

	friend std::istream& operator >>(std::istream& stream, Charset& CharsetDesc);

protected:

	virtual ~Charset() {}

private:

	unsigned short m_LineHeight;
	unsigned short m_Base;
	unsigned short m_Pages;
	FontArray m_Chars;
	std::vector<KerningPair> m_kerningPairs;

	friend class ResourceManager;
};

std::istream& operator >>(const std::istream& stream, Charset& out);

// Resource Manager implementation
class ResourceManager : public IResourceManager
{
public:

	ResourceManager();
	virtual ~ResourceManager();

	bool LoadTexture(const std::string& id, const std::string& file) override;

	bool LoadAnimation(const std::string& id, const std::string& file) override;

	bool LoadFont(const std::string& id, const std::string& file) override;

	bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag) override;

	bool GetTextureInfo(const std::string& id, TextureInfo& out) const override;

	void Clear() override;

	// method only accessable in the OpenGL function to access OpenGL specific information about the resources
	IResource* GetResource(const std::string& name);
	const IResource* GetResource(const std::string& name) const;

private:

	typedef std::map<std::string,IResource*> ResourceMap;

	ResourceMap m_resources;

	bool CreateOpenGLTexture(const std::string& file, int& width, int& height, unsigned char** pImgData, GLuint& out);

	// converts # of components into the corresponding OpenGL format.
	void GetOpenGLFormat(int comp, GLenum& format, GLint& internalFormat);

	// Create shader objects from program id
	bool CreateShaderInstance(const std::string& id, GLuint programID);
};

#endif // _OGLRESOURCEMANAGER_
