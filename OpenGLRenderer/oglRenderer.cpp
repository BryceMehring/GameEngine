﻿#include "oglRenderer.h"
#include "VertexStructures.h"

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

oglRenderer::oglRenderer() : m_pWindow(nullptr), m_pWorldSpaceFonts(nullptr), m_pScreenSpaceFonts(nullptr),
	m_pWorldSpaceLines(nullptr), m_pScreenSpaceLines(nullptr), m_pWorldSpaceSprites(nullptr), m_pScreenSpaceSprites(nullptr), m_pMonitors(nullptr), m_iMonitorCount(0),
	m_iCurrentMonitor(0), m_iCurrentDisplayMode(0), m_renderSpace(RenderSpace::World), m_bFullscreen(false)
{
	s_pThis = this;

	EnumerateDisplayAdaptors();
	ParseVideoSettingsFile();
	ConfigureGLFW();
	ConfigureOpenGL();
	BuildCamera();
	EnableVSync(true);

	IndexedVertexBuffer* pSpriteVertexBuffer = new IndexedVertexBuffer(sizeof(VertexPTC),1024*40);
	IndexedVertexBuffer* pFontVertexBuffer = new IndexedVertexBuffer(sizeof(VertexPTC),1024*8);
	VertexBuffer* pLineVertexBuffer = new VertexBuffer(sizeof(VertexPC),1024*8*4);

	m_pWorldSpaceFonts.reset(new FontEngine(&m_rm,pFontVertexBuffer));
	m_pScreenSpaceFonts.reset(new FontEngine(&m_rm,pFontVertexBuffer,&m_OrthoCamera));

	m_pWorldSpaceLines.reset(new LineEngine(&m_rm,pLineVertexBuffer,World));
	m_pScreenSpaceLines.reset(new LineEngine(&m_rm,pLineVertexBuffer,Screen,&m_OrthoCamera));

	m_pWorldSpaceSprites.reset(new SpriteEngine(&m_rm,pSpriteVertexBuffer));
	m_pScreenSpaceSprites.reset(new SpriteEngine(&m_rm,pSpriteVertexBuffer,&m_OrthoCamera));

	m_vertexBuffers.push_back(pFontVertexBuffer);
	m_vertexBuffers.push_back(pLineVertexBuffer);
	m_vertexBuffers.push_back(pSpriteVertexBuffer);
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

void oglRenderer::Init(asIScriptEngine* pAS) {}

void oglRenderer::Destroy(asIScriptEngine* pAS) {}

void oglRenderer::DrawLine(const glm::vec3* pArray, unsigned int length, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	if(m_renderSpace == World)
	{
		m_pWorldSpaceLines->DrawLine(pArray,length,fWidth,color,T);
	}
	else
	{
		m_pScreenSpaceLines->DrawLine(pArray,length,fWidth,color,T);
	}
}

void oglRenderer::DrawString(const char* str, const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& color, const char* font, FontAlignment options)
{
	if(m_renderSpace == World)
	{
		m_pWorldSpaceFonts->DrawString(str,font,pos,scale,color,options);
	}
	else
	{
		m_pScreenSpaceFonts->DrawString(str,font,pos,scale,color,options);
	}
}

void oglRenderer::DrawSprite(const std::string& texture, const glm::mat4& transformation, const glm::vec3& color, const glm::vec2& tiling, unsigned int iCellId, const std::string& tech)
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

IResourceManager& oglRenderer::GetResourceManager()
{
	return m_rm;
}

bool oglRenderer::GetDisplayMode(int monitor, int i, int& width, int& height) const
{
	auto monitorModes = m_videoModes[monitor];

	if(i >= monitorModes.second)
		return false;

	int index = monitorModes.second - i - 1;

	width = monitorModes.first[index].width;
	height = monitorModes.first[index].height;

	return true;
}

bool oglRenderer::GetDisplayMode(int& width, int& height) const
{
	return GetDisplayMode(m_iCurrentMonitor,m_iCurrentDisplayMode,width,height);
}

int oglRenderer::GetNumMonitors() const
{
	return m_iMonitorCount;
}

int oglRenderer::GetNumDisplayModes(int monitor) const
{
	return m_videoModes[monitor].second;
}

void oglRenderer::GetLineWidthRange(float& min, float& max) const
{
	m_pScreenSpaceLines->GetLineWidthRange(min, max);
}

void oglRenderer::GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	//m_pFonts->GetStringRec(str,scale,out);
}

void oglRenderer::SetCamera(Camera* pCam)
{
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
	auto pairedVideoMode = m_videoModes[m_iCurrentMonitor];
	if(i < pairedVideoMode.second && i >= 0)
	{
		glfwSetWindowSize(m_pWindow,pairedVideoMode.first[i].width,pairedVideoMode.first[i].height);
		BuildCamera();
	}
}

void oglRenderer::SetRenderSpace(RenderSpace space)
{
	m_renderSpace = space;
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

void oglRenderer::EnableVSync(bool enable)
{
	glfwSwapInterval(enable ? 1 : 0);
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

void oglRenderer::ConfigureGLFW()
{
	GLFWOpenWindowHints();

	auto iter = m_videoModes[m_iCurrentMonitor];
	m_pWindow = glfwCreateWindow(iter.first[iter.second - m_iCurrentDisplayMode - 1].width, // width
											iter.first[iter.second - m_iCurrentDisplayMode - 1].height, // height
											"", // window title
											NULL, // monitor to display on
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

	//glFrontFace(GL_CW);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
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

void oglRenderer::GLFWOpenWindowHints()
{
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
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
					stream << " *";
				}

				stream << endl;
			}

			stream << endl;
		}

		stream.close();
	}

}

void oglRenderer::BuildCamera()
{
	int width, height;
	glfwGetFramebufferSize(m_pWindow,&width, &height);

	m_OrthoCamera.setLens((float)width,(float)height,0.1f,5000.0f); // 2.0f, 2.0f
	m_OrthoCamera.lookAt(glm::vec3(0.0f,0.0f,2.0f),glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));
	m_OrthoCamera.update();
}


