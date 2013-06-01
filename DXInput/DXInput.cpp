
#define GLFW_DLL
#define GLFW_NO_GLU

#include <GL/glfw.h>
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

void GLFWCALL DirectInput::CharCallback(int c, int action)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iCharKeyDown = c;
		s_pThis->m_iCharAction = action;
	}
}

void GLFWCALL DirectInput::KeyCallback(int c, int action)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iKeyDown = c;
		s_pThis->m_iKeyAction = action;
	}
}

void GLFWCALL DirectInput::MouseCallback(int x, int y)
{
	if(s_pThis != nullptr)
	{
		s_pThis->UpdateMouse(x,y);
	}
}

void GLFWCALL DirectInput::MouseClickCallback(int button, int action)
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

	Reset();

	glfwSetCharCallback(CharCallback);
	glfwSetKeyCallback(KeyCallback);
	glfwSetMousePosCallback(MouseCallback);
	glfwSetMouseButtonCallback(MouseClickCallback);

	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwEnable(GLFW_MOUSE_CURSOR);
	glfwEnable(GLFW_KEY_REPEAT);
	glfwEnable(GLFW_STICKY_KEYS);

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
	return InputPlugin;
}

void DirectInput::CenterMouse()
{
	int width;
	int height;

	glfwGetWindowSize(&width,&height);
	glfwSetMousePos(width / 2, height / 2);
}

void DirectInput::Reset()
{
	m_MouseClickOnce[0] = m_MouseClickOnce[1] = -1;
	m_iKeyAction = m_iKeyDown = -1;
	m_iCharAction = m_iCharKeyDown = -1;
}

void DirectInput::UpdateMouse(int x, int y)
{
	int width = 0;
	int height = 0;

	glfwGetWindowSize(&width,&height);

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

void DirectInput::MousePos(int& x, int& y) const
{
	glfwGetMousePos(&x,&y);
}

const glm::vec2& DirectInput::GetTransformedMousePos() const
{
	return m_tpos;
}

bool DirectInput::KeyDown(int Key, bool once)
{
	return once ? (m_iKeyAction == GLFW_PRESS && m_iKeyDown == Key) : (glfwGetKey( Key ) == GLFW_PRESS);
}
bool DirectInput::KeyUp(int Key, bool once)
{
	return once ? (m_iKeyAction == GLFW_RELEASE && m_iKeyDown == Key) : (glfwGetKey( Key ) == GLFW_RELEASE);
}
bool DirectInput::CharKeyDown(char& out) const
{
	if(m_iCharKeyDown == -1 || m_iCharAction == GLFW_RELEASE)
		return false;

	out = m_iCharKeyDown;

	return true;
}

bool DirectInput::MouseClick(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_PRESS) : glfwGetMouseButton(iButton) == GLFW_PRESS);
}
bool DirectInput::MouseRelease(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_RELEASE) : glfwGetMouseButton(iButton) == GLFW_RELEASE);
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
	return glfwGetMouseWheel();
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
