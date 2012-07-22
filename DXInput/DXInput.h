// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_
#pragma once

// This is the DirectX Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "IKMInput.h"
#include "PluginManager.h"
#include <dinput.h>

class DirectInput : public IKMInput//, public IScripted<DirectInput>
{
public:

	// IRender
	virtual DLLType GetType() const;
	virtual const char* GetName() const { return "DirectInput"; }
	virtual void About() const;
	virtual int GetVersion() const;

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

	virtual void SetMouseState(MouseCursorState state);

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

	// Mouse
	int m_iMouseX;
	int m_iMouseY;
	int m_iMouseZ;
	POINT m_MousePos;

	bool m_bMouseClick[2];

	bool m_bMouseMove;

	std::vector<HCURSOR> m_cursors;
	unsigned int m_uiCurrentCursor;

	int m_eventId;

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);

	// helper functions
	void Reset();
	void ReadMouse(const RAWMOUSE& mouse);
	void ReadKeyboard();

	void InitRawInput();
	void LoadCursors();

	void RegisterScript();
};

#endif
