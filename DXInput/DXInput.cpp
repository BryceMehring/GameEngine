
#define GLFW_DLL

#include <GLFW/glfw3.h>
#include <angelscript.h>
#include "DXInput.h"

#include <iostream>

using namespace std;

// Input plug-in implementation
extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new DirectInput();
}

DirectInput* DirectInput::s_pThis = nullptr;

void DirectInput::CharCallback(GLFWwindow*, unsigned int c)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iCharKeyDown = c;
	}
}

void DirectInput::KeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iKeyDown = key;
		s_pThis->m_iKeyAction = action;
	}
}

void DirectInput::MouseCallback(GLFWwindow*, double x, double y)
{
	if(s_pThis != nullptr)
	{
		s_pThis->UpdateMouse(x,y);
	}
}

void DirectInput::MouseButtonCallback(GLFWwindow*,int button, int action, int mods)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_selectedPos = s_pThis->m_tpos;
		s_pThis->m_MouseClickOnce[button] = action;
	}
}

// DirectInput ctor
DirectInput::DirectInput() : m_iMouseX(0), m_iMouseY(0), m_fMouseSensistivity(100.0f), m_tpos(0.0f,0.0f)
{
	s_pThis = this;

	Poll();

	glfwSetCharCallback(glfwGetCurrentContext(),CharCallback);
	glfwSetKeyCallback(glfwGetCurrentContext(),KeyCallback);
	glfwSetCursorPosCallback(glfwGetCurrentContext(),MouseCallback);
	glfwSetMouseButtonCallback(glfwGetCurrentContext(),MouseButtonCallback);

	glfwSetInputMode(glfwGetCurrentContext(),GLFW_CURSOR,GL_FALSE);
	glfwSetInputMode(glfwGetCurrentContext(),GLFW_STICKY_KEYS,GL_TRUE);

	CenterMouse();
}

void DirectInput::Init(asIScriptEngine* pAS)
{
	RegisterScript(pAS);
}

void DirectInput::Destroy(asIScriptEngine* pAS)
{
	// remove config group from script
	pAS->RemoveConfigGroup("Input");
}

int DirectInput::GetVersion() const
{
	return 0;
}

DLLType DirectInput::GetPluginType() const
{
	return DLLType::Input;
}

void DirectInput::CenterMouse()
{
	int width;
	int height;

	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);
	glfwSetCursorPos(glfwGetCurrentContext(),width / 2, height / 2);
}

void DirectInput::Poll()
{
	m_MouseClickOnce[0] = m_MouseClickOnce[1] = -1;
	m_iKeyAction = m_iKeyDown = -1;
	m_iCharKeyDown = -1;

	glfwPollEvents();
}

void DirectInput::UpdateMouse(double x, double y)
{
	int width = 0;
	int height = 0;

	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);

	m_iMouseX = x - (width / 2);
	m_iMouseY = -y + (height / 2);

	m_tpos += m_fMouseSensistivity * glm::vec2(m_iMouseX / (float)width ,m_iMouseY / (float)height);

	CenterMouse();
	ClampMouse();
}

void DirectInput::ClampMouse()
{
	if(m_tpos.x < -100.0f)
	{
		m_tpos.x = -100.0f;
	}
	else if(m_tpos.x > 100.0f)
	{
		m_tpos.x = 100.0f;
	}

	if(m_tpos.y < -100.0f)
	{
		m_tpos.y = -100.0f;
	}
	else if(m_tpos.y > 100.0f)
	{
		m_tpos.y = 100.0f;
	}
}

void DirectInput::CursorPos(double& x, double& y) const
{
	glfwGetCursorPos(glfwGetCurrentContext(),&x,&y);
}

const glm::vec2& DirectInput::GetTransformedMousePos() const
{
	return m_tpos;
}

bool DirectInput::KeyDown(int Key, bool once)
{
	return once ? (m_iKeyAction == GLFW_PRESS && m_iKeyDown == Key) : (glfwGetKey(glfwGetCurrentContext(), Key ) == GLFW_PRESS);
}
bool DirectInput::KeyUp(int Key, bool once)
{
	return once ? (m_iKeyAction == GLFW_RELEASE && m_iKeyDown == Key) : (glfwGetKey(glfwGetCurrentContext(), Key ) == GLFW_RELEASE);
}
bool DirectInput::CharKeyDown(char& out) const
{
	if(m_iCharKeyDown == -1)
		return false;

	out = m_iCharKeyDown;

	return true;
}

bool DirectInput::MouseClick(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_PRESS) : glfwGetMouseButton(glfwGetCurrentContext(),iButton) == GLFW_PRESS);
}
bool DirectInput::MouseRelease(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_RELEASE) : glfwGetMouseButton(glfwGetCurrentContext(),iButton) == GLFW_RELEASE);
}
int DirectInput::MouseX() const
{
	return m_iMouseX;
}

int DirectInput::MouseY() const
{
	return m_iMouseY;
}

int DirectInput::MouseZ() const
{
	return 0;
	//return glfwGetMouseWheel();
}

bool DirectInput::GetSelectedRect(Math::AABB& out)
{
	if(!MouseClick(0,false))
		return false;

	out.min = glm::vec2(min(m_selectedPos.x,m_tpos.x),min(m_selectedPos.y,m_tpos.y));
	out.max = glm::vec2(max(m_selectedPos.x,m_tpos.x),max(m_selectedPos.y,m_tpos.y));

	return true;
}

void DirectInput::SetCursorSensitivity(float s)
{
	m_fMouseSensistivity = s;
}

void DirectInput::RegisterScript(asIScriptEngine* pAS)
{
	pAS->BeginConfigGroup("Input");

	(pAS->RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	(pAS->RegisterObjectMethod("IKMInput","int mouseX()",asMETHOD(DirectInput,MouseX),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	(pAS->RegisterGlobalProperty("IKMInput input",this));

	pAS->EndConfigGroup();
}
