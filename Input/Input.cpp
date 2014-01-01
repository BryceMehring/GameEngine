
#include <GLFW/glfw3.h>
#include <angelscript.h>
#include "Input.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtx/fast_square_root.hpp>
#include <glm/gtc/constants.hpp>

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

Input::Input() : m_fMouseSensistivity(100.0f), m_tpos(0.0f, 0.0f), m_iNumJoystickAxes(0), 
m_pJoystickAxes(nullptr), m_iNumJoystickButtons(0), m_pJoystickButtons(nullptr)
{
	s_pThis = this;

	for (auto& iter : m_fJoyDeadZone)
	{
		iter = 0.2f;
	}

	Reset();

	glfwSetCharCallback(glfwGetCurrentContext(),CharCallback);
	glfwSetKeyCallback(glfwGetCurrentContext(),KeyCallback);
	glfwSetCursorPosCallback(glfwGetCurrentContext(),MouseCallback);
	glfwSetMouseButtonCallback(glfwGetCurrentContext(),MouseButtonCallback);
	glfwSetScrollCallback(glfwGetCurrentContext(),MouseScrollCallback);

	glfwSetInputMode(glfwGetCurrentContext(),GLFW_CURSOR,GLFW_CURSOR_HIDDEN);

	CenterMouse();
}

DLLType Input::GetPluginType() const
{
	return DLLType::Input;
}

const char* Input::GetName() const
{
	return "Input";
}

int Input::GetVersion() const
{
	return 0;
}

void Input::Init(asIScriptEngine* pAS)
{
	RegisterScript(pAS);
}

void Input::Destroy(asIScriptEngine* pAS)
{
	// Remove config group from script
	pAS->RemoveConfigGroup("Input");
}

void Input::Poll()
{
	Reset();

	UpdateJoystick();

	glfwPollEvents();
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

bool Input::KeyPress(int Key, bool once)
{
	return CheckKey(Key, once, GLFW_PRESS);
}
bool Input::KeyRelease(int Key, bool once)
{
	return CheckKey(Key, once, GLFW_RELEASE);
}
bool Input::CharKeyDown(char& out) const
{
	if (m_iCharKeyDown == (unsigned int)-1)
		return false;

	out = m_iCharKeyDown;

	return true;
}

bool Input::MouseClick(int iButton, bool once) const
{
	if (iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_PRESS) : glfwGetMouseButton(glfwGetCurrentContext(), iButton) == GLFW_PRESS);
}
bool Input::MouseRelease(int iButton, bool once) const
{
	if (iButton > 1)
		return false;

	return (once ? (m_MouseClickOnce[iButton] == GLFW_RELEASE) : glfwGetMouseButton(glfwGetCurrentContext(), iButton) == GLFW_RELEASE);
}

const glm::vec2& Input::GetCursorPos() const
{
	return m_tpos;
}

void Input::SetCursorPos(glm::vec2 pos)
{
	m_tpos = pos;
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
	if (!MouseClick(0, false))
		return false;

	out.min = glm::vec2(min(m_selectedPos.x, m_tpos.x), min(m_selectedPos.y, m_tpos.y));
	out.max = glm::vec2(max(m_selectedPos.x, m_tpos.x), max(m_selectedPos.y, m_tpos.y));

	return true;
}

void Input::SetCursorSensitivity(float s)
{
	m_fMouseSensistivity = s;
}

bool Input::IsValidJoystickConnected() const
{
	return (m_pJoystickAxes != nullptr) && (m_iNumJoystickAxes >= 5);
}

std::string Input::GetJoystickName() const
{
	const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

	return ((name != nullptr) ? std::string(name) : std::string());
}

void Input::SetJoystickAxesDeadZone(JoystickAxes i, float deadZone)
{
	m_fJoyDeadZone[(int)i] = deadZone;
}

bool Input::GetMovingJoystickAxes(int& outAxes, int& outDir) const
{
	bool success = false;

	for (unsigned int i = 0; i < 2 && !success; ++i)
	{
		glm::vec2 axes = GetJoystickAxes((JoystickAxes)i);

		// If the axes is being pushed
		if (axes.x > 0.0f || axes.x < 0.0f || axes.y > 0.0f || axes.y < 0.0f)
		{
			// Get the angle of the direction in degrees
			float angle = atan2(-axes.y, axes.x) * 180.0f / glm::pi<float>();

			if (angle < 135.0f && angle > 45.0f)
			{
				outDir = 0; // Up
			}
			else if (angle > -135.0f && angle < -45.0f)
			{
				outDir = 1; // Down
			}
			else if (angle > 135.0f || angle < -135.0f)
			{
				outDir = 2; // Left
			}
			else
			{
				outDir = 3; // Right
			}

			outAxes = i;
			
			success = true;
		}
	}

	return success;
}

