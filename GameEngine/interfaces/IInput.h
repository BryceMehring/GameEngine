#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"
#include "VecMath.h"
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

	// processes input events
	virtual void Poll() = 0;

	// ----- Keyboard -----

	/*
	Loads a Keybind file
	format:
	bind NEW_KEY OLD_KEY
	*/
	virtual bool LoadKeyBindFile(const std::string& file) = 0;

	//note about the once parameter:
	//true: only process the event once,
	//false: returns true the entire period of the event

	// returns true if Key is pressed
	virtual bool KeyPress(int Key, bool once = true) = 0;

	// returns true if Key is released
	virtual bool KeyRelease(int Key, bool once = true) = 0;

	// returns true if there is a character pressed, which is outputted through the parameter out
	// note: this method should only be used for text input
	virtual bool CharKeyDown(char& out) const = 0;

	// ----- Cursor -----

	// returns true if the mouse is clicked
	virtual bool MouseClick(int Button, bool once = true) const = 0;

	// returns true if the mouse is released
	virtual bool MouseRelease(int Button, bool once = true) const = 0;

	//gets cursor position in world space
	// +x axis --> right
	// -x axis --> left
	// +y axis --> up
	// -y axis --> down
	virtual const glm::vec2& GetCursorPos() const = 0;

	virtual void SetCursorPos(glm::vec2 pos) = 0;

	// horizontal acceleration
	virtual int MouseX() const = 0;

	// vertical acceleration
	virtual int MouseY() const = 0;

	// scroll change
	virtual double MouseZ() const = 0;

	// returns true if user clicks, out is the current selection box
	virtual bool GetSelectedRect(Math::AABB& out) = 0;

	// Cursor Sensitivity, (0,FLT_MAX]
	virtual void SetCursorSensitivity(float) = 0;

	// ----- Joysticks -----

	// Returns true if a joystick is connected, false otherwise
	virtual bool IsValidJoystickConnected() const = 0;

	// Returns the name of the connected joystick
	// Note: If a joystick is not connected, an empty string is returned
	virtual std::string GetJoystickName() const = 0;

	// Sets the dead zone for the specified joystick axes
	// Dead zones are an area around the center of the joystick in which the axes will be zeroed
	virtual void SetJoystickAxesDeadZone(JoystickAxes i, float deadZone) = 0;

	//virtual void SetJoystickAxes(int axes, int dir) = 0;

	// Returns the axes and direction of the joystick axes being pushed via parameters
	// The function returns true when any axes is pressed, else false is returned
	// todo: replace the ints with something more meaningful
	virtual bool GetMovingJoystickAxes(int& axes, int& dir) const = 0;

	// Get the current value of the joystick axes
	// Note: If a joystick is not connected, a zeroed vec2 is returned
	virtual glm::vec2 GetJoystickAxes(JoystickAxes i) const = 0;

	// Returns the number of buttons on the joystick
	virtual int GetNumJoystickButtons() const = 0;

	// Returns true if the button is pressed, else false
	virtual bool JoystickButtonPress(int i) const = 0;
	
protected:

	virtual ~IInput() {}

};

#endif // _IINPUT_
