// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_

// This is the GLFW Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#include "IInput.h"
#include "PluginManager.h"
#include <unordered_map>
#include <array>

class Input : public IInput
{
public:

	Input();

	// Callbacks
	static void CharCallback(GLFWwindow*,unsigned int);
	static void KeyCallback(GLFWwindow*,int,int,int,int);
	static void MouseCallback(GLFWwindow*,double,double);
	static void MouseButtonCallback(GLFWwindow*,int,int,int);
	static void MouseScrollCallback(GLFWwindow*,double,double);

	// IPlugin

	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const;
	virtual int GetVersion() const;

	virtual void Init(class asIScriptEngine*);
	virtual void Destroy(class asIScriptEngine*);

	// IInput

	virtual void Poll();

	// ----- Keyboard -----

	/*
	Loads a Keybind file
	format:
	bind NEW_KEY OLD_KEY
	*/
	virtual bool LoadKeyBindFile(const std::string& file);

	//note about the once parameter:
	//true: only process the event once,
	//false: returns true the entire period of the event

	// returns true if Key is pressed
	virtual bool KeyPress(int Key, bool once = true);

	// returns true if Key is released
	virtual bool KeyRelease(int Key, bool once = true);

	// returns true if there is a character pressed, which is outputted through the parameter out
	// note: this method should only be used for text input
	virtual bool CharKeyDown(char& out) const;

	// ----- Cursor -----

	// returns true if the mouse is clicked
	virtual bool MouseClick(int Button, bool once = true) const;

	// returns true if the mouse is released
	virtual bool MouseRelease(int Button, bool once = true) const;

	//gets cursor position in world space
	// +x axis --> right
	// -x axis --> left
	// +y axis --> up
	// -y axis --> down
	virtual const glm::vec2& GetCursorPos() const;

	virtual void SetCursorPos(glm::vec2 pos);

	// horizontal acceleration
	virtual int MouseX() const;

	// vertical acceleration
	virtual int MouseY() const;

	// scroll acceleration
	virtual double MouseZ() const;

	// returns true if user clicks, out is the current selection box
	virtual bool GetSelectedRect(Math::AABB& out);

	// Cursor Sensitivity, (0,FLT_MAX]
	virtual void SetCursorSensitivity(float);

	// ----- Joysticks -----

	// Returns true if a joystick is connected, false otherwise
	virtual bool IsValidJoystickConnected() const;

	// Returns the name of the connected joystick
	// Note: If a joystick is not connected, an empty string is returned
	virtual std::string GetJoystickName() const;

	// Sets the dead zone for the specified joystick axes
	// Dead zones are an area around the center of the joystick in which the axes will be zeroed
	virtual void SetJoystickAxesDeadZone(JoystickAxes i, float deadZone);

	// Returns the axes and direction of the joystick axes being pushed via parameters
	// The function returns true when any axes is pressed, else false is returned
	// todo: replace the ints with something more meaningful
	virtual bool GetMovingJoystickAxes(int& axes, int& dir) const;

	// Get the current value of the joystick axes
	// Note: If a joystick is not connected, a zeroed vec2 is returned
	virtual glm::vec2 GetJoystickAxes(JoystickAxes i) const;

	// Returns the number of buttons on the joystick
	virtual int GetNumJoystickButtons() const;

	// Returns true if the button is pressed, else false
	virtual bool JoystickButtonPress(int i) const;

private:

	static Input* s_pThis;

	std::unordered_map<int,std::vector<int> > m_bindings;

	// Keyboard
	int m_iKeyDown;
	int m_iKeyAction;

	unsigned int m_iCharKeyDown;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;
	double m_fYScrollOffset;

	std::array<int, 2> m_MouseClickOnce;

	float m_fMouseSensistivity;

	glm::vec2 m_tpos;
	glm::vec2 m_selectedPos;

	// Joysticks
	std::array<float, 2> m_fJoyDeadZone;

	// The number of joystick axes
	int m_iNumJoystickAxes;

	// Points to all of the joystick axes on joystick 1
	// glfw allocated pointer, do not delete
	const float* m_pJoystickAxes;

	int m_iNumJoystickButtons;

	const unsigned char* m_pJoystickButtons;

	// helper methods
	void Reset();
	bool CheckKey(int Key, bool once, int flag);
	void CenterMouse();
	void UpdateMouse(double x, double y);
	void UpdateJoystick();
	void RegisterScript(class asIScriptEngine*);
};

#endif
