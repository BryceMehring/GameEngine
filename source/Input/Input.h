// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_


#include "IInput.h"
#include "PluginManager.h"
#include <array>
#include <unordered_map>
#include <vector>
#include <functional>
#include <GLFW/glfw3.h>

// GLFW Input plug-in
class Input final : public IInput
{
public:

	//note about the once parameter:
	//true: only process the event once,
	//false: returns true the entire period of the event

	Input();
	~Input();

	// Callbacks
	static void CharCallback(GLFWwindow*,unsigned int);
	static void KeyCallback(GLFWwindow*,int,int,int,int);
	static void MouseCallback(GLFWwindow*,double,double);
	static void MouseButtonCallback(GLFWwindow*,int,int,int);
	static void MouseScrollCallback(GLFWwindow*,double,double);
	static void CursorEnterCallback(GLFWwindow*,int);

	// IPlugin

	DLLType GetPluginType() const override;
	const char* GetName() const override;
	int GetVersion() const override;

	// IInput

	// Processes input events
	void Poll() override;

	// ----- Keyboard -----

	void clearCallbacks() override;

	void removeCallback(const std::string& id) override;

	void addKeyPressCallback(std::string&& id, const std::function<bool(int, bool)>&& callback) override;
	void addMouseButtonCallback(std::string&& id, const std::function<bool(int, bool)>&& callback) override;
	void addMouseCursorPosCallback(std::string&& id, const MOUSE_CURSOR_POS_CALLBACK_TYPE&& callback) override;

	// ----- Cursor -----

	// Returns true if the cursor is shown, false otherwise
	bool IsCursorShown() const override;

	// Shows the cursor if bShow is true
	// Disables the cursor if bShow is false
	void ShowCursor(bool bShow) override;

	// ----- Joysticks -----

	// Returns true if a joystick is connected, false otherwise
	bool IsValidJoystickConnected() const override;

	// Returns the name of the connected joystick
	// Note: If a joystick is not connected, an empty string is returned
	std::string GetJoystickName() const override;

	// Sets the dead zone for the specified joystick axes
	// Dead zones are an area around the center of the joystick in which the axes will be zeroed
	void SetJoystickAxesDeadZone(JoystickAxes i, float deadZone) override;

	// Returns the axes and direction of the joystick axes being pushed via parameters
	// The function returns true when any axes is pressed, else false is returned
	// todo: replace the ints with something more meaningful
	bool GetMovingJoystickAxes(int& axes, int& dir) const override;

	// Returns the current value of the joystick axis
	// Note: If a joystick is not connected, a zeroed vec2 is returned
	glm::vec2 GetJoystickAxes(JoystickAxes i) const override;

	// Returns the number of buttons on the joystick
	int GetNumJoystickButtons() const override;

	// Returns true if the button is pressed, else false
	bool JoystickButtonPress(int button, bool once = true) const override;

	// Returns true if the button is pressed, else false
	bool JoystickButtonRelease(int button, bool once = true) const override;

private:

	static Input* s_pThis;

	// Keyboard
	std::multimap< std::string, std::function<bool(int, bool)> > m_keyCallback;

	// Mouse
	std::multimap< std::string, std::function<bool(int, bool)> > m_mouseButtonCallback;
	std::multimap< std::string, MOUSE_CURSOR_POS_CALLBACK_TYPE > m_mouseCursorPosCallback;

	glm::dvec2 m_oldMousePos;

	// Joysticks
	std::array<float, 2> m_fJoyDeadZone;

	// The number of joystick axes
	int m_iNumJoystickAxes;

	// Points to all of the joystick axes on joystick 1
	// glfw allocated pointer, do not delete
	const float* m_pJoystickAxes;

	std::vector<unsigned char> m_joystickButtons;

	// helper methods
	void OnAction(std::multimap< std::string, std::function<bool(int, bool)> >& callbacks, int key, int action);
	void OnKey(int key, int action);
	void OnMouseButtonCallback(int button, int action);
	void OnMouseCurrsorPos(glm::dvec2);
	void UpdateJoystick();
};

#endif
