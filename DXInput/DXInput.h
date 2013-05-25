// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_

// This is the GLFW Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "IKMInput.h"
#include "PluginManager.h"

class DirectInput : public IKMInput //, public IScripted<DirectInput>
{
public:

    DirectInput();

    // Callbacks
	static void GLFWCALL CharCallback(int c, int action);
	static void GLFWCALL KeyCallback(int c, int action);
	static void GLFWCALL MouseCallback(int x, int y);
    static void GLFWCALL MouseClickCallback(int button, int action);

    // IPlugin
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const { return "DirectInput"; }
	virtual int GetVersion() const;
	
	// Keyboard
	virtual bool KeyDown(int Key, bool once = true);
    virtual bool KeyUp(int Key, bool once = true);
	virtual int GetKeyDown() const;
    virtual bool IsKeyDown() const;

	// Mouse
	virtual void MousePos(int& x, int& y) const;
	virtual const glm::vec2& GetTransformedMousePos() const;
    virtual bool MouseClick(int Button, bool once) const;
    virtual bool MouseRelease(int Button, bool once) const;

	virtual int MouseX() const;
	virtual int MouseY() const;
	virtual int MouseZ() const;

    virtual bool GetSelectedRect(Math::AABB& out);

    virtual void Init(class asIScriptEngine*);
    virtual void Destroy(class asIScriptEngine*);

private:

	static DirectInput* s_pThis;

	// Keyboard
    int m_iKeyDown;
    int m_iKeyAction;

	int m_iCharKeyDown;
	int m_iCharAction;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;

	glm::vec2 m_tpos;
	glm::vec2 m_selectedPos;

    int m_MouseClickOnce[2];

	// helper functions
	void Reset();
	void ClampMouse();
	void CenterMouse();
	void UpdateMouse(int x, int y);
    void RegisterScript(class asIScriptEngine*);
};

#endif
