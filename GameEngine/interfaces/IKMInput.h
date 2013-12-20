#ifndef _IKMINPUT_
#define _IKMINPUT_

#include "IInput.h"
#include "VecMath.h"
#include <string>

// Keyboard/Mouse plugin
class IKMInput : public IInputPlugin
{
public:

	/*
	 Loads a Keybind file
	 format:
	 bind NEW_KEY OLD_KEY
	*/
	virtual bool LoadKeyBindFile(const std::string& file) = 0;

	//note about the once paramater:
	//true: only process the event once,
	//false: returns true the entire period of the event

	// returns true if Key is pressed
	virtual bool KeyPress(int Key, bool once = true) = 0;

	// returns true if Key is released
	virtual bool KeyRelease(int Key, bool once = true) = 0;

	// returns true if there is a character pressed, which is ouputed through the paramater out
	// note: this method should only be used for text input
	virtual bool CharKeyDown(char& out) const = 0;

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

protected:

	virtual ~IKMInput() {}

};

#endif //_IKMINPUT_
