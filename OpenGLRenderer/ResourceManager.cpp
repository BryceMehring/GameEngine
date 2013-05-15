#include "ResourceManager.h"
#include <fstream>
#include <stb_image.c>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    for(auto iter = m_resources.begin(); iter != m_resources.end(); ++iter)
    {
        delete iter->second;
    }
}

void ResourceManager::LoadTexture(const std::string& id, const std::string& file)
{
    auto iter = m_resources.find(id);
    if(iter != m_resources.end() && iter->second->GetType() == Tex)
    {
        // texture already loaded
        return;
    }

    int spriteWidth = 1;
    int spriteHeight = 1;

    int x, y, comp;
    unsigned char* pImg = stbi_load(file.c_str(),&x,&y,&comp,4);

    //todo: need to notify the user that the texture cannot be loaded
    if(pImg == nullptr)
        return;

    GLuint textureId;
    glGenTextures(1,&textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pImg);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    std::fstream in(file + ".txt",std::ios::in);
    if(in)
    {
        in >> spriteWidth >> spriteHeight;
        in.close();
    }

    in.open(file + ".fnt");
    if(in)
    {
        // Load font
        Charset* pCharSet = new Charset(textureId,x,y,spriteWidth,spriteHeight);
        ParseFont(in,*pCharSet);

        m_resources.insert(std::make_pair(id,pCharSet));
    }
    else
    {
        m_resources.insert(std::make_pair(id,new Texture(textureId,x,y,spriteWidth,spriteHeight)));
    }

    stbi_image_free(pImg);
}

void ResourceManager::LoadShader(const std::string& id, const std::string& vert, const std::string& frag)
{
    auto iter = m_resources.find(id);
    if(iter != m_resources.end() && iter->second->GetType() == Shad)
    {
        // shader already loaded
        return;
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
    printf("Compiling shader : %s\n", vert.c_str());
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", frag.c_str());
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    Shader* pShader = new Shader(ProgramID);

    int total = 0;
    glGetProgramiv( ProgramID, GL_ACTIVE_UNIFORMS, &total );
    for(int i = 0; i < total; ++i)
    {
        char name[128];
        int iNameLength = 0;
        int iSize = 0;
        GLenum type;

        glGetActiveUniform(ProgramID,i,sizeof(name) - 1,&iNameLength,&iSize,&type,name);
        name[iNameLength] = 0;

        GLuint location = glGetUniformLocation( ProgramID, name );

        pShader->uniforms.insert(std::make_pair(name,location));
    }

    m_resources.insert(std::make_pair(id,pShader));
}

void ResourceManager::ParseFont(std::fstream& stream, Charset& CharsetDesc)
{
    std::string Line;
    std::string Read, Key, Value;
    std::size_t i;
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
                    Converter >> CharsetDesc.LineHeight;
                else if( Key == "base" )
                    Converter >> CharsetDesc.Base;
                else if( Key == "pages" )
                    Converter >> CharsetDesc.Pages;
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
                    Converter >> CharsetDesc.Chars[CharID].x;
                else if( Key == "y" )
                    Converter >> CharsetDesc.Chars[CharID].y;
                else if( Key == "width" )
                    Converter >> CharsetDesc.Chars[CharID].Width;
                else if( Key == "height" )
                    Converter >> CharsetDesc.Chars[CharID].Height;
                else if( Key == "xoffset" )
                    Converter >> CharsetDesc.Chars[CharID].XOffset;
                else if( Key == "yoffset" )
                    Converter >> CharsetDesc.Chars[CharID].YOffset;
                else if( Key == "xadvance" )
                    Converter >> CharsetDesc.Chars[CharID].XAdvance;
                else if( Key == "page" )
                    Converter >> CharsetDesc.Chars[CharID].Page;
            }
        }
    }
}

bool ResourceManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
{
    auto iter = m_resources.find(name);

	const IResource* pResource = iter->second;

	if(iter == m_resources.end() || pResource->GetType() != ResourceType::Tex)
		return false;

	const Texture* pTex = static_cast<const Texture*>(pResource);

	out.uiCellsHeight = pTex->iCellsHeight;
	out.uiCellsWidth = pTex->iCellsWidth;
	out.uiHeight = pTex->iHeight;
	out.uiWidth = pTex->iWidth;

}
void ResourceManager::RemoveTexture(const std::string& name)
{
}
void ResourceManager::RemoveAllTextures()
{
}
void ResourceManager::RemoveAllShaders()
{
}

IResource& ResourceManager::GetResource(const std::string& name)
{
    return *(m_resources[name]);
}
