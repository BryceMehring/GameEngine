
#include <GLFW/glfw3.h>
#include <angelscript.h>
#include "Input.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Input plug-in implementation
extern "C" PLUGINDECL IPlugin* CreatePlugin()
{
	return new Input();
}

Input* Input::s_pThis = nullptr;

void Input::CharCallback(GLFWwindow*, unsigned int c)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iCharKeyDown = c;
	}
}

void Input::KeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_iKeyDown = key;
		s_pThis->m_iKeyAction = action;
	}
}

void Input::MouseCallback(GLFWwindow*, double x, double y)
{
	if(s_pThis != nullptr)
	{
		s_pThis->UpdateMouse(x,y);
	}
}

void Input::MouseButtonCallback(GLFWwindow*,int button, int action, int mods)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_selectedPos = s_pThis->m_tpos;
		s_pThis->m_MouseClickOnce[button] = action;
	}
}

void Input::MouseScrollCallback(GLFWwindow*, double, double yOffset)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_fYScrollOffset = yOffset;
	}
}

// Input ctor
Input::Input() : m_fMouseSensistivity(100.0f), m_tpos(0.0f,0.0f)
{
	s_pThis = this;

	Reset();

	glfwSetCharCallback(glfwGetCurrentContext(),CharCallback);
	glfwSetKeyCallback(glfwGetCurrentContext(),KeyCallback);
	glfwSetCursorPosCallback(glfwGetCurrentContext(),MouseCallback);
	glfwSetMouseButtonCallback(glfwGetCurrentContext(),MouseButtonCallback);
	glfwSetScrollCallback(glfwGetCurrentContext(),MouseScrollCallback);

	glfwSetInputMode(glfwGetCurrentContext(),GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
	glfwSetInputMode(glfwGetCurrentContext(),GLFW_STICKY_KEYS,GL_TRUE);

	CenterMouse();
}

void Input::Init(asIScriptEngine* pAS)
{
	RegisterScript(pAS);
}

void Input::Destroy(asIScriptEngine* pAS)
{
	// remove config group from script
	pAS->RemoveConfigGroup("Input");
}

int Input::GetVersion() const
{
	return 0;
}

DLLType Input::GetPluginType() const
{
	return DLLType::Input;
}

bool Input::LoadKeyBindFile(const string& file)
{
	ifstream inFile(file);
	if(!inFile.is_open())
		return false;

	string line;
	while(getline(inFile,line))
	{
		istringstream stream(line);

		string command;
		stream >> command;

		if(command.size() > 0 && command[0] != ';')
		{
			if(command == "bind")
			{
				string from;
				string to;

				stream >> from;
				stream >> to;

				if(from.size() > 0 && to.size() > 0)
				{
					m_bindings[to[0]].push_back(from[0]);
				}
			}
		}
	}

	inFile.close();

	return true;
}

void Input::CenterMouse()
{
	int width;
	int height;

	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);
	glfwSetCursorPos(glfwGetCurrentContext(),width / 2, height / 2);
}

void Input::Reset()
{
	m_MouseClickOnce[0] = m_MouseClickOnce[1] = -1;
	m_iKeyAction = m_iKeyDown = -1;
	m_iCharKeyDown = -1;
	m_iMouseX = m_iMouseY = 0;
	m_fYScrollOffset = 0.0f;
}

void Input::Poll()
{
	Reset();

	/*if(glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE)
	{
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1,&count);
		if(axes[0] > 0.2f && fabsf(axes[1]) < 0.5f)
		{
			m_bJoyRight = true;
		}
		else
		{
			m_bJoyRight = false;
		}

	}*/
	glfwPollEvents();
}

void Input::UpdateMouse(double x, double y)
{
	int width = 0;
	int height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);

	m_iMouseX = (int)x - (width / 2);
	m_iMouseY = -(int)y + (height / 2);

	m_tpos += m_fMouseSensistivity * glm::vec2(m_iMouseX,m_iMouseY);

	CenterMouse();
}
const glm::vec2& Input::GetCursorPos() const
{
	return m_tpos;
}
void Input::SetCursorPos(glm::vec2 pos)
{
	m_tpos = pos;
}

bool Input::CheckKey(int Key, bool once, int flag)
{
	if(once && (m_iKeyAction != flag))
		return false;

	bool bSuccess = false;
	auto iter = m_bindings.find(Key);
	if(once)
	{
		if(iter == m_bindings.end())
		{
			bSuccess = (Key == m_iKeyDown);
		}
		else
		{
			unsigned int i = 0;
			while(i < iter->second.size() && (iter->second[i] != m_iKeyDown)) { ++i; }

			bSuccess = (i < iter->second.size());
		}
	}
	else
	{
		if(iter == m_bindings.end())
		{
			bSuccess = (glfwGetKey(glfwGetCurrentContext(), Key  ) == flag);
		}
		else
		{
			unsigned int i = 0;
			while(i < iter->second.size() && (glfwGetKey(glfwGetCurrentContext(), iter->second[i]  ) != flag)) { ++i; }

			bSuccess = (i < iter->second.size());
		}
	}

	return bSuccess;
}

bool Input::KeyPress(int Key, bool once)
{
	return CheckKey(Key,once,GLFW_PRESS);
}
bool Input::KeyRelease(int Key, bool once)
{
	return CheckKey(Key,once,GLFW_RELEASE);
}
bool Input::CharKeyDown(char& out) const
{
	if(m_iCharKeyDown == (unsigned int)-1)
		return false;

	out = m_iCharKeyDown;

	return true;
}

bool Input::MouseClick(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_PRESS) : glfwGetMouseButton(glfwGetCurrentContext(),iButton) == GLFW_PRESS);
}
bool Input::MouseRelease(int iButton, bool once) const
{
	if(iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_RELEASE) : glfwGetMouseButton(glfwGetCurrentContext(),iButton) == GLFW_RELEASE);
}
int Input::MouseX() const
{
	return m_iMouseX;
}

int Input::MouseY() const
{
	return m_iMouseY;
}

double Input::MouseZ() const
{
	return m_fYScrollOffset;
}

bool Input::GetSelectedRect(Math::AABB& out)
{
	if(!MouseClick(0,false))
		return false;

	out.min = glm::vec2(min(m_selectedPos.x,m_tpos.x),min(m_selectedPos.y,m_tpos.y));
	out.max = glm::vec2(max(m_selectedPos.x,m_tpos.x),max(m_selectedPos.y,m_tpos.y));

	return true;
}

void Input::SetCursorSensitivity(float s)
{
	m_fMouseSensistivity = s;
}

void Input::RegisterScript(asIScriptEngine* pAS)
{
	pAS->BeginConfigGroup("Input");

	(pAS->RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	(pAS->RegisterObjectMethod("IKMInput","int mouseX()",asMETHOD(Input,MouseX),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(Input,MouseY),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(Input,MouseZ),asCALL_THISCALL));
	(pAS->RegisterGlobalProperty("IKMInput input",this));

	pAS->EndConfigGroup();
}
