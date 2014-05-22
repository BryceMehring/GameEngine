#include "oglRenderer.h"
#include "VertexStructures.h"
#include "../common/Log.h"

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
const std::string oglRenderer::s_videoModeFile = "VideoModes.txt";

void APIENTRY OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
	std::ostringstream stream;
	stream << "OpenGL Error" << endl << "Source: ";

	switch(source)
	{
		case GL_DEBUG_SOURCE_API: stream << "API"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: stream << "GLSL compiler"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: stream << "Windowing system"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: stream << "External Libraries"; break;
		case GL_DEBUG_SOURCE_APPLICATION: stream << "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: stream << "Other"; break;
		default: stream << "Unknown"; break;
	}

	stream << endl << "Type: ";

	switch(type)
	{
		case GL_DEBUG_TYPE_ERROR: stream << "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: stream << "Deprecated"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: stream << "Undefined"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: stream << "Performance"; break;
		case GL_DEBUG_TYPE_PORTABILITY: stream << "Portability"; break;
		case GL_DEBUG_TYPE_OTHER: stream << "Other"; break;
		default: stream << "Unknown"; break;
	}

	stream << endl << "Severity: ";
	
	switch(severity)
	{
		case GL_DEBUG_SEVERITY_LOW: stream << "low"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: stream << "medium"; break;
		case GL_DEBUG_SEVERITY_HIGH: stream << "high"; break;
		default: stream << "Unknown"; break;
	}

	stream << endl << "Message: " << message;

	Log::Instance().Write(stream.str());

	if(severity ==  GL_DEBUG_SEVERITY_HIGH)
	{
		abort();
	}
}

oglRenderer::oglRenderer() : m_pWorldCamera(nullptr), m_pWindow(nullptr), m_pWorldSpaceFonts(nullptr),
m_pScreenSpaceFonts(nullptr), m_pWorldSpaceLines(nullptr), m_pScreenSpaceLines(nullptr),
m_pWorldSpaceSprites(nullptr), m_pScreenSpaceSprites(nullptr), m_pMonitors(nullptr), m_iMonitorCount(0),
m_iCurrentMonitor(0), m_iCurrentDisplayMode(0), m_renderSpace(RenderSpace::Screen), m_bFullscreen(false)
{
	s_pThis = this;

	EnumerateDisplayAdaptors();
	ParseVideoSettingsFile();
	ConfigureGLFW();
	ConfigureOpenGL();
	BuildBuffers();
	BuildCamera();
	EnableVSync(true);
}

oglRenderer::~oglRenderer()
{
	for(auto iter : m_vertexBuffers)
	{
		delete iter;
	}

	SaveDisplayList();
	glfwDestroyWindow(m_pWindow);
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

void oglRenderer::DrawLine(const glm::vec3* pArray, unsigned int length, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	if (pArray != nullptr)
	{
		if (m_renderSpace == World)
		{
			m_pWorldSpaceLines->DrawLine(pArray, length, fWidth, color, T);
		}
		else
		{
			m_pScreenSpaceLines->DrawLine(pArray, length, fWidth, color, T);
		}
	}
}

void oglRenderer::DrawString(const char* str, const glm::vec3& pos, float scale, const glm::vec4& color, const char* font, FontAlignment alignment)
{
	if (str != nullptr)
	{
		if (m_renderSpace == World)
		{
			m_pWorldSpaceFonts->DrawString(str, font, pos, scale, color, alignment);
		}
		else
		{
			m_pScreenSpaceFonts->DrawString(str, font, pos, scale, color, alignment);
		}
	}
}

void oglRenderer::DrawSprite(const std::string& texture, const glm::mat4& transformation, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId, const std::string& tech)
{
	if(m_renderSpace == World)
	{
		m_pWorldSpaceSprites->DrawSprite(tech,texture,transformation,color,tiling,iCellId);
	}
	else
	{
		m_pScreenSpaceSprites->DrawSprite(tech,texture,transformation,color,tiling,iCellId);
	}
}

int oglRenderer::CreateCursor(const std::string& texture, int xhot, int yhot)
{
	Cursor* pTexture = static_cast<Cursor*>(m_rm.GetResource(texture, ResourceType::Cursor));
	if(pTexture != nullptr)
	{
		GLFWimage img;
		img.width = pTexture->GetWidth();
		img.height = pTexture->GetHeight();
		img.pixels = pTexture->GetImgData();

		GLFWcursor* pCursor = glfwCreateCursor(&img, xhot, yhot);
		if(pCursor != nullptr)
		{
			int index = m_cursors.size() + 1;
			m_cursors.emplace(index, pCursor);
			return index;
		}
	}

	return 0;
}
void oglRenderer::DestroyCursor(int cursor)
{
	auto iter = m_cursors.find(cursor);
	if(iter != m_cursors.end())
	{
		glfwDestroyCursor(iter->second);
		m_cursors.erase(iter);
	}
}
void oglRenderer::SetCursor(int cursor)
{
	GLFWcursor* pCursor = nullptr;

	if(cursor != 0)
	{
		auto iter = m_cursors.find(cursor);
		if(iter != m_cursors.end())
		{
			pCursor = iter->second;
		}
	}

	glfwSetCursor(m_pWindow, pCursor);
}

IResourceManager& oglRenderer::GetResourceManager()
{
	return m_rm;
}

bool oglRenderer::GetDisplayMode(int monitor, int i, int* width, int* height) const
{
	if (monitor >= m_iMonitorCount)
		return false;

	std::pair<const GLFWvidmode*,int> modes = m_videoModes[monitor];

	if (i >= modes.second)
		return false;

	int index = modes.second - i - 1;

	if (width != nullptr)
	{
		*width = modes.first[index].width;
	}

	if (height != nullptr)
	{
		*height = modes.first[index].height;
	}

	return true;
}

bool oglRenderer::GetDisplayMode(int* width, int* height, bool* vsync) const
{
	bool status = GetDisplayMode(m_iCurrentMonitor,m_iCurrentDisplayMode,width,height);

	if (vsync != nullptr)
	{
		*vsync = m_bVSync;
	}

	return status;
}

int oglRenderer::GetNumMonitors() const
{
	return m_iMonitorCount;
}

int oglRenderer::GetNumDisplayModes(int monitor) const
{
	return m_videoModes[monitor].second;
}

void oglRenderer::GetStringRect(const char* str, float scale, FontAlignment alignment, Math::FRECT& inout) const
{
	if (str != nullptr)
	{
		m_pScreenSpaceFonts->GetStringRect(str, scale, alignment, inout);
	}
}

void oglRenderer::SetCamera(PerspectiveCamera* pCam)
{
	m_pWorldCamera = pCam;
	m_pWorldSpaceFonts->SetCamera(pCam);
	m_pWorldSpaceLines->SetCamera(pCam);
	m_pWorldSpaceSprites->SetCamera(pCam);
}

void oglRenderer::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x,color.y,color.z,0.0f);
}

