
#include "DXInput.h"

// todo: why are we including FileManager?
#include "FileManager.h"
#include "WindowManager.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Game Engine.lib")

// todo: this should not be needed
#pragma comment(lib,"d3dx9.lib")


// Input plug-in implementation
PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DirectInput(mgr);
}

DirectInput::DirectInput(PluginManager& mgr)
: m_mgr(mgr), m_iMouseX(0), m_iMouseY(0), m_uiCurrentCursor(0), m_bMouseMove(false)
{
	WindowManager& winMgr = m_mgr.GetWindowManager();
	m_eventId = winMgr.AddMsgListener(WindowManager::MsgDelegate(this,&DirectInput::Poll));

	RegisterScript();

	InitRawInput();

	LoadCursors();

	Reset();

	memset(m_bMouseClick,0,sizeof(m_bMouseClickOnce));
}


DirectInput::~DirectInput()
{
	WindowManager& winMgr = m_mgr.GetWindowManager();
	winMgr.RemoveListener(m_eventId);

	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();
	pEngine->RemoveConfigGroup("Input");
	pEngine->Release();
	//m_mgr.GetMsgProcEvent().Detach(m_id);
}

void DirectInput::InitRawInput()
{
	RAWINPUTDEVICE Rid;
        
	Rid.usUsagePage = 0x01; 
	Rid.usUsage = 0x02; 
	Rid.dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid.hwndTarget = m_mgr.GetWindowManager().GetWindowHandle();

	/*Rid[1].usUsagePage = 0x01; 
	Rid[1].usUsage = 0x06; 
	Rid[1].dwFlags = RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
	Rid[1].hwndTarget = 0;*/

	if (RegisterRawInputDevices(&Rid, 1, sizeof(Rid)) == FALSE) {
	//registration failed. Call GetLastError for the cause of the error.
	}
}

void DirectInput::LoadCursors()
{
	m_cursors.push_back(LoadCursor(NULL,IDC_ARROW));
	m_cursors.push_back(LoadCursor(NULL,IDC_HAND));
	m_cursors.push_back(LoadCursor(NULL,IDC_IBEAM));
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

	m_bMouseMove = false;

	memset(m_bMouseClickOnce,0,sizeof(m_bMouseClickOnce));
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
		case WM_SETCURSOR:
			SetCursor(m_cursors[m_uiCurrentCursor]);
			break;
		case WM_MOUSEMOVE:
			m_bMouseMove = true;
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
		/*case WM_LBUTTONDOWN:
			m_bMouseClick[0] = true;
			break;
		case WM_LBUTTONUP:
			m_bMouseClick[0] = false;
			break;*/
	}
}

void DirectInput::ReadMouse(const RAWMOUSE& mouse)
{
	switch(mouse.usButtonFlags)
	{
		case RI_MOUSE_LEFT_BUTTON_DOWN:
			m_bMouseClick[0] = m_bMouseClickOnce[0] = true;
			break;
		case RI_MOUSE_LEFT_BUTTON_UP:
			m_bMouseClick[0] = false;
			break;
		case RI_MOUSE_RIGHT_BUTTON_DOWN:
			m_bMouseClick[1] = m_bMouseClickOnce[1] = true;
			break;
		case RI_MOUSE_RIGHT_BUTTON_UP:
			m_bMouseClick[1] = false;
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
bool DirectInput::MouseClick(int iButton, bool once)
{
	return (once ? m_bMouseClickOnce[iButton] : m_bMouseClick[iButton]);
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

void DirectInput::SetMouseState(MouseCursorState state)
{
	m_uiCurrentCursor = state;
}

void DirectInput::About() const
{
	MessageBox(nullptr,"DirectInput DLL\nProgrammed By Bryce Mehring","About",MB_OK);
}

void DirectInput::RegisterScript()
{
	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();

	pEngine->BeginConfigGroup("Input");

	DBAS(pEngine->RegisterObjectType("IKMInput",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("IKMInput","void about()",asMETHOD(DirectInput,About),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("IKMInput","int mouseX()",asMETHOD(DirectInput,MouseX),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("IKMInput","int mouseY()",asMETHOD(DirectInput,MouseY),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("IKMInput","int mouseZ()",asMETHOD(DirectInput,MouseZ),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("IKMInput input",this));

	pEngine->EndConfigGroup();

	pEngine->Release();
}