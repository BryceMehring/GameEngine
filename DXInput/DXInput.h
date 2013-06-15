// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_

// This is the GLFW Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#define PLUGIN_EXPORTS
#include "IKMInput.h"
#include "PluginManager.h"

class DirectInput : public IKMInput
{
public:

	DirectInput();

	// Callbacks
	static void CharCallback(GLFWwindow*,unsigned int);
	static void KeyCallback(GLFWwindow*,int,int,int,int);
	static void MouseCallback(GLFWwindow*,double,double);
	static void MouseButtonCallback(GLFWwindow*,int,int,int);

	// IPlugin
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const { return "DirectInput"; }
	virtual int GetVersion() const;

	virtual void Poll();
	
	// Keyboard
	virtual bool KeyDown(int Key, bool once = true);
	virtual bool KeyUp(int Key, bool once = true);
	virtual bool CharKeyDown(char&) const;

	// Mouse
	virtual void CursorPos(double& x, double& y) const;
	virtual const glm::vec2& GetTransformedMousePos() const;
	virtual bool MouseClick(int Button, bool once) const;
	virtual bool MouseRelease(int Button, bool once) const;

	virtual int MouseX() const;
	virtual int MouseY() const;
	virtual int MouseZ() const;

	virtual bool GetSelectedRect(Math::AABB& out);

	virtual void SetCursorSensitivity(float);

	virtual void Init(class asIScriptEngine*);
	virtual void Destroy(class asIScriptEngine*);

private:

	static DirectInput* s_pThis;

	// Keyboard
	int m_iKeyDown;
	int m_iKeyAction;

	unsigned int m_iCharKeyDown;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;

	int m_MouseClickOnce[2];

	float m_fMouseSensistivity;

	glm::vec2 m_tpos;
	glm::vec2 m_selectedPos;

	// helper functions
	void ClampMouse();
	void CenterMouse();
	void UpdateMouse(double x, double y);
	void RegisterScript(class asIScriptEngine*);
};

#endif
