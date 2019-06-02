
#include "Input.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/geometric.hpp>
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
		s_pThis->OnKey(key, action);
	}
}

void Input::MouseCallback(GLFWwindow*, double x, double y)
{
	if(s_pThis != nullptr)
	{
		s_pThis->OnMouseCurrsorPos(glm::vec2(x, y));
	}
}

void Input::MouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
	if(s_pThis != nullptr)
	{
		s_pThis->OnMouseButtonCallback(button, action);
	}
}

void Input::MouseScrollCallback(GLFWwindow*, double, double yOffset)
{
	if(s_pThis != nullptr)
	{
		s_pThis->m_fYScrollOffset = yOffset;
	}
}

void Input::CursorEnterCallback(GLFWwindow*, int entered)
{
	s_pThis->m_bEntered = entered == GL_TRUE;
}

Input::Input() : m_bEntered(true), m_iNumJoystickAxes(0), m_pJoystickAxes(nullptr)
{
	s_pThis = this;

	for (auto& iter : m_fJoyDeadZone)
	{
		iter = 0.2f;
	}

	Reset();

	// Configure Keyboard and Mouse callbacks
	GLFWwindow* currentContext = glfwGetCurrentContext();
	glfwSetCharCallback(currentContext, CharCallback);
	glfwSetKeyCallback(currentContext, KeyCallback);
	glfwSetCursorPosCallback(currentContext, MouseCallback);
	glfwSetMouseButtonCallback(currentContext, MouseButtonCallback);
	glfwSetScrollCallback(currentContext, MouseScrollCallback);
	glfwSetCursorEnterCallback(currentContext, CursorEnterCallback);

	// Move the mouse to the center of the screen
	int width, height;
	glfwGetWindowSize(currentContext, &width, &height);
	glfwSetCursorPos(currentContext, width / 2, height / 2);

	m_cursorPos.x = width / 2;
	m_cursorPos.y = height / 2;

	m_fOldMousePosX = width / 2.0;
	m_fOldMousePosY = height / 2.0;
}

Input::~Input()
{
	GLFWwindow* currentContext = glfwGetCurrentContext();
	glfwSetCharCallback(currentContext, nullptr);
	glfwSetKeyCallback(currentContext, nullptr);
	glfwSetCursorPosCallback(currentContext, nullptr);
	glfwSetMouseButtonCallback(currentContext, nullptr);
	glfwSetScrollCallback(currentContext, nullptr);
	glfwSetCursorEnterCallback(currentContext, nullptr);
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

void Input::Poll()
{
	Reset();

	UpdateJoystick();
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

		if(!command.empty() && command[0] != '#')
		{
			if(command == "bind")
			{
				string from;
				string to;

				stream >> from;
				stream >> to;

				if(!from.empty() && !to.empty())
				{
					RemapKey(from[0], to[0]);
				}
			}
		}
	}

	inFile.close();

	return true;
}

void Input::clearCallbacks()
{
	m_keyCallback.clear();
}

void Input::removeCallback(const std::string& id) {
	m_keyCallback.erase(id);
}

void Input::addKeyPressCallback(std::string&& id, const std::function<bool(int, bool)>&& callback)
{
	m_keyCallback.emplace(id, callback);
}

void Input::addMouseButtonCallback(std::string&& id, const std::function<bool(int, bool)>&& callback)
{
	m_mouseButtonCallback.emplace(id, callback);
}

void Input::addMouseCursorPosCallback(std::string&& id, const MOUSE_CURSOR_POS_CALLBACK_TYPE&& callback)
{
	m_mouseCursorPosCallback.emplace(id, callback);
}

void Input::OnAction(std::multimap<std::string, std::function<bool(int, bool)>>& callbacks, int keyOrButton, int action)
{
	auto callback = callbacks.begin();
	while (callback != callbacks.end()) {
		if (!(callback->second)(keyOrButton, action == GLFW_PRESS)) {
			callback = callbacks.erase(callback);
		}
		else
		{
			callback++;
		}
	}
}

void Input::OnKey(int key, int action)
{
	OnAction(m_keyCallback, key, action);
}

void Input::OnMouseButtonCallback(int button, int action)
{
	OnAction(m_mouseButtonCallback, button, action);
}

