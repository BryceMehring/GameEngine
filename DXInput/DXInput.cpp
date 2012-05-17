
#include "DXInput.h"
#include "WindowManager.h"
#include <Windowsx.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Game Engine.lib")


// Input plug-in implementation
PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DirectInput(mgr);
}

DirectInput::DirectInput(PluginManager& mgr)
: m_mgr(mgr), m_iMouseX(0), m_iMouseY(0)
{
	WindowManager* pWinMgr = m_mgr.GetWindowManager();
	m_eventId = pWinMgr->AddMsgListener(WindowManager::MsgDelegate(this,&DirectInput::Poll));

	InitRawInput();

	Reset();
}


DirectInput::~DirectInput()
{
	WindowManager* pWinMgr = m_mgr.GetWindowManager();
	pWinMgr->RemoveListener(m_eventId);
	//m_mgr.GetMsgProcEvent().Detach(m_id);
}

void DirectInput::InitRawInput()
{
	RAWINPUTDEVICE Rid;
        
	Rid.usUsagePage = 0x01; 
	Rid.usUsage = 0x02; 
	Rid.dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid.hwndTarget = m_mgr.GetWindowHandle();

	/*Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
	Rid[1].hwndTarget = 0;*/

	if (RegisterRawInputDevices(&Rid, 1, sizeof(Rid)) == FALSE) {
	//registration failed. Call GetLastError for the cause of the error.
	}
}

/*void DirectInput::Notify(const MsgProcData& data)
{
	
}*/

int DirectInput::GetVersion() const
{
	return 0;
}

DLLType DirectInput::GetType() const
{
	return Input;
}

void DirectInput::Reset()
{
	m_cKeyDown = m_cCharDown = -1;
	m_iMouseX = m_iMouseY = m_iMouseZ = 0;

	memset(m_bMouseClick,0,sizeof(m_bMouseClick));
}

void DirectInput::Poll(const MsgProcData& data)
{
	// todo: Look up SendInput()
	// Link: http://msdn.microsoft.com/en-us/library/windows/desktop/ms646310(v=vs.85).aspx

	switch(data.msg)
	{
		case WM_INPUT: 
		{
			UINT dwSize = 40;
			BYTE lpb[40];
    
			GetRawInputData((HRAWINPUT)data.lparam, RID_INPUT, 
			lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
			RAWINPUT* raw = (RAWINPUT*)lpb;

			ReadMouse(raw->data.mouse);

			break;
		} 

		case WM_MOUSEMOVE:
			m_MousePos.x  = LOWORD(data.lparam);
			m_MousePos.y = HIWORD(data.lparam);
			break;
		case WM_KEYDOWN:
			m_cKeyDown = data.wParam;
			break;
		case WM_KEYUP:
			m_cKeyDown = -1;
			break;
		case WM_CHAR:
			m_cCharDown = data.wParam;
			break;
		case WM_LBUTTONDOWN:
			//m_bLeftMouseClick = true;
			break;
	}
}

void DirectInput::ReadMouse(const RAWMOUSE& mouse)
{
	switch(mouse.usButtonFlags)
	{
		case RI_MOUSE_LEFT_BUTTON_DOWN:
			m_bMouseClick[0] = true;
		break;
		case RI_MOUSE_RIGHT_BUTTON_DOWN:
			m_bMouseClick[1] = true;
			break;
		case RI_MOUSE_WHEEL:
		{
			m_iMouseZ = mouse.usButtonData / 120;

			if(m_iMouseZ > 1)
			{
				m_iMouseZ = -1;
			}

			break;
		}

	}

	m_iMouseX = mouse.lLastX;
	m_iMouseY = -mouse.lLastY;
}

POINT DirectInput::MousePos()
{
	return m_MousePos;
}

bool DirectInput::KeyDown(char Key)
{
	return (m_cKeyDown == Key);
}
bool DirectInput::IsKeyDown() const
{
	return (m_cCharDown != -1) && (m_cCharDown >= 32);
	//return m_keyDown;
}
char DirectInput::GetKeyDown() const
{
	return m_cCharDown;
	//return m_Key;
}
bool DirectInput::MouseClick(int iButton)
{
	return m_bMouseClick[iButton];
	//return ((m_MouseState.rgbButtons[iButton] & 0x80) != 0);
}
int DirectInput::MouseX()
{
	return m_iMouseX;
	//return m_MouseState.lX;
}

int DirectInput::MouseY()
{
	return m_iMouseY;
	//return m_MouseState.lY;
}

int DirectInput::MouseZ()
{
	return m_iMouseZ;
}

void DirectInput::About() const
{
	MessageBox(m_mgr.GetWindowHandle(),"DirectInput DLL\nProgrammed By Bryce Mehring","About",MB_OK);
}

/*void DirectInput::_RegisterScript()
{	
	DBAS(s_pScriptEngine->RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","void about()",asMETHOD(DirectInput,About),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","void poll()",asMETHOD(DirectInput,Poll),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","bool mouseClick(int)",asMETHOD(DirectInput,MouseClick),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod("DirectInput","bool KeyDown(char)",asMETHOD(DirectInput,KeyDown),asCALL_THISCALL));
	
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","void mousePos(POINT& out)",asMETHOD(DirectInput,MousePos),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","int mouseX()",asMETHOD(DirectInput,MouseX),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	DBAS(s_pScriptEngine->RegisterGlobalProperty("IKMInput input",s_pThis));
}*/