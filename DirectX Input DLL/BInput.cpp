
#include "stdafx.h"
#include "BInput.h"
#include "BEngine.h"
#include "Singleton.h"


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Game Engine.lib")

using namespace AngelScript;

// Input plug-in implementation
PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DirectInput(mgr);
}

DirectInput::DirectInput(PluginManager& mgr) : m_mgr(mgr)
{
	ZeroMemory(m_KeyboardState,sizeof(m_KeyboardState));
	ZeroMemory(&m_MouseState,sizeof(m_MouseState));

	DirectInput8Create(m_mgr.GetHINSTANCE(),DIRECTINPUT_VERSION,IID_IDirectInput8A,(void**)&m_pDirectInput,0);

	//Create Keyboard
	m_pDirectInput->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,0);
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(m_mgr.GetWindowHandle(),DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboard->Acquire();

	//Create Mouse 
	m_pDirectInput->CreateDevice(GUID_SysMouse,&m_pMouse,0);
	m_pMouse->SetDataFormat(&c_dfDIMouse2);
	m_pMouse->SetCooperativeLevel(m_mgr.GetWindowHandle(),DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	m_pMouse->Acquire();

	m_MousePos.x = m_MousePos.y = 0;

	RegisterScript();
}


DirectInput::~DirectInput()
{
	//m_mgr->Release();

	m_pDirectInput->Release();

	m_pKeyboard->Unacquire();
	m_pMouse->Unacquire();

	m_pKeyboard->Release();
	m_pMouse->Release();
}

DLLType DirectInput::GetType()
{
	return Input;
}

void DirectInput::Poll()
{
	//Poll Keyboard
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_KeyboardState),m_KeyboardState);

	if(FAILED(hr))
	{
		// Keyboard Lost
		ZeroMemory(m_KeyboardState,sizeof(m_KeyboardState));
		m_pKeyboard->Acquire();
	}

	// Poll mouse
	hr = m_pMouse->GetDeviceState(sizeof(m_MouseState),&m_MouseState);

	if(FAILED(hr))
	{
		ZeroMemory(&m_MouseState,sizeof(m_MouseState));
		m_pMouse->Acquire();
	}

	// mouse coords relative to the window
	RECT rect;

	GetWindowRect(m_mgr.GetWindowHandle(),&rect);
	GetCursorPos(&m_MousePos);

	// todo: need to change this for full screen vs. windows
	m_MousePos.x -= rect.left;
	m_MousePos.y -= rect.top + 30;
}

void DirectInput::MousePos(POINT& pos)
{
	pos = m_MousePos;
}

bool DirectInput::KeyDown(char Key)
{
	return ((m_KeyboardState[Key] & 0x80) != 0);
}

bool DirectInput::MouseClick(int iButton)
{
	return ((m_MouseState.rgbButtons[iButton] & 0x80) != 0);
}
int DirectInput::MouseX()
{
	return m_MouseState.lX;
}

int DirectInput::MouseY()
{
	return m_MouseState.lY;
}

int DirectInput::MouseZ()
{
	return m_MouseState.lZ;
}

void DirectInput::About()
{
	MessageBox(m_mgr.GetWindowHandle(),"DirectInput DLL\nProgrammed By Bryce Mehring","About",MB_OK);
}

void DirectInput::RegisterScript()
{	
	asVM& vm = m_mgr.GetScriptVM();
	asIScriptEngine& engine = vm.GetScriptEngine();

	DBAS(engine.RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(engine.RegisterObjectMethod("IKMInput","void Poll()",asMETHOD(DirectInput,Poll),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("IKMInput","bool MouseClick(int)",asMETHOD(DirectInput,MouseClick),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod("DirectInput","bool KeyDown(char)",asMETHOD(DirectInput,KeyDown),asCALL_THISCALL));
	
	DBAS(engine.RegisterObjectMethod("IKMInput","void MousePos(POINT& out)",asMETHOD(DirectInput,MousePos),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("IKMInput","int MouseX()",asMETHOD(DirectInput,MouseX),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("IKMInput","int MouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	DBAS(engine.RegisterObjectMethod("IKMInput","int MouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	DBAS(engine.RegisterGlobalProperty("IKMInput input",this));

	engine.Release();
}