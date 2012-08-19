#ifndef _IKMINPUT_
#define _IKMINPUT_

#include "IInput.h"
#include <Windows.h>

enum KeyCode
{
	BACKSPACE = 8,
	TAB,
	ENTER = 13,
	ESCAPE = 27,
	SPACE = 32,
	LEFT = 37, UP,RIGHT,DOWN,
	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
	DEL = 127,
};

enum MouseCursorState
{
	Default,
	Hand,
	Beam,
};

// Keyboard mouse
class __declspec(novtable) IKMInput : public IInputPlugin
{
public:

	// keyboard
	virtual bool KeyDown(char Key) = 0;
	virtual bool IsKeyDown() const = 0;
	virtual char GetKeyDown() const = 0;

	// mouse
	virtual bool MouseClick(int Button, bool once = true) = 0;

	virtual POINT MousePos() = 0;
	virtual int MouseX() = 0;
	virtual int MouseY() = 0;
	virtual int MouseZ() = 0;

	// mouse cursor states
	virtual void SetMouseState(MouseCursorState state) = 0; 

protected:

	virtual ~IKMInput() {}

};

#endif //_IKMINPUT_