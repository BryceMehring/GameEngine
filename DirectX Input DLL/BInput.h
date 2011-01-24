
#pragma once
#ifndef _DINPUT_
#define _DINPUT_

// This is the DirectX Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#include "PluginManager.h"
#include <d3dx9.h>

#include <dinput.h>

//PLUGIN_CREATION_DECL(CreateInputPlugin);
// Input plug-in deceleration
extern "C" PLUGINDECL IPlugin* CreatePlugin(PluginManager* mgr);

class DirectInput : public IInputPlugin
{
public:

	virtual const std::string& GetName() { return "DirectInput"; }
	virtual void About() {}

	// DirectX's version of poll 
	virtual void Poll();

	virtual bool KeyDown(char Key);
	virtual bool MouseClick(int Button);

	virtual int MouseX();
	virtual int MouseY();
	virtual int MouseZ();

protected:

	DirectInput(PluginManager* mgr);
	virtual ~DirectInput();

	// Plug-in Manager
	PluginManager* m_mgr;

	// Interface
	IDirectInput8* m_pDirectInput;

	// Keyboard
	IDirectInputDevice8* m_pKeyboard;
	char m_KeyboardState[256];

	// Mouse
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE2 m_MouseState;
	D3DXVECTOR3 m_Pos; 

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager* mgr);
};

#endif
