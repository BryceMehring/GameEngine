#ifndef _IKMINPUT_
#define _IKMINPUT_

#include "IInput.h"
#include <Windows.h>

enum KeyCode
{
	BACKSPACE = 8,
	TAB,
	ESCAPE = 27,
	SPACE = 32,
	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
	DEL = 127,
};

// Keyboard mouse
class __declspec(novtable) IKMInput : public IInputPlugin
{
public:

	// keyboard
	virtual bool KeyDown(unsigned char Key) = 0;
	virtual bool IsKeyDown() const = 0;
	virtual char GetKeyDown() const = 0;

	// mouse
	virtual bool MouseClick(int Button) = 0;
	virtual POINT MousePos() = 0;
	virtual int MouseX() = 0;
	virtual int MouseY() = 0;
	virtual int MouseZ() = 0;

protected:

	virtual ~IKMInput() {}

};

#endif //_IKMINPUT_