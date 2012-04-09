// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_
#pragma once

// This is the DirectX Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "IScripted.h"
#include "IKMInput.h"
#include "PluginManager.h"
#include <dinput.h>

class DirectInput : public IKMInput//, public IScripted<DirectInput>
{
public:

	friend class IScripted<DirectInput>;

	// IRender
	virtual DLLType GetType() const;
	virtual void About();

	// DirectX's version of poll 
	
	
	// Keyboard
	virtual bool KeyDown(char Key);
	virtual char GetKeyDown() const;
	virtual bool IsKeyDown() const; // true if WM_CHAR is thrown

	// Mouse
	virtual POINT MousePos();
	virtual bool MouseClick(int Button);

	virtual int MouseX();
	virtual int MouseY();
	virtual int MouseZ();

private:

	virtual void Poll(const struct MsgProcData& data);

	DirectInput(PluginManager& mgr);
	virtual ~DirectInput();

	// Plug-in Manager
	PluginManager& m_mgr;

	// Interface

	// Keyboard
	char m_cKeyDown;
	char m_cCharDown;

	bool m_bLeftMouseClick;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;
	POINT m_MousePos;

	int m_eventId;

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);

	// helper functions
	void Reset();
	void ReadKeyboard();

	static void _RegisterScript();
};

#endif
