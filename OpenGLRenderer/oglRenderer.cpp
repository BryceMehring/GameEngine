#include "oglRenderer.h"

#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new oglRenderer();
}

oglRenderer::oglRenderer() : m_pCamera(nullptr), m_uiCurrentDisplayMode(0), m_pFonts(nullptr), m_bFullscreen(false)
{
	m_pCamera = CreateCamera();
	m_pCamera->setLens(200.0f,200.0f,0.0f,5000.0f);
	m_pCamera->lookAt(glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	m_pCamera->update(0.0f);

	EnumerateDisplayAdaptors();

	GLFWOpenWindowHints();
	if(glfwOpenWindow( m_VideoModes[0].Width,
					   m_VideoModes[0].Height,
					   m_VideoModes[0].RedBits,
					   m_VideoModes[0].GreenBits,
					   m_VideoModes[0].BlueBits,
					   8,24,0,GLFW_WINDOW ) < 1) // // GLFW_WINDOW, GLFW_FULLSCREEN
	{
		glfwTerminate();
		throw std::string("Failed to create window");
	}

	int major, minor, rev;
	glfwGetGLVersion(&major,&minor,&rev);

	cout<<"major: "<<major<<endl;
	cout<<"minor: "<<minor<<endl;
	cout<<"rev: "<<rev<<endl;

	glfwSwapInterval(1);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		throw std::string("Failed to initialize GLEW");
	}

	m_pFonts = new FontEngine(&m_rm,1024*8,m_pCamera);
	m_pLines = new LineEngine(&m_rm,1024*20,m_pCamera);
	m_pSprites = new SpriteEngine(&m_rm,1024*20,m_pCamera);

}

oglRenderer::~oglRenderer()
{
	delete m_pFonts;
	delete m_pLines;
	delete m_pSprites;
	m_pCamera->Release();
	m_pCamera = nullptr;

	glfwCloseWindow();
}

void oglRenderer::Init(asIScriptEngine* pAS)
{
	// todo: implement this later
}

void oglRenderer::Destroy(asIScriptEngine* pAS)
{
	// todo: implement this later
}

DLLType oglRenderer::GetPluginType() const
{
	return DLLType::Rendering;
}
const char* oglRenderer::GetName() const
{
	return "oglRenderer";
}
int oglRenderer::GetVersion() const
{
	return 0;
}

void oglRenderer::GLFWOpenWindowHints()
{
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES,2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2); // We want OpenGL 2.1
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
}

void oglRenderer::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
}

void oglRenderer::Present()
{
	ClearScreen();

	m_pLines->Render();
	m_pSprites->Render();
	m_pFonts->Render();


	glfwSwapBuffers();
}

void oglRenderer::EnableVSync(bool enable)
{
	glfwSwapInterval(enable ? 1 : 0);
}

void oglRenderer::EnumerateDisplayAdaptors()
{
	m_VideoModes.resize(64);

	int iVideoModes = glfwGetVideoModes(&m_VideoModes.front(),m_VideoModes.size());

	m_VideoModes.resize(iVideoModes);
	m_VideoModes.shrink_to_fit();

	m_VideoModeStr.reserve(iVideoModes);

	std::reverse(m_VideoModes.begin(),m_VideoModes.end());

	for(unsigned int i = 0; i < m_VideoModes.size(); ++i)
	{
		std::stringstream stream;
		stream << m_VideoModes[i].Width << 'x' << m_VideoModes[i].Height;

		m_VideoModeStr.push_back(stream.str());
	}
}

unsigned int oglRenderer::GetNumDisplayModes() const
{
	return m_VideoModes.size();
}

unsigned int oglRenderer::GetCurrentDisplayMode() const
{
	return m_uiCurrentDisplayMode;
}

void oglRenderer::SetDisplayMode(unsigned int i)
{
	if(i < GetNumDisplayModes())
	{
		glfwSetWindowSize(m_VideoModes[i].Width,m_VideoModes[i].Height);
	}
}

const std::string& oglRenderer::GetDisplayModeStr(unsigned int i) const
{
	// todo: return error if i is out of the range of the vector
	return m_VideoModeStr[i];
}

void oglRenderer::ToggleFullscreen()
{
	/*m_bFullscreen = !m_bFullscreen;

	glfwCloseWindow();

	GLFWOpenWindowHints();

	if(m_bFullscreen)
	{
		CreateWindow(m_VideoModes[m_uiCurrentDisplayMode].Width,
					   m_VideoModes[m_uiCurrentDisplayMode].Height,GLFW_FULLSCREEN);
	}
	else
	{
		CreateWindow(800,600,GLFW_WINDOW);
	}

	m_pFonts->OnReset();
	m_pLines->OnReset();
	m_pSprites->OnReset();
	m_rm.Clear();*/

}

IResourceManager& oglRenderer::GetResourceManager()
{
	return m_rm;
}

void oglRenderer::GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	m_pFonts->GetStringRec(str,scale,out);
}

void oglRenderer::DrawString(const char* str, const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& color, const char* font, FontAlignment options)
{
	m_pFonts->DrawString(str,font,pos,scale,color,options);
}

void oglRenderer::GetLineWidthRange(glm::vec2& out) const
{
	m_pLines->GetLineWidthRange(out);
}

void oglRenderer::DrawLine(const glm::vec3* pArray, unsigned int length, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	m_pLines->DrawLine(pArray,length,fWidth,color,T);
}

void oglRenderer::DrawSprite(const std::string& texture, const glm::mat4& transformation, const glm::vec2& tiling, unsigned int iCellId)
{
	m_pSprites->DrawSprite("sprite",texture,transformation,tiling,iCellId);
}

void oglRenderer::SetShaderValue(const std::string& shader, const string& location, float value )
{
	IResource& resource = m_rm.GetResource(shader);
	assert(resource.GetType() == ResourceType::Shader);

	Shader& resourceShader = static_cast<Shader&>(resource);
	glUseProgram(resourceShader.id);
	glUniform1f(resourceShader.uniforms[location],value);
}

void oglRenderer::SetShaderValue(const std::string& shader, const string& location, const glm::vec2& value )
{
	IResource& resource = m_rm.GetResource(shader);
	assert(resource.GetType() == ResourceType::Shader);

	Shader& resourceShader = static_cast<Shader&>(resource);
	glUseProgram(resourceShader.id);
	glUniform2f(resourceShader.uniforms[location],value.x,value.y);
}

void oglRenderer::SetCamera(Camera* pCam)
{
	//m_pFonts->SetCamera(pCam);
	m_pLines->SetCamera(pCam);
	m_pSprites->SetCamera(pCam);

	/*if(m_pCamera != nullptr)
	{
		m_pCamera->Release();
		m_pCamera = pCam;
	}*/


}

