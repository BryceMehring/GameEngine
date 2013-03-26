#include "ResourceManager.h"
#include <GL\glew.h>
#include <SOIL.h>
#include <fstream>

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

	GLuint texId = SOIL_load_OGL_texture(file.c_str(),0,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);

	if(texId == 0)
		return; // report error, texture was not loaded

	int texWidth = 0;
	int texHeight = 0;
	int spriteWidth = 1;
	int spriteHeight = 1;

	glBindTexture(GL_TEXTURE_2D,texId);

	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&texWidth);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&texHeight);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	std::fstream in(file + ".txt",std::ios::in);
	if(in)
	{
		in >> spriteWidth >> spriteHeight;
		in.close();
	}

	m_resources.insert(std::make_pair(id,new ResourceManager::Texture(texId,texWidth,texHeight,spriteWidth,spriteHeight)));

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

bool ResourceManager::GetTextureInfo(const std::string& name, TextureInfo& out) const
{
	return false;
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

ResourceManager::IResource& ResourceManager::GetResource(const std::string& name)
{
	return *(m_resources[name]);
}
