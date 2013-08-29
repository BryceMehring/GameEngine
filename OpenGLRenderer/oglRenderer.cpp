#include "oglRenderer.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new oglRenderer();
}

oglRenderer::oglRenderer() : m_pCamera(nullptr), m_pWindow(nullptr), m_pFonts(nullptr), m_pLines(nullptr), m_pSprites(nullptr), m_uiCurrentDisplayMode(0), m_bFullscreen(false)
{
	m_pCamera = CreateCamera();
	m_pCamera->setLens(200.0f,200.0f,0.0f,5.0f);
	m_pCamera->lookAt(glm::vec3(0.0f,0.0f,1000.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	m_pCamera->update(0.0f);

	EnumerateDisplayAdaptors();

	GLFWOpenWindowHints();
	m_pWindow = glfwCreateWindow(m_pVideoModes[m_iNumVideoModes - 1].width, m_pVideoModes[m_iNumVideoModes - 1].height, "", NULL, NULL);
	if(m_pWindow == nullptr)
	{
		glfwTerminate();
		throw std::string("Failed to create window");
	}

	glfwMakeContextCurrent(m_pWindow);

	EnableVSync(false);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(1.0f,1.0f,1.0f,0.0f);

	// Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		throw std::string("Failed to initialize GLEW");
	}

	m_rm.LoadResourceFile("base.r");

	m_pFonts.reset(new FontEngine(&m_rm,1024*8,m_pCamera));
	m_pLines.reset(new LineEngine(&m_rm,1024*20,m_pCamera));
	m_pSprites.reset(new SpriteEngine(&m_rm,1024*20,m_pCamera));

}

oglRenderer::~oglRenderer()
{
	m_pCamera->Release();
	m_pCamera = nullptr;

	glfwDestroyWindow(m_pWindow);
}

void oglRenderer::Init(asIScriptEngine* pAS)
{
	pAS->BeginConfigGroup("renderer");
	(pAS->RegisterObjectType("renderer",0,asOBJ_REF | asOBJ_NOHANDLE));
	//(pAS->RegisterObjectMethod("renderer","int mouseX()",asMETHOD(oglRenderer,MouseX),asCALL_THISCALL));
	//(pAS->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	//(pAS->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	//(pAS->RegisterGlobalProperty("IKMInput input",this));
	// todo: implement this later

	pAS->EndConfigGroup();
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
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
}

void oglRenderer::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void oglRenderer::Present()
{
	ClearScreen();

	m_pSprites->Render();
	m_pFonts->Render();
	m_pLines->Render();


	glfwSwapBuffers(m_pWindow);
}

void oglRenderer::EnableVSync(bool enable)
{
#ifdef _WIN32
	// Turn on vertical screen sync under Windows.
	// (I.e. it uses the WGL_EXT_swap_control extension)
	typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT)
		wglSwapIntervalEXT(enable ? 1 : 0);
#else
	glfwSwapInterval(enable ? 1 : 0);
#endif


}

void oglRenderer::EnumerateDisplayAdaptors()
{
	m_pVideoModes = glfwGetVideoModes(glfwGetPrimaryMonitor(),&m_iNumVideoModes);

	m_VideoModeStr.reserve(m_iNumVideoModes);

	for(int i = 0; i < m_iNumVideoModes; ++i)
	{
		std::stringstream stream;
		stream << m_pVideoModes[i].width << 'x' << m_pVideoModes[i].height;

		m_VideoModeStr.push_back(stream.str());
	}
}

int oglRenderer::GetNumDisplayModes() const
{
	return m_iNumVideoModes;
}

int oglRenderer::GetCurrentDisplayMode() const
{
	return m_uiCurrentDisplayMode;
}

void oglRenderer::SetDisplayMode(int i)
{
	if(i < GetNumDisplayModes() && i >= 0)
	{
		glfwSetWindowSize(m_pWindow,m_pVideoModes[i].width,m_pVideoModes[i].height);
	}
}

const std::string& oglRenderer::GetDisplayModeStr(int i) const
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


void oglRenderer::DrawString(const char* str, const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& color, const char* font, FontAlignment options)
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

void oglRenderer::DrawSprite(const std::string& texture, const glm::mat4& transformation, const glm::vec2& tiling, unsigned int iCellId, const std::string& tech)
{
	m_pSprites->DrawSprite(tech,texture,transformation,tiling,iCellId);
}

bool oglRenderer::CheckShader(const std::string& shader, const string& location,  GLuint& shaderID, GLuint& outLocation) const
{
	const IResource* pResource = m_rm.GetResource(shader);

	if(pResource == nullptr)
		return false;

	auto type = pResource->GetType();

	if((type != ResourceType::Shader) && (type != ResourceType::TexturedShader))
		return false;

	const Shader* pShader = static_cast<const Shader*>(pResource);
	const auto& unifromMap = pShader->GetUniforms();

	auto iterLocation = unifromMap.find(location);
	if(iterLocation == unifromMap.end())
		return false;

	shaderID = pShader->GetID();
	outLocation = iterLocation->second;

	return true;
}

bool oglRenderer::SetShaderValue(const std::string& shader, const string& location, float value )
{
	GLuint shaderID;
	GLuint unifromLocation;
	bool bSuccess = CheckShader(shader,location,shaderID,unifromLocation);

	if(bSuccess)
	{
		glUseProgram(shaderID);
		glUniform1f(unifromLocation,value);
	}

	return bSuccess;
}

bool oglRenderer::SetShaderValue(const std::string& shader, const string& location, const glm::vec2& value )
{
	GLuint shaderID;
	GLuint unifromLocation;
	bool bSuccess = CheckShader(shader,location,shaderID,unifromLocation);

	if(bSuccess)
	{
		glUseProgram(shaderID);
		glUniform2f(unifromLocation,value.x,value.y);
	}

	return bSuccess;
}

void oglRenderer::SetCamera(Camera* pCam)
{
	m_pFonts->SetCamera(pCam);
	m_pLines->SetCamera(pCam);
	m_pSprites->SetCamera(pCam);

	/*if(m_pCamera != nullptr)
	{
		m_pCamera->Release();
		m_pCamera = pCam;
	}*/


}

