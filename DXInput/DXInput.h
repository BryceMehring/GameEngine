// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_
#pragma once

// This is the DirectX Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "IKMInput.h"
#include "PluginManager.h"

class DirectInput : public IKMInput//, public IScripted<DirectInput>
{
public:

	DirectInput(asIScriptEngine* as);
	virtual ~DirectInput();

	static void GLFWCALL CharCallback(int c, int action);
	static void GLFWCALL KeyCallback(int c, int action);
	static void GLFWCALL MouseCallback(int x, int y);
    static void GLFWCALL MouseClickCallback(int button, int action);

	// IRender
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const { return "DirectInput"; }
	virtual int GetVersion() const;
	
	// Keyboard
	virtual bool KeyDown(int Key, bool once = true);
	virtual int GetKeyDown() const;
	virtual bool IsKeyDown() const; // true if WM_CHAR is thrown

	// Mouse
	virtual void MousePos(int& x, int& y) const;
	virtual const glm::vec2& GetTransformedMousePos() const;
	virtual bool MouseClick(int Button, bool once) const;

	virtual int MouseX() const;
	virtual int MouseY() const;
	virtual int MouseZ() const;

	virtual bool GetSelectedRect(Math::AABB& out);

	virtual void SetMouseState(MouseCursorState state);

private:

	static DirectInput* s_pThis;

	// scripting engine
	asIScriptEngine* m_as;

	// Keyboard
	int m_iCharKeyDown;
	int m_iCharAction;

	int m_iKeyDown;
	int m_iKeyAction;

	unsigned char m_cKeyDownOnce;
	unsigned char m_cCharDown;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;
	int m_iMouseZ;
	//POINT m_MousePos;

	glm::vec2 m_tpos;
	glm::vec2 m_selectedPos;

	bool m_bMouseClick[2];
	bool m_bMouseClickOnce[2];

	bool m_bMouseMove;

	unsigned int m_uiCurrentCursor;

	int m_eventId;

	// helper functions
	void Reset();

	void ClampMouse();

	void CenterMouse();

	void RegisterScript();

	void UpdateMouse(int x, int y);
};

#endif
