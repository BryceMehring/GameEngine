// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_


#include "IInput.h"
#include "PluginManager.h"
#include <array>
#include <unordered_map>
#include <vector>
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

	/*
	Loads a Keybind file
	format:
	bind NEW_KEY OLD_KEY
	*/
	bool LoadKeyBindFile(const std::string& file) override;

	// Returns true if Key is pressed, false otherwise
	bool KeyPress(int key, bool once = true) const override;

	// Returns true if Key is released, false otherwise
	bool KeyRelease(int key, bool once = true) const override;

	// Returns true if there is a character pressed, which is outputted through the parameter out
	// note: this method should only be used for text input
	bool CharKeyDown(char& out) const override;

	void RemapKey(int key, int newKey) override;

	// ----- Cursor -----

	// Returns true if the mouse button(0-7) is clicked, false otherwise
	bool MouseClick(int button, bool once = true) const override;

	// Returns true if the mouse button(0-7) is released, false otherwise
	bool MouseRelease(int button, bool once = true) const override;

	// Returns the cursor position in screen space
	// Origin: Bottom left hand corner
	// range: [(0, 0), (width, height)]
	// Note: if the cursor is disabled via ShowCursor(), the cursors position will not be updated
	const glm::ivec2& GetCursorPos() const override;

	// Moves the cursor to pos
	void SetCursorPos(glm::ivec2 pos) override;

	// Returns true if the cursor is shown, false otherwise
	bool IsCursorShown() const override;

	// Returns true if the cursor is within the main window
	bool IsCursorEntered() const override;

	// Shows the cursor if bShow is true
	// Disables the cursor if bShow is false
	void ShowCursor(bool bShow) override;

	// Returns Cursor Acceleration
	glm::ivec2 CursorAcceleration() const override;

	// Scroll acceleration
	double MouseZ() const override;

	// Returns true if user clicks, out is the current selection box
	bool GetSelectedRect(glm::ivec2& min, glm::ivec2& max) override;

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
	int m_iKeyDown;
	int m_iKeyAction;

	unsigned int m_iCharKeyDown;

	std::unordered_map<int, int> m_keyboardMapping;

	// Mouse
	int m_iMouseAccelerationX;
	int m_iMouseAccelerationY;
	double m_fOldMousePosX;
	double m_fOldMousePosY;
	double m_fYScrollOffset;
	bool m_bEntered;
	std::array<int, GLFW_MOUSE_BUTTON_LAST + 1> m_MouseClickOnce;

	glm::ivec2 m_cursorPos;
	glm::ivec2 m_selectedPos;

	// Joysticks
	std::array<float, 2> m_fJoyDeadZone;

	// The number of joystick axes
	int m_iNumJoystickAxes;

	// Points to all of the joystick axes on joystick 1
	// glfw allocated pointer, do not delete
	const float* m_pJoystickAxes;

	std::vector<unsigned char> m_joystickButtons;

	// helper methods
	void Reset();
	bool CheckKey(int key, bool once, int flag) const;
	void UpdateMouse(double x, double y);
	void UpdateJoystick();
};

#endif