void oglRenderer::SetDisplayMode(int i)
{
	std::pair<const GLFWvidmode*, int> videoMode = m_videoModes[m_iCurrentMonitor];
	if (i < videoMode.second && i >= 0)
	{
		int index = videoMode.second - i - 1;
		glfwSetWindowSize(m_pWindow, videoMode.first[index].width, videoMode.first[index].height);
		UpdateCamera();

		m_iCurrentDisplayMode = i;
	}
}

void oglRenderer::SetRenderSpace(RenderSpace space)
{
	m_renderSpace = space;
}

void oglRenderer::SetShaderValue(const std::string& shader, const string& location, float value)
{
	Shader* pShader = static_cast<Shader*>(m_rm.GetResource(shader, ResourceType::Shader));
	if(pShader != nullptr)
	{
		pShader->Bind();
		pShader->SetValue(location,value);
		pShader->UnBind();
	}
}

void oglRenderer::SetShaderValue(const std::string& shader, const string& location, const glm::vec2& value)
{
	Shader* pShader = static_cast<Shader*>(m_rm.GetResource(shader, ResourceType::Shader));
	if(pShader != nullptr)
	{
		pShader->Bind();
		pShader->SetValue(location,value);
		pShader->UnBind();
	}
}

void oglRenderer::EnableVSync(bool enable)
{
	glfwSwapInterval(enable);
	m_bVSync = enable;
}

void oglRenderer::Present()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pWorldSpaceSprites->Render();
	m_pWorldSpaceLines->Render();
	m_pWorldSpaceFonts->Render();

	m_pScreenSpaceSprites->Render();
	m_pScreenSpaceLines->Render();
	m_pScreenSpaceFonts->Render();

	glfwSwapBuffers(m_pWindow);
}

void oglRenderer::MonitorCallback(GLFWmonitor* monitor, int state)
{
	s_pThis->EnumerateDisplayAdaptors();
}

void oglRenderer::GLFWOpenWindowHints()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef _WIN32
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifdef DEBUG_BUILD
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE);
#endif

	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
}

void oglRenderer::ConfigureGLFW()
{
	GLFWOpenWindowHints();

	bool bFullscreen = false;

#ifndef DEBUG_BUILD
	bFullscreen = m_bFullscreen;
#endif

	int width, height;
	GetDisplayMode(&width, &height);

	m_pWindow = glfwCreateWindow(width, height, "", bFullscreen ? m_pMonitors[m_iCurrentMonitor] : nullptr, nullptr);

	assert(m_pWindow != nullptr);

	glfwMakeContextCurrent(m_pWindow);
	glfwSetMonitorCallback(MonitorCallback);

	// Get the OpenGL version that we have created
	int major = glfwGetWindowAttrib(m_pWindow,GLFW_CONTEXT_VERSION_MAJOR);
	int minor = glfwGetWindowAttrib(m_pWindow,GLFW_CONTEXT_VERSION_MINOR);

	std::ostringstream stream;
	stream << "OpenGL Version: " << major << '.' << minor;
	Log::Instance().Write(stream.str());
}

