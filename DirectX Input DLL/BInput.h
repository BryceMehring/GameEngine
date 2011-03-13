// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_
#pragma once

// This is the DirectX Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "PluginManager.h"

#include <dinput.h>


class DirectInput : public IKMInput
{
public:

	virtual void GetName(std::string& name);
	virtual void About();

	// DirectX's version of poll 
	virtual void Poll();

	virtual void MousePos(POINT&);
	virtual bool KeyDown(char Key);
	virtual bool MouseClick(int Button);

	virtual int MouseX();
	virtual int MouseY();
	virtual int MouseZ();

private:

	
	DirectInput(PluginManager& mgr);
	virtual ~DirectInput();

	// Plug-in Manager
	PluginManager& m_mgr;

	// Interface
	IDirectInput8* m_pDirectInput;

	// Keyboard
	IDirectInputDevice8* m_pKeyboard;
	char m_KeyboardState[256];

	// Mouse
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE2 m_MouseState;
	POINT m_MousePos;

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);
};

#endif
