
#define GLFW_DLL
#define GLFW_NO_GLU

//#include <vld.h>
#include <GL/glfw.h>
#include <angelscript.h>
#include "DXInput.h"

#include <iostream>

using namespace std;

// Input plug-in implementation
extern "C" PLUGINDECL IPlugin* CreatePlugin(asIScriptEngine* as)
{
	return new DirectInput(as);
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
        //s_pThis->m_iKeyDown;
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
        s_pThis->m_bMouseClickOnce[button] = action;
    }
}

// DirectInput ctor
DirectInput::DirectInput(asIScriptEngine* as) : m_iMouseX(0), m_iMouseY(0),
	m_uiCurrentCursor(0), m_bMouseMove(false), m_as(as), m_tpos(0.0f,0.0f)
{
	// todo: need to rework this: 
	s_pThis = this;

	RegisterScript();

	Reset();

	glfwSetCharCallback(CharCallback);
	glfwSetKeyCallback(KeyCallback);
	glfwSetMousePosCallback(MouseCallback);
    glfwSetMouseButtonCallback(MouseClickCallback);

	glfwEnable(GLFW_KEY_REPEAT);

	CenterMouse();
}


DirectInput::~DirectInput()
{
	// remove config group from script
	m_as->RemoveConfigGroup("Input");
	m_as->Release();
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
	// todo: clean this up

	/*m_cKeyDownOnce = m_cCharDown = -1;
	m_iMouseX = m_iMouseY = m_iMouseZ = 0;

	m_bMouseMove = false;

	memset(m_bMouseClickOnce,0,sizeof(m_bMouseClickOnce));


	m_iCharAction = -1;*/
    m_bMouseClickOnce[0] = m_bMouseClickOnce[1] = false;
}

void DirectInput::UpdateMouse(int x, int y)
{
	int width = 0;
	int height = 0;

	glfwGetWindowSize(&width,&height);

	m_iMouseX = x - (width / 2);
	m_iMouseY = -y + (height / 2);

	m_tpos += 50.0f * glm::vec2(m_iMouseX / (float)width ,m_iMouseY / (float)height);

	CenterMouse();
}

void DirectInput::ClampMouse()
{
	// todo: fix this
	/*if(m_tpos.x < -50.0f)
	{
		m_MousePos.x -= m_iMouseX;
		m_tpos.x = -50.0f;
	}
	else if(m_tpos.x > 50.0f)
	{
		m_MousePos.x -= m_iMouseX;
		m_tpos.x = 50.0f;
	}

	if(m_tpos.y < -50.0f)
	{
		m_MousePos.y -= m_iMouseY;
		m_tpos.y = -50.0f;
	}
	else if(m_tpos.y > 50.0f)
	{
		m_MousePos.y -= m_iMouseY;
		m_tpos.y = 50.0f;
	}*/
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
	return once ? (m_iKeyAction == GLFW_PRESS && m_iKeyDown == Key) : (glfwGetKey(Key) == GLFW_PRESS);
}
bool DirectInput::IsKeyDown() const
{
	return m_iCharAction != -1 && m_iCharAction == GLFW_PRESS;
}
int DirectInput::GetKeyDown() const
{
	return m_iCharKeyDown;
}
bool DirectInput::MouseClick(int iButton, bool once) const
{
    return (once ? m_bMouseClickOnce[iButton] : glfwGetMouseButton(iButton) == GLFW_PRESS);
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
	return m_iMouseZ;
}

bool DirectInput::GetSelectedRect(Math::AABB& out)
{
	if(!MouseClick(0,false))
		return false;

	out.min = glm::vec2(min(m_selectedPos.x,m_tpos.x),min(m_selectedPos.y,m_tpos.y));
	out.max = glm::vec2(max(m_selectedPos.x,m_tpos.x),max(m_selectedPos.y,m_tpos.y));

	return true;
}

void DirectInput::SetMouseState(MouseCursorState state)
{
	m_uiCurrentCursor = state;
}

void DirectInput::RegisterScript()
{
	m_as->BeginConfigGroup("Input");

	(m_as->RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	(m_as->RegisterObjectMethod("IKMInput","int mouseX()",asMETHOD(DirectInput,MouseX),asCALL_THISCALL));
	(m_as->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	(m_as->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	(m_as->RegisterObjectMethod("IKMInput","void SetMouseState(int)",asMETHOD(DirectInput,SetMouseState),asCALL_THISCALL));
	(m_as->RegisterGlobalProperty("IKMInput input",this));

	m_as->EndConfigGroup();
}
