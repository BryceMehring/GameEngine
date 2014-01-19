#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"
#include <glm/vec2.hpp>
#include <string>

enum class JoystickAxes
{
	LS,
	RS,
	LT,
	RT,
};

// Input Plugin
class IInput : public IPlugin
{
public:

	//note about the once parameter:
	//true: only process the event once,
	//false: returns true the entire period of the event

	// Processes input events
	virtual void Poll() = 0;

	// ----- Keyboard -----

	/*
	Loads a Keybind file
	format:
	bind NEW_KEY OLD_KEY
	*/
	virtual bool LoadKeyBindFile(const std::string& file) = 0;

	// Returns true if Key is pressed, false otherwise
	virtual bool KeyPress(int key, bool once = true) const = 0;

	// Returns true if Key is released, false otherwise
	virtual bool KeyRelease(int key, bool once = true) const = 0;

	// Returns true if there is a character pressed, which is outputted through the parameter out
	// note: this method should only be used for text input
	virtual bool CharKeyDown(char& out) const = 0;

	// ----- Cursor -----

	// Returns true if the mouse button(0 - 7) is clicked, false otherwise
	virtual bool MouseClick(int button, bool once = true) const = 0;

	// Returns true if the mouse button(0 - 7) is released, false otherwise
	virtual bool MouseRelease(int button, bool once = true) const = 0;

	// Returns the cursor position in screen space
	// Origin: Bottom left hand corner
	// range: [(0, 0), (width, height)]
	// Note: if the cursor is disabled via ShowCursor(), the cursors position will not be updated
	virtual const glm::ivec2& GetCursorPos() const = 0;

	// Moves the cursor to pos
	virtual void SetCursorPos(glm::ivec2 pos) = 0;

	// Returns true if the cursor is shown, false otherwise
	virtual bool IsCursorShown() const = 0;

	// Shows the cursor if bShow is true
	// Disables the cursor if bShow is false
	virtual void ShowCursor(bool bShow) = 0;

	// Horizontal acceleration
	virtual int MouseX() const = 0;

	// Vertical acceleration
	virtual int MouseY() const = 0;

	// Scroll change
	virtual double MouseZ() const = 0;

	// Returns true if user clicks, out is the current selection box
	virtual bool GetSelectedRect(glm::ivec2& min, glm::ivec2& max) = 0;

	// ----- Joysticks -----

	// Returns true if a joystick is connected, false otherwise
	virtual bool IsValidJoystickConnected() const = 0;

	// Returns the name of the connected joystick
	// Note: If a joystick is not connected, an empty string is returned
	virtual std::string GetJoystickName() const = 0;

	// Sets the dead zone for the specified joystick axes
	// Dead zones are an area around the center of the joystick in which the axes will be zeroed
	virtual void SetJoystickAxesDeadZone(JoystickAxes i, float deadZone) = 0;

	// Returns the axes and direction of the joystick axes being pushed via parameters
	// The function returns true when any axes is pressed, else false is returned
	// todo: replace the ints with something more meaningful
	virtual bool GetMovingJoystickAxes(int& axes, int& dir) const = 0;

	// Returns the current value of the joystick axis
	// Note: If a joystick is not connected, a zeroed vec2 is returned
	virtual glm::vec2 GetJoystickAxes(JoystickAxes i) const = 0;

	// Returns the number of buttons on the joystick
	virtual int GetNumJoystickButtons() const = 0;

	// Returns true if the button is pressed, else false
	virtual bool JoystickButtonPress(int button, bool once = true) const = 0;

	// Returns true if the button is released, else false
	virtual bool JoystickButtonRelease(int button, bool once = true) const = 0;
	
protected:

	virtual ~IInput() {}

};

#endif // _IINPUT_
