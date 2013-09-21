// Programmed By Bryce Mehring


#ifndef _DINPUT_
#define _DINPUT_

// This is the GLFW Input Plug-in class
// All that's needed is too implement the functions provided in the abstract interface
#include "IKMInput.h"
#include "PluginManager.h"
#include <unordered_map>

#ifdef __GNUC__
using namespace __gnu_cxx;
#endif

class Input : public IKMInput
{
public:

	Input();

	// Callbacks
	static void CharCallback(GLFWwindow*,unsigned int);
	static void KeyCallback(GLFWwindow*,int,int,int,int);
	static void MouseCallback(GLFWwindow*,double,double);
	static void MouseButtonCallback(GLFWwindow*,int,int,int);
	static void MouseScrollCallback(GLFWwindow*,double,double);

	// IPlugin
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const { return "DirectInput"; }
	virtual int GetVersion() const;

	// IKMInput

	virtual void Poll();

	virtual bool LoadKeyBindFile(const std::string& file);

	// Keyboard
	virtual bool KeyDown(int Key, bool once = true);
	virtual bool KeyUp(int Key, bool once = true);
	virtual bool CharKeyDown(char&) const;

	// Mouse
	virtual const glm::vec2& GetCursorPos() const;
	virtual bool MouseClick(int Button, bool once) const;
	virtual bool MouseRelease(int Button, bool once) const;

	virtual int MouseX() const;
	virtual int MouseY() const;
	virtual double MouseZ() const;

	virtual bool GetSelectedRect(Math::AABB& out);

	virtual void SetCursorSensitivity(float);

	virtual void Init(class asIScriptEngine*);
	virtual void Destroy(class asIScriptEngine*);

private:

	static Input* s_pThis;

	std::unordered_map<int,std::vector<int> > m_bindings;

	// Keyboard
	int m_iKeyDown;
	int m_iKeyAction;

	unsigned int m_iCharKeyDown;

	//bool m_bJoyRight;

	// Mouse
	int m_iMouseX;
	int m_iMouseY;
	double m_fYScrollOffset;

	int m_MouseClickOnce[2];

	float m_fMouseSensistivity;

	glm::vec2 m_tpos;
	glm::vec2 m_selectedPos;

	// helper methods
	void Reset();
	bool CheckKey(int Key, bool once, int flag);
	void CenterMouse();
	void UpdateMouse(double x, double y);
	void RegisterScript(class asIScriptEngine*);
};

#endif
