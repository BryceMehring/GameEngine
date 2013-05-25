#ifndef _OGLRESOURCEMANAGER_
#define _OGLRESOURCEMANAGER_

#include "BaseResourceManager.h"
#include <GL/glew.h>
#include <map>
#include <vector>

enum ResourceType
{
    Tex,
    Shad,
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

struct IResource
{
    IResource(int i) : id(i) {}
    virtual ~IResource() {}
    virtual ResourceType GetType() const = 0;

    GLuint id;
};

struct Texture : IResource
{
    Texture(GLuint i, int tw, int th, int sw, int sh) : IResource(i), iWidth(tw), iHeight(th), iCellsWidth(sw), iCellsHeight(sh)
    {
    }
    virtual ~Texture()
    {
        glDeleteTextures(1,&id);
    }

    virtual ResourceType GetType() const { return Tex; }

    int iWidth;
    int iHeight;
    int iCellsWidth;
    int iCellsHeight;
};

struct Shader : IResource
{
    explicit Shader(GLuint i = 0) : IResource(i)
    {
    }
    virtual ~Shader()
    {
        glDeleteProgram(id);
    }

    virtual ResourceType GetType() const { return Shad; }

    std::map<std::string,unsigned int> uniforms;
    //std::map<std::string,unsigned int> parameters;
};

struct Charset : Texture
{
    Charset(GLuint i, int tw, int th, int sw, int sh) : Texture(i,tw,th,sw,sh)
    {
    }

    virtual ResourceType GetType() const { return Font; }

    unsigned short LineHeight;
    unsigned short Base;
    unsigned short Pages;
    CharDescriptor Chars[256];
};


// Resource Manager implementation
class ResourceManager : public BaseResourceManager
{
public:

    ResourceManager();
    virtual ~ResourceManager();

    virtual bool LoadTexture(const std::string& id, const std::string& file);

    virtual bool LoadShader(const std::string& id, const std::string& vert, const std::string& frag);

    virtual bool GetTextureInfo(const std::string& id, TextureInfo& out) const;
    virtual void RemoveTexture(const std::string& id);
    virtual void RemoveAllTextures();
    virtual void RemoveAllShaders();

    virtual void Clear();

    // method only accessable in the OpenGL function to access OpenGL specific information about the resources
    IResource& GetResource(const std::string& name);

private:

    typedef std::map<std::string,IResource*> ResourceMap;

    ResourceMap m_resources;

    void ParseFont(std::fstream& stream, Charset& CharsetDesc);
};

#endif // _OGLRESOURCEMANAGER_
