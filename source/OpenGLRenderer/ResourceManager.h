#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "IResourceManager.h"
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <map>
#include <array>
#include <unordered_map>

// Valid resource types
enum class ResourceType
{
	Cursor,
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

	friend class ResourceManager;

	// Returns the interface of type
	// If a match cannot be made, nullptr is returned
	virtual void* QueryInterface(ResourceType type) const = 0;

protected:

	virtual ~IResource() {}
};

class Cursor : public IResource
{
public:

	Cursor(int width, int height, unsigned char* img);

	virtual void* QueryInterface(ResourceType type) const override;

	int GetWidth() const;
	int GetHeight() const;
	const unsigned char* GetImgData() const;

protected:

	virtual ~Cursor();

private:

	int m_iWidth;
	int m_iHeight;
	unsigned char* m_pImg;
};

class OpenGLResource : public IResource
{
public:

	friend class TexturedShader;
	friend class Texture;

	OpenGLResource(GLuint id);

protected:

	const GLuint m_id;
};

// Defines a texture resource
class Texture : public OpenGLResource
{
public:

	Texture(GLuint i, unsigned char* pImg, int comp, int tw, int th, int cw = 1, int ch = 1);

	virtual void* QueryInterface(ResourceType type) const override;

	int GetWidth() const;
	int GetHeight() const;
	int GetCellsWidth() const;
	int GetCellsHeight() const;
	int GetComponents() const;
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

};

// Defines a shader resource
class Shader : public OpenGLResource
{
public:

	typedef std::map<std::string,GLuint> UnifromMap;

	Shader(GLuint i, GLuint MVP, GLuint color, UnifromMap&& uniforms);

	virtual void* QueryInterface(ResourceType type) const override;

	// Sets this shader as the active shader
	void Bind();

	// Removes this shader as the active shader
	void UnBind();

	// Returns true if this shader is active
	bool IsBound() const;

	// Sets the MVP matrix in the shader
	void SetMVP(const glm::mat4& mvp) const;

	// Sets the color in the shader
	void SetColor(const glm::vec4& color) const;

	// Set generic values in the shader
	void SetValue(const std::string& location, int v);
	void SetValue(const std::string& location, float v);
	void SetValue(const std::string& location, const glm::vec2& v);
	void SetValue(const std::string& location, const glm::mat4& v);

protected:

	virtual ~Shader();

private:

	GLuint m_MVP;
	GLuint m_color;
	UnifromMap m_uniforms;
	bool m_bUse;


};

// Defines a textured shader resource
class TexturedShader : public Shader
{
public:

	TexturedShader(GLuint i, GLuint MVP, GLuint color, GLuint texID, UnifromMap&& uniforms);

	virtual void* QueryInterface(ResourceType type) const override;
	void BindTexture(const OpenGLResource& texture) const;

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

	typedef std::array<CharDescriptor,256> FontArrayType;
	typedef std::array<std::array<short, 128>, 128> KerningArrayType;

	Font(GLuint i, unsigned char* pImg, int comp, int tw, int th);

	virtual void* QueryInterface(ResourceType type) const override;

	// Returns the distance in pixels between each line of text.
	unsigned int GetLineHeight() const;

	// Returns the number of pixels from the absolute top of the line to the base of the characters.
	unsigned int GetBase() const;

	// Returns the number of texture pages included in the font.
	unsigned int GetPages() const;

	// Returns the CharDescriptor of the specified character
	const CharDescriptor& GetCharDescriptor(char c) const;

	// Returns true if c is a valid character in the font
	bool IsValidCharacter(char c) const;

	// Returns the kerning pair offset
	int GetKerningPairOffset(unsigned int first, unsigned int second) const;

	friend std::istream& operator >>(std::istream& stream, Font& CharsetDesc);

protected:

	virtual ~Font() {}

private:

	FontArrayType m_Chars;
	KerningArrayType m_kerningPairs;

	unsigned short m_LineHeight;
	unsigned short m_Base;
	unsigned short m_Pages;
};

std::istream& operator >>(const std::istream& stream, Font& out);

// Resource Manager implementation
class ResourceManager : public IResourceManager
{
public:

	ResourceManager();
	virtual ~ResourceManager();

	bool LoadCursor(const std::string& id, const std::string& file) override;

	bool LoadTexture(const std::string& id, const std::string& file) override;

	bool LoadAnimation(const std::string& id, const std::string& file) override;

	bool LoadFont(const std::string& id, const std::string& file) override;

	bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag) override;

	bool GetTextureInfo(const std::string& id, TextureInfo& out) const override;

	void Clear() override;

	// Method only accessible in the OpenGL plugin to access OpenGL specific information about the resources
	// If the resource is not found, nullptr is returned
	IResource* GetResource(const std::string& name, ResourceType type);
	const IResource* GetResource(const std::string& name, ResourceType type) const;

	IResource* GetResource(const std::string& name);
	const IResource* GetResource(const std::string& name) const;

private:

	typedef std::map<std::string, IResource*> ResourceMap;

	ResourceMap m_resources;

	bool CreateTexture(const std::string& file, int& width, int& height, int& comp, unsigned char** pImgData);
	bool CreateOpenGLTexture(const std::string& file, int& width, int& height, int& comp, unsigned char** pImgData, GLuint& out);

	// converts # of components into the corresponding OpenGL format.
	void GetOpenGLFormat(int comp, GLenum& format, GLint& internalFormat);

	// Create shader objects from program id
	bool CreateShaderInstance(const std::string& id, GLuint programID);
};

#endif // _OGLRESOURCEMANAGER_