glm::vec2 Input::GetJoystickAxes(JoystickAxes i) const
{
	glm::vec2 axes;

	if (IsValidJoystickConnected())
	{
		if (i == JoystickAxes::LS || i == JoystickAxes::RS)
		{
			if (i == JoystickAxes::LS)
			{
				axes = glm::vec2(m_pJoystickAxes[0], m_pJoystickAxes[1]);
			}
			else
			{
				axes = glm::vec2(m_pJoystickAxes[4], m_pJoystickAxes[3]);
			}

			float deadZone = m_fJoyDeadZone[(int)i];

			// Determine how far the controller is pushed
			float magnitude = glm::fastLength(axes);

			// Check if the controller is outside a circular dead zone
			if (magnitude > deadZone)
			{
				// Clip the magnitude at its expected maximum value
				if (magnitude > 1.0f)
				{
					magnitude = 1.0f;
				}

				// Determine the direction the controller is pushed
				axes /= magnitude;

				// Adjust magnitude relative to the end of the dead zone
				magnitude -= deadZone;

				// Normalize the magnitude with respect to its expected range
				// giving a magnitude value of 0.0 to 1.0
				axes *= (magnitude / (1.0f - deadZone));
			}
			else
			{
				axes = glm::vec2();
			}
		}
		else
		{
			axes = glm::vec2(0.0f, m_pJoystickAxes[2]);

			// Clip the magnitude at its expected maximum value
			axes.y = glm::clamp(axes.y, -1.0f, 1.0f);
		}
	}

	return axes;
}

int Input::GetNumJoystickButtons() const
{
	return m_iNumJoystickButtons;
}

bool Input::JoystickButtonPress(int i) const
{
	return (i < m_iNumJoystickButtons && i >= 0) && (m_pJoystickButtons[i] == GL_TRUE);
}

void Input::Reset()
{
	for (auto& iter : m_MouseClickOnce)
	{
		iter = -1;
	}

	m_iKeyAction = m_iKeyDown = -1;
	m_iCharKeyDown = -1;
	m_iMouseX = m_iMouseY = 0;
	m_fYScrollOffset = 0.0;
}

bool Input::CheckKey(int Key, bool once, int flag)
{
	if (once && (m_iKeyAction != flag))
		return false;

	bool bSuccess = false;
	auto iter = m_bindings.find(Key);
	if (once)
	{
		if (iter == m_bindings.end())
		{
			bSuccess = (Key == m_iKeyDown);
		}
		else
		{
			unsigned int i = 0;
			while (i < iter->second.size() && (iter->second[i] != m_iKeyDown)) { ++i; }

			bSuccess = (i < iter->second.size());
		}
	}
	else
	{
		if (iter == m_bindings.end())
		{
			bSuccess = (glfwGetKey(glfwGetCurrentContext(), Key) == flag);
		}
		else
		{
			unsigned int i = 0;
			while (i < iter->second.size() && (glfwGetKey(glfwGetCurrentContext(), iter->second[i]) != flag)) { ++i; }

			bSuccess = (i < iter->second.size());
		}
	}

	return bSuccess;
}

void Input::CenterMouse()
{
	int width;
	int height;

	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);
	glfwSetCursorPos(glfwGetCurrentContext(),width / 2, height / 2);
}

void Input::UpdateMouse(double x, double y)
{
	int width = 0;
	int height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(),&width,&height);

	// Get cursor acceleration in world space
	m_iMouseX = (int)x - (width / 2);
	m_iMouseY = -(int)y + (height / 2);

	// Offset cursor from the acceleration
	m_tpos += m_fMouseSensistivity * glm::vec2(m_iMouseX,m_iMouseY);

	CenterMouse();
}

void Input::UpdateJoystick()
{
	m_pJoystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &m_iNumJoystickAxes);
	m_pJoystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &m_iNumJoystickButtons);
}

void Input::RegisterScript(asIScriptEngine* pAS)
{
	pAS->BeginConfigGroup("Input");

	(pAS->RegisterObjectType("IInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	(pAS->RegisterObjectMethod("IInput","int mouseX()",asMETHOD(Input,MouseX),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IInput","int mouseY()",asMETHOD(Input,MouseY),asCALL_THISCALL));
	(pAS->RegisterObjectMethod("IInput","int mouseZ()",asMETHOD(Input,MouseZ),asCALL_THISCALL));
	(pAS->RegisterGlobalProperty("IInput input",this));

	pAS->EndConfigGroup();
}
