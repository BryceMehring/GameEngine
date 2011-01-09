
#pragma once
#include "Singleton.h"

class BInput
{
public:

	DECLARE_SINGLETON(BInput);

	void Poll();
	bool KeyDown(char Key);
	bool MouseClick(int Button);

	int MouseX();
	int MouseY();
	int MouseZ();

private:

	// Interface
	IDirectInput8* m_pDirectInput;

	// Keyboard
	IDirectInputDevice8* m_pKeyboard;
	char m_KeyboardState[256];

	// Mouse
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE2 m_MouseState;
	D3DXVECTOR3 m_Pos;

};