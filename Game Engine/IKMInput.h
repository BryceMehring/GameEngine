#ifndef _IKMINPUT_
#define _IKMINPUT_

#include "IInput.h"
#include "VecMath.h"
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
	TILDE = 96,
	F1 = 112, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
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
	virtual bool KeyDown(unsigned char Key, bool once = true) = 0;
	virtual bool IsKeyDown() const = 0;
	virtual unsigned char GetKeyDown() const = 0;

	// mouse
	virtual bool MouseClick(int Button, bool once = true) const = 0;

	virtual POINT MousePos() = 0;
	virtual const struct D3DXVECTOR2& GetTransformedMousePos() const = 0;

	virtual int MouseX() = 0;
	virtual int MouseY() = 0;
	virtual int MouseZ() = 0;
	virtual bool GetSelectedRect(Math::FRECT& out) = 0;

	// mouse cursor states
	virtual void SetMouseState(MouseCursorState state) = 0; 

protected:

	virtual ~IKMInput() {}

};

#endif //_IKMINPUT_