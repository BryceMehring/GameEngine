#include "oglRenderer.h"
#include "FontRenderer.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include "oglCallback.h"
#include "Log.h"

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

void oglRenderer::IconifyCallback(GLFWwindow* window, int flag)
{
	if (s_pThis != nullptr)
	{
		s_pThis->m_bIconify = flag;
	}
}

oglRenderer::oglRenderer() : m_pWorldCamera(nullptr), m_pWindow(nullptr), m_pWorldSpaceSprites(nullptr), m_pScreenSpaceSprites(nullptr),
m_pMonitors(nullptr), m_iMonitorCount(0), m_iCurrentMonitor(0), m_iCurrentDisplayMode(0), m_renderSpace(RenderSpace::Screen), m_bFullscreen(true)
{
	s_pThis = this;
	m_iClearBits = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;

	ParseVideoSettingsFile();
	EnumerateDisplayAdaptors();
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
	if (m_renderSpace == World)
	{
		m_pWorldSpaceSprites->DrawLine(pArray, length, fWidth, color, T);
	}
	else
	{
		m_pScreenSpaceSprites->DrawLine(pArray, length, fWidth, color, T);
	}
}

void oglRenderer::DrawCircle(const glm::vec3 &center, float radius, float thickness, unsigned int segments, const glm::vec4 &color)
{
	float delta = 361.0f / (segments - 1);
	std::vector<glm::vec3> line(segments);
	for(unsigned int i = 0; i < segments; ++i)
	{
		float angle = glm::radians(delta * i);

		line[i] = radius * glm::vec3(cos(angle), sin(angle), 0.0f) + center;
	}

	DrawLine(line.data(), segments, thickness, color);
}

void oglRenderer::DrawString(const char* str, const glm::vec3& pos, const glm::vec4& color, float scale, const char* font, FontAlignment alignment)
{
	if (m_renderSpace == World)
	{
		m_pWorldSpaceSprites->DrawString(str, font, pos, scale, color, alignment);
	}
	else
	{
		m_pScreenSpaceSprites->DrawString(str, font, pos, scale, color, alignment);
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

void oglRenderer::DrawSprite(const glm::mat4& transformation, const glm::vec4& color, const glm::vec2& tiling, unsigned int iCellId, const std::string& tech)
{
	if (m_renderSpace == World)
	{
		m_pWorldSpaceSprites->DrawSprite(tech, "blank", transformation, color, tiling, iCellId);
	}
	else
	{
		m_pScreenSpaceSprites->DrawSprite(tech, "blank", transformation, color, tiling, iCellId);
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
		img.pixels = const_cast<unsigned char*>(pTexture->GetImgData());

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

float oglRenderer::ReadPixels(const glm::ivec2 &pos) const
{
	float depth = 0.0f;
	glReadPixels(pos.x, pos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

	return depth;
}

const GLFWvidmode* oglRenderer::GetDisplayMode() const
{
	return GetDisplayMode(m_iCurrentMonitor, m_iCurrentDisplayMode);
}

const GLFWvidmode* oglRenderer::GetDisplayMode(int monitor, int mode) const
{
	if ((monitor >= m_iMonitorCount) || (monitor < 0))
		return nullptr;

	std::pair<const GLFWvidmode*,int> modes = m_videoModes[monitor];

	if ((mode >= modes.second) || (mode < 0))
		return nullptr;

	return modes.first + (modes.second - mode - 1);
}

bool oglRenderer::GetDisplayMode(int monitor, int i, int* width, int* height) const
{
	const GLFWvidmode* pMode = GetDisplayMode(monitor, i);

	if(pMode == nullptr)
		return false;

	if (width != nullptr)
	{
		*width = pMode->width;
	}

	if (height != nullptr)
	{
		*height = pMode->height;
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
	// todo: fix this
	const Font* pFont = static_cast<const Font*>(m_rm.GetResource("font", ResourceType::Font));
	if(pFont != nullptr)
	{
		FontRenderable::GetStringRect(str, pFont, scale, alignment, inout);
	}
}

void oglRenderer::SetCamera(PerspectiveCamera* pCam)
{
	m_pWorldCamera = pCam;
	m_pWorldSpaceSprites->SetCamera(pCam);
}

void oglRenderer::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x,color.y,color.z,0.0f);
}

void oglRenderer::EnableColorClearing(bool bEnable)
{
	m_iClearBits = GL_DEPTH_BUFFER_BIT;
	if (bEnable == true)
	{
		m_iClearBits |= GL_COLOR_BUFFER_BIT;
	}
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
	ApplyShader pShader = static_cast<Shader*>(m_rm.GetResource(shader, ResourceType::Shader));
	pShader->SetValue(location, value);
}

void oglRenderer::SetShaderValue(const std::string& shader, const string& location, const glm::vec2& value)
{
	ApplyShader pShader = static_cast<Shader*>(m_rm.GetResource(shader, ResourceType::Shader));
	pShader->SetValue(location, value);
}

void oglRenderer::EnableVSync(bool enable)
{
	glfwSwapInterval(enable);
	m_bVSync = enable;
}

bool oglRenderer::IsIconified() const
{
	return m_bIconify;
}

void oglRenderer::Present()
{
	glClear(m_iClearBits);

	m_pWorldSpaceSprites->Render();
	m_pScreenSpaceSprites->Render();

	glfwSwapBuffers(m_pWindow);
}

void oglRenderer::MonitorCallback(GLFWmonitor* monitor, int state)
{
	s_pThis->EnumerateDisplayAdaptors();
}

void oglRenderer::ConfigureGLFW()
{
	// todo: need to clean this up

	bool bFullscreen = false;

#ifndef DEBUG_BUILD
	bFullscreen = m_bFullscreen;
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE);
	Log::Instance().Write("Debug OpenGL Context");
#endif

	const GLFWvidmode* pVideoMode = GetDisplayMode();
	assert(pVideoMode != nullptr);

	if(bFullscreen)
	{
		glfwWindowHint(GLFW_RED_BITS, pVideoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, pVideoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, pVideoMode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, pVideoMode->refreshRate);
	}

	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);

	m_pWindow = glfwCreateWindow(pVideoMode->width, pVideoMode->height, "", bFullscreen ? m_pMonitors[m_iCurrentMonitor] : nullptr, nullptr);
	if(m_pWindow == nullptr)
	{
		throw std::string("Failed to create window");
	}

	glfwMakeContextCurrent(m_pWindow);
	glfwSetMonitorCallback(MonitorCallback);
	glfwSetWindowIconifyCallback(m_pWindow, IconifyCallback);

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
	if(glewInit() != GLEW_OK)
	{
		throw std::string("Cannot initialize GLEW");
	}

	// Check to make sure that the hardware is supported
	if(glewIsSupported("GL_VERSION_3_3") == false)
	{
		throw std::string("OpenGL 3.3 is not supported");
	}

	// Check for ARB_debug_output
	std::ostringstream stream;
	stream << "ARB_debug_output ";

	if(GLEW_ARB_debug_output)
	{
		stream << "supported ";
#ifdef DEBUG_BUILD
		stream << "and enabled";
		glDebugMessageCallback(OpenGLErrorCallback, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#else
		stream << "and disabled";
#endif
	}
	else
	{
		stream << "not supported";
	}
	Log::Instance().Write(stream.str());

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
}

void oglRenderer::EnumerateDisplayAdaptors()
{
	GLFWmonitor* pPrimaryMonitor = glfwGetPrimaryMonitor();
	int iPrimaryMontior = 0;

	m_pMonitors = glfwGetMonitors(&m_iMonitorCount);
	m_videoModes.reserve(m_iMonitorCount);

	if(m_iCurrentMonitor >= m_iMonitorCount)
	{
		m_bFirstRun = true;
	}

	for (int i = 0; i < m_iMonitorCount; ++i)
	{
		int size = 0;
		const GLFWvidmode* pVidMode = glfwGetVideoModes(m_pMonitors[i], &size);
		m_videoModes.emplace_back(pVidMode, size);

		// Check if this is the current monitor previously saved, and if the entry is invalid
		if(m_iCurrentMonitor == i && m_iCurrentDisplayMode >= size)
		{
			m_bFirstRun = true;
		}

		// Check if the current monitor is the default monitor
		if(m_pMonitors[i] == pPrimaryMonitor)
		{
			iPrimaryMontior = i;
		}
	}

	// If this is the first run, or the VideoModes.txt file became corrupted, then use the default desktop resolution on the default desktop
	if(m_bFirstRun)
	{
		const GLFWvidmode* pCurrentVideoMode = glfwGetVideoMode(pPrimaryMonitor);
		const GLFWvidmode* pVidModes = m_videoModes[iPrimaryMontior].first;
		int size = m_videoModes[iPrimaryMontior].second;

		for(int i = size - 1; i >= 0; --i)
		{
			if(pCurrentVideoMode->width == pVidModes[i].width &&
			   pCurrentVideoMode->height == pVidModes[i].height &&
			   pCurrentVideoMode->redBits == pVidModes[i].redBits &&
			   pCurrentVideoMode->greenBits == pVidModes[i].greenBits &&
			   pCurrentVideoMode->blueBits == pVidModes[i].blueBits &&
			   pCurrentVideoMode->refreshRate == pVidModes[i].refreshRate)
			{
				m_iCurrentDisplayMode = size - i - 1;
				break;
			}
		}

		m_iCurrentMonitor = iPrimaryMontior;
	}
}

void oglRenderer::ParseVideoSettingsFile()
{
	std::ifstream stream(s_videoModeFile);
	int iMonitorCounter = 0;
	int iDisplayCounter = 0;

	if(!stream.is_open())
		return;

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
			m_bFirstRun = false;

			break;
		}

		iDisplayCounter++;
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

	m_pWorldSpaceSprites.reset(new AbstractRenderer(&m_rm, pSpriteVertexBuffer));
	m_pScreenSpaceSprites.reset(new AbstractRenderer(&m_rm, pSpriteVertexBuffer,&m_OrthoCamera));

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


