#include "oglRenderer.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new oglRenderer();
}

oglRenderer* oglRenderer::s_pThis = nullptr;

void oglRenderer::MonitorCallback(GLFWmonitor* monitor, int state)
{
	s_pThis->EnumerateDisplayAdaptors();
}

oglRenderer::oglRenderer() : m_pCamera(nullptr), m_pWindow(nullptr), m_pFonts(nullptr),
	m_pLines(nullptr), m_pSprites(nullptr), m_pMonitors(nullptr), m_iMonitorCount(0),
	m_iCurrentMonitor(0), m_iCurrentDisplayMode(0), m_bFullscreen(false)
{
	s_pThis = this;

	ParseVideoSettingsFile();
	ConfigureGLFW();
	ConfigureOpenGL();
	EnableVSync(true);

	m_rm.LoadResourceFile("base.r");

	m_pFonts.reset(new FontEngine(&m_rm,1024*8,m_pCamera));
	m_pLines.reset(new LineEngine(&m_rm,1024*8,m_pCamera));
	m_pSprites.reset(new SpriteEngine(&m_rm,1024*20,m_pCamera));

}

oglRenderer::~oglRenderer()
{
	SaveDisplayList();
	ReleaseCamera(m_pCamera);
	glfwDestroyWindow(m_pWindow);
}

void oglRenderer::ParseVideoSettingsFile()
{
	std::ifstream stream("videoModes.txt");
	int iMonitorCounter = 0;
	int iDisplayCounter = 0;

	if(stream.is_open())
	{
		std::string line;
		while(std::getline(stream,line))
		{
			std::istringstream inStream(line);

			string subLine;
			inStream >> subLine;

			if(subLine == "Monitor")
			{
				iMonitorCounter++;
				iDisplayCounter = 0;
			}
			else if(!line.empty() && line.back() == '*')
			{
				m_iCurrentMonitor = iMonitorCounter - 1;
				m_iCurrentDisplayMode = iDisplayCounter - 1;

				break;
			}

			iDisplayCounter++;
		}

		stream.close();
	}
}

void oglRenderer::SaveDisplayList()
{
	std::ofstream stream("videoModes.txt");

	if(stream.is_open())
	{
		for(unsigned int i = 0; i < m_videoModes.size(); ++i)
		{
			stream << "Monitor " << i << endl;

			const GLFWvidmode* pModes = m_videoModes[i].first;

			for(int j = m_videoModes[i].second - 1; j >= 0; --j)
			{
				int index = m_videoModes[i].second - j - 1;
				stream << index << ": " << pModes[j].width << ' ' << pModes[j].height << ' ' << pModes[j].refreshRate;

				if(i == m_iCurrentMonitor && index == m_iCurrentDisplayMode)
				{
					stream << " *";
				}

				stream << endl;
			}

			stream << endl;
		}

		stream.close();
	}

}

void oglRenderer::ConfigureGLFW()
{
	EnumerateDisplayAdaptors();
	GLFWOpenWindowHints();

	auto iter = m_videoModes[m_iCurrentMonitor];
	m_pWindow = glfwCreateWindow(iter.first[iter.second - m_iCurrentDisplayMode - 1].width, // width
											iter.first[iter.second - m_iCurrentDisplayMode - 1].height, // height
											"", // window title
											m_pMonitors[m_iCurrentMonitor], // monitor to display on
											NULL); // not used
	assert(m_pWindow != nullptr);

	glfwMakeContextCurrent(m_pWindow);
	glfwSetMonitorCallback(MonitorCallback);
}

void oglRenderer::ConfigureOpenGL()
{
	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	assert(glewInit() == GLEW_OK);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void oglRenderer::Init(asIScriptEngine* pAS) {}

void oglRenderer::Destroy(asIScriptEngine* pAS) {}

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

void oglRenderer::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x,color.y,color.z,0.0f);
}

void oglRenderer::Present()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(m_pCamera != nullptr)
	{
		m_pSprites->Render();
		m_pFonts->Render();
		m_pLines->Render();
	}

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
	m_pMonitors = glfwGetMonitors(&m_iMonitorCount);

	m_videoModes.resize(m_iMonitorCount);

	for(unsigned int i = 0; i < m_videoModes.size(); ++i)
	{
		int size = 0;
		const GLFWvidmode* pVidMode = nullptr;

		pVidMode = glfwGetVideoModes(m_pMonitors[i],&size);

		m_videoModes[i].first = pVidMode;
		m_videoModes[i].second = size;
	}
}

int oglRenderer::GetNumMonitors() const
{
	return m_iMonitorCount;
}

int oglRenderer::GetNumDisplayModes(int monitor) const
{
	return m_videoModes[monitor].second;
}

void oglRenderer::GetCurrentDisplayMode(int& monitor, int& mode) const
{
	monitor = m_iCurrentMonitor;
	mode = m_iCurrentDisplayMode;
}

void oglRenderer::SetDisplayMode(int i)
{
	auto pairedVideoMode = m_videoModes[m_iCurrentMonitor];
	if(i < pairedVideoMode.second && i >= 0)
	{
		glfwSetWindowSize(m_pWindow,pairedVideoMode.first[i].width,pairedVideoMode.first[i].height);
	}
}

bool oglRenderer::GetDisplayMode(int monitor, int i, int& width, int& height) const
{
	auto monitorModes = m_videoModes[monitor];

	if(i >= monitorModes.second)
		return false;

	width = monitorModes.first[i].width;
	height = monitorModes.first[i].height;

	return true;
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

void oglRenderer::DrawSprite(const std::string& texture, const glm::mat4& transformation, const glm::vec3& color, const glm::vec2& tiling, unsigned int iCellId, const std::string& tech)
{
	m_pSprites->DrawSprite(tech,texture,transformation,color,tiling,iCellId);
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

	if(m_pCamera != nullptr)
	{
		m_pCamera->Release();
	}

	m_pCamera = pCam;
}

