#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "IResourceManager.h"
#include <GL/glew.h>
#include <map>
#include <array>
#include <unordered_map>

// Valid resource types
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

	// Returns the interface of type
	// If a match cannot be made, nullptr is returned
	virtual void* QueryInterface(ResourceType type) = 0;

	// Return the OpenGL object id
	const GLuint& GetID() const;

protected:

	virtual ~IResource() = 0;

private:

	friend class ResourceManager;

	GLuint id;

};

// Defines a texture resource
class Texture : public IResource
{
public:
	Texture(GLuint i, unsigned char* pImg, int comp, int tw, int th, int cw = 1, int ch = 1);

	virtual void* QueryInterface(ResourceType type) override;

	int GetWidth() const;
	int GetHeight() const;
	int GetCellsWidth() const;
	int GetCellsHeight() const;
	const unsigned char* GetImgData() const;

protected:

	virtual ~Texture();

private:

	int m_iWidth;
	int m_iHeight;
	int m_iCellsWidth;
	int m_iCellsHeight;
	int m_iComp;
	unsigned char* m_pImg;

	friend class ResourceManager;

};

// Defines a shader resource
class Shader : public IResource
{
public:

	typedef std::map<std::string,GLuint> UnifromMap;

	Shader(GLuint i, GLuint MVP, UnifromMap&& uniforms);

	virtual void* QueryInterface(ResourceType type) override;

	GLuint GetMVP() const;

	const UnifromMap& GetUniforms() const;

protected:

	virtual ~Shader();

private:

	GLuint m_MVP;
	UnifromMap m_uniforms;

	friend class ResourceManager;
};

// Defines a textured shader resource
class TexturedShader : public Shader
{
public:

	TexturedShader(GLuint i, GLuint MVP, GLuint texID, UnifromMap&& uniforms);

	virtual void* QueryInterface(ResourceType type) override;

	GLuint GetTextureSamplerID() const;

protected:

	virtual ~TexturedShader() {}

private:

	GLuint m_TextureSamplerID;
};

struct CharDescriptor
{
	unsigned short x, y;
	unsigned short Width, Height;
	short XOffset, YOffset;
	short XAdvance;
	unsigned short Page;
};

// Defines a font resource
class Font : public Texture
{
public:

	typedef std::array<CharDescriptor,256> FontArray;

	Font(GLuint i, unsigned char* pImg, int comp, int tw, int th);

	virtual void* QueryInterface(ResourceType type) override;

	unsigned int GetLineHeight() const;
	unsigned int GetBase() const;
	unsigned int GetPages() const;
	const CharDescriptor& GetCharDescriptor(char c) const;
	bool IsValidCharacter(char c) const;
	int GetKerningPairOffset(unsigned int first, unsigned int second) const;

	friend std::istream& operator >>(std::istream& stream, Font& CharsetDesc);

protected:

	virtual ~Font() {}

private:

	unsigned short m_LineHeight;
	unsigned short m_Base;
	unsigned short m_Pages;
	FontArray m_Chars;

	std::unordered_map<unsigned int, std::unordered_map<unsigned int, int>> m_kerningPairs;

	friend class ResourceManager;
};

std::istream& operator >>(const std::istream& stream, Font& out);

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

	// method only accessible in the OpenGL plugin to access OpenGL specific information about the resources
	IResource* GetResource(const std::string& name, ResourceType type);
	const IResource* GetResource(const std::string& name, ResourceType type) const;

private:

	typedef std::map<std::string,IResource*> ResourceMap;

	ResourceMap m_resources;

	bool CreateOpenGLTexture(const std::string& file, int& width, int& height, int& comp, unsigned char** pImgData, GLuint& out);

	// converts # of components into the corresponding OpenGL format.
	void GetOpenGLFormat(int comp, GLenum& format, GLint& internalFormat);

	// Create shader objects from program id
	bool CreateShaderInstance(const std::string& id, GLuint programID);
};

#endif // _OGLRESOURCEMANAGER_
