#ifndef _IKMINPUT_
#define _IKMINPUT_

#include "IInput.h"
#include "VecMath.h"

// Keyboard/Mouse plugin
class IKMInput : public IInputPlugin
{
public:

	//note: the once parameter:
	//true, to only process the event the first time you press, false, returns true the entire time of the press

	// keyboard
	virtual bool KeyDown(int Key, bool once = true) = 0;
	virtual bool KeyUp(int Key, bool once = true) = 0;

	virtual bool CharKeyDown(char&) const = 0;

	// returns true if you click, button is down
	virtual bool MouseClick(int Button, bool once = true) const = 0;
	virtual bool MouseRelease(int Button, bool once = true) const = 0;

	//gets the mouse position
	virtual void MousePos(int& x, int& y) const = 0;

	//gets mouse position in world space
	virtual const glm::vec2& GetTransformedMousePos() const = 0;

	virtual int MouseX() const = 0; // horizontal
	virtual int MouseY() const = 0; // vertical
	virtual int MouseZ() const = 0; // scroll

	// selection box; returns true if user clicks
	virtual bool GetSelectedRect(Math::AABB& out) = 0;

	// Sensitivity, (0,FLT_MAX]
	virtual void SetCursorSensitivity(float) = 0;

protected:

	virtual ~IKMInput() {}

};

#endif //_IKMINPUT_
