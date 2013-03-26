//#include <vld.h>
#include "oglRenderer.h"

#include <sstream>

#pragma comment(lib,"Game Engine.lib")

extern "C" PLUGINDECL IPlugin* CreatePlugin(asIScriptEngine* as)
{
	return new oglRenderer(as);
}

oglRenderer::oglRenderer(asIScriptEngine* as) : m_pCamera(CreateCamera()), m_uiCurrentDisplayMode(0), m_as(as), m_pFonts(nullptr), m_bFullscreen(false)
{
	m_pCamera->setLens(100.0f,100.0f,1.0f,500.0f);
	m_pCamera->lookAt(glm::vec3(0.0f,0.0f,1.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	m_pCamera->update(0.0f);

	GLFWOpenWindowHints();

	if(glfwOpenWindow( 800, 600, 8,8,8,8, 0,0, GLFW_WINDOW ) < 1 )
	{
		glfwTerminate();
		throw std::string("Failed to create window");
	}

	glfwSwapInterval(0);

	// Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if (glewInit() != GLEW_OK)
	{
		throw std::string("Failed to initialize GLEW");
	}

	/*GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);*/
	


	// Enable depth test
	//glEnable(GL_CULL_FACE);
//	glCullFace(GL_FRONT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	

	

	//glFrontFace(GL_CCW);

	// Enable depth test
	/*glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);*/

	m_pFonts = new FontEngine(&m_rm,1024,m_pCamera);
	m_pLines = new LineEngine(&m_rm,1024,m_pCamera);

	EnumerateDisplayAdaptors();

	//::glClearColor(1.0f,0.0f,0.0f,0.0f);
}

oglRenderer::~oglRenderer()
{
	delete m_pFonts;
	delete m_pLines;
	ReleaseCamera(m_pCamera);

	m_as->Release();
}

DLLType oglRenderer::GetPluginType() const
{
	return DLLType::RenderingPlugin;
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
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES,4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2); // We want OpenGL 2.1
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
}

void oglRenderer::ClearScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void oglRenderer::Present()
{
	ClearScreen();

	m_pFonts->Render();
	m_pLines->Render();

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
	m_bFullscreen = !m_bFullscreen;

	glfwCloseWindow();

	GLFWOpenWindowHints();

	if(m_bFullscreen)
	{
		glfwOpenWindow(m_VideoModes[m_uiCurrentDisplayMode].Width,
					   m_VideoModes[m_uiCurrentDisplayMode].Height,8,8,8,8,0,0,GLFW_WINDOW);
	}
	else
	{
		glfwOpenWindow(800,600,8,8,8,8,0,0,GLFW_WINDOW);
	}

}

IResourceManager& oglRenderer::GetResourceManager()
{
	return m_rm;
}

void oglRenderer::DrawString(const char* str, const glm::vec2& pos, const char* font, const glm::vec4& color, const glm::vec2& scale)
{
	m_pFonts->DrawString(str,font,pos,scale,color);
}

void oglRenderer::DrawLine(const glm::vec3* pArray, unsigned int length, const glm::vec4& color, const glm::mat4& T)
{
	m_pLines->DrawLine(pArray,length,color,T);
}

void oglRenderer::GetStringRec(const char* str, const glm::vec2& pos, const glm::vec2& scale, Math::FRECT& out)
{
	// todo: implement
}

void oglRenderer::DrawSprite(const glm::mat4& transformation, const std::string& texture, unsigned int iCellId, float dx, float dy)
{
	// todo: implement
}

void oglRenderer::SetCamera(class Camera*)
{
	// todo: implement
}