void Input::OnMouseCurrsorPos(glm::dvec2 cursorPos)
{
	glm::dvec2 mouseAcceleration = cursorPos - m_oldMousePos;
	mouseAcceleration.y = -mouseAcceleration.y;

	m_oldMousePos = cursorPos;

	int height;
	glfwGetWindowSize(glfwGetCurrentContext(), nullptr, &height);

	glm::dvec2 windowCursorPos(cursorPos.x, height - cursorPos.y);

	auto callback = m_mouseCursorPosCallback.begin();
	while (callback != m_mouseCursorPosCallback.end()) {
		if (!(callback->second)(cursorPos, windowCursorPos, mouseAcceleration)) {
			callback = m_mouseCursorPosCallback.erase(callback);
		}
		else
		{
			callback++;
		}
	}
}

void Input::RemapKey(int key, int newKey)
{
	m_keyboardMapping[newKey] = key;
}

bool Input::IsCursorShown() const
{
	return (glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
}

void Input::ShowCursor(bool bShow)
{
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, bShow ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool Input::GetSelectedRect(glm::ivec2& min, glm::ivec2& max)
{
	if (!MouseClick(0, false))
		return false;

	min = glm::ivec2(std::min(m_selectedPos.x, m_cursorPos.x), std::min(m_selectedPos.y, m_cursorPos.y));
	max = glm::ivec2(std::max(m_selectedPos.x, m_cursorPos.x), std::max(m_selectedPos.y, m_cursorPos.y));

	return true;
}

bool Input::IsValidJoystickConnected() const
{
	return (m_pJoystickAxes != nullptr) && (m_iNumJoystickAxes >= 2);
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
			else if (m_iNumJoystickAxes >= 5)
			{
				axes = glm::vec2(m_pJoystickAxes[4], m_pJoystickAxes[3]);
			}

			float deadZone = m_fJoyDeadZone[(int)i];

			// Determine how far the controller is pushed
			float magnitude = glm::length(axes);

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
	return m_joystickButtons.size();
}

bool Input::JoystickButtonPress(int button, bool once) const
{
	return (button < GetNumJoystickButtons() && button >= 0) && (m_joystickButtons[button] == 1 || (!once && m_joystickButtons[button] == 2));
}

bool Input::JoystickButtonRelease(int button, bool once) const
{
	return (button < GetNumJoystickButtons() && button >= 0) && ((!once && m_joystickButtons[button] == 0) || (once && m_joystickButtons[button] == 3));
}

void Input::Reset()
{
	for (auto& iter : m_MouseClickOnce)
	{
		iter = -1;
	}

	m_iKeyAction = m_iKeyDown = -1;
	m_iCharKeyDown = -1;
	m_iMouseAccelerationX = m_iMouseAccelerationY = 0;
	m_fYScrollOffset = 0.0;
}

bool Input::CheckKey(int key, bool once, int flag) const
{
	bool bSuccess = false;

	auto iter = m_keyboardMapping.find(key);
	if (iter != m_keyboardMapping.end())
	{
		key = iter->second;
	}
	
	if (once)
	{
		bSuccess = (key == m_iKeyDown) && (m_iKeyAction == flag);
	}
	else
	{
		bSuccess = (glfwGetKey(glfwGetCurrentContext(), key) == flag);
	}
	
	return bSuccess;
}

void Input::UpdateMouse(double x, double y)
{
	// Get cursor acceleration in world space
	m_iMouseAccelerationX = static_cast<int>(x - m_fOldMousePosX);
	m_iMouseAccelerationY = -static_cast<int>(y - m_fOldMousePosY);
	m_fOldMousePosX = x;
	m_fOldMousePosY = y;

	if (IsCursorShown())
	{
		int height;
		glfwGetWindowSize(glfwGetCurrentContext(), nullptr, &height);

		m_cursorPos.x = static_cast<int>(x);
		m_cursorPos.y = height - static_cast<int>(y);
	}
}

void Input::UpdateJoystick()
{
	m_pJoystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &m_iNumJoystickAxes);

	int count = 0;
	const unsigned char* pJoystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

	m_joystickButtons.resize(count);
	for (int i = 0; i < count; ++i)
	{
		if (pJoystickButtons[i] == 1)
		{
			m_joystickButtons[i] += 1;
			if (m_joystickButtons[i] > 2)
			{
				m_joystickButtons[i] = 2;
			}
		}
		else if (pJoystickButtons[i] == 0)
		{
			if (JoystickButtonPress(i,false))
			{
				m_joystickButtons[i] = 3;
			}
			else
			{
				m_joystickButtons[i] = 0;
			}			
		}
	}
}