void oglRenderer::ConfigureOpenGL()
{
	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	assert(glewInit() == GLEW_OK);

	// Check to make sure that the hardware is supported
	assert(glewIsSupported("GL_VERSION_3_3"));

#ifdef DEBUG_BUILD
	if (GLEW_ARB_debug_output)
	{
		glDebugMessageCallback(OpenGLErrorCallback,0);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void oglRenderer::EnumerateDisplayAdaptors()
{
	m_pMonitors = glfwGetMonitors(&m_iMonitorCount);

	m_videoModes.reserve(m_iMonitorCount);

	for (int i = 0; i < m_iMonitorCount; ++i)
	{
		int size = 0;
		const GLFWvidmode* pVidMode = glfwGetVideoModes(m_pMonitors[i], &size);

		m_videoModes.emplace_back(pVidMode, size);
	}
}

void oglRenderer::ParseVideoSettingsFile()
{
	std::ifstream stream(s_videoModeFile);
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
			else if (!line.empty() && ((line.back() == 'W') || (line.back() == 'F')))
			{
				m_iCurrentMonitor = iMonitorCounter - 1;
				m_iCurrentDisplayMode = iDisplayCounter - 1;

				m_bFullscreen = (line.back() == 'F');

				break;
			}

			iDisplayCounter++;
		}

		stream.close();
	}
}

void oglRenderer::SaveDisplayList()
{
	std::ofstream stream(s_videoModeFile);

	if(stream.is_open())
	{
		for(int i = 0; i < (int)m_videoModes.size(); ++i)
		{
			stream << "Monitor " << i << endl;

			const GLFWvidmode* pModes = m_videoModes[i].first;

			for(int j = m_videoModes[i].second - 1; j >= 0; --j)
			{
				int index = m_videoModes[i].second - j - 1;
				stream << index << ": " << pModes[j].width << ' ' << pModes[j].height << ' ' << pModes[j].refreshRate;

				if(i == m_iCurrentMonitor && index == m_iCurrentDisplayMode)
				{
					stream << ' ' << (m_bFullscreen ? 'F' : 'W');
				}

				stream << endl;
			}

			stream << endl;
		}

		stream.close();
	}

}

void oglRenderer::BuildBuffers()
{
	const unsigned char indexBuffer[6] = { 0, 2, 1,	2, 3, 1 };

	VertexPT verticies[] =
	{
		{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0)},
		{glm::vec3(-0.5f, -0.5f, 0.0), glm::vec2(0,1)},
		{glm::vec3(0.5f, 0.5f, 0.0), glm::vec2(1,0)},
		{glm::vec3(0.5f, -0.5f, 0.0), glm::vec2(1)}
	};

	VertexBuffer* pSpriteVertexBuffer = new VertexBuffer(verticies, sizeof(VertexPT), 4, GL_STATIC_DRAW,indexBuffer, 6);
	VertexBuffer* pLineVertexBuffer = new VertexBuffer(0, sizeof(VertexP), 1024*8, GL_DYNAMIC_DRAW, indexBuffer, 6, false);

	m_pWorldSpaceFonts.reset(new FontEngine(&m_rm,pSpriteVertexBuffer));
	m_pScreenSpaceFonts.reset(new FontEngine(&m_rm,pSpriteVertexBuffer,&m_OrthoCamera));

	m_pWorldSpaceLines.reset(new LineEngine(&m_rm,pLineVertexBuffer));
	m_pScreenSpaceLines.reset(new LineEngine(&m_rm,pLineVertexBuffer,&m_OrthoCamera));

	m_pWorldSpaceSprites.reset(new SpriteEngine(&m_rm,pSpriteVertexBuffer));
	m_pScreenSpaceSprites.reset(new SpriteEngine(&m_rm,pSpriteVertexBuffer,&m_OrthoCamera));

	m_vertexBuffers.push_back(pLineVertexBuffer);
	m_vertexBuffers.push_back(pSpriteVertexBuffer);
}

void oglRenderer::BuildCamera()
{
	int width, height;
	glfwGetFramebufferSize(m_pWindow, &width, &height);

	m_OrthoCamera.LookAt(glm::vec3(0.0f,0.0f,2.0f));
	m_OrthoCamera.SetLens(0.0f, (float)width, (float)height, 0.1f, 5000.0f);
	m_OrthoCamera.Update();
}

void oglRenderer::UpdateCamera()
{
	int width, height;
	glfwGetFramebufferSize(m_pWindow, &width, &height);

	m_OrthoCamera.UpdateAspectRatio((float)width, (float)height);
	m_OrthoCamera.Update();

	if (m_pWorldCamera != nullptr)
	{
		m_pWorldCamera->UpdateAspectRatio((float)width, (float)height);
		m_pWorldCamera->Update();
	}

	glViewport(0, 0, width, height);
}


