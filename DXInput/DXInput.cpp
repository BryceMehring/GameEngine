
#include "DXInput.h"

// todo: why are we including FileManager?
#include "FileManager.h"
#include "WindowManager.h"

#pragma comment(lib,"Game Engine.lib")


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
	memset(m_cKeyDown,0,sizeof(m_cKeyDown));
	memset(m_tpos,0,sizeof(D3DXVECTOR2));
}


DirectInput::~DirectInput()
{
	WindowManager& winMgr = m_mgr.GetWindowManager();
	winMgr.RemoveListener(m_eventId);

	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();
	DBAS(pEngine->RemoveConfigGroup("Input"));
	pEngine->Release();

	for(auto iter = this->m_cursors.begin(); iter != m_cursors.end(); ++iter)
	{
		DestroyCursor(*iter);
	}
}

void DirectInput::InitRawInput()
{
	RAWINPUTDEVICE Rid[1];
        
	Rid[0].usUsagePage = 0x01; 
	Rid[0].usUsage = 0x02; 
	Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = m_mgr.GetWindowManager().GetWindowHandle();

	RegisterRawInputDevices(Rid,1,sizeof(RAWINPUTDEVICE));
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

DLLType DirectInput::GetPluginType() const
{
	return InputPlugin;
}

void DirectInput::Reset()
{
	m_cKeyDownOnce = m_cCharDown = -1;
	m_iMouseX = m_iMouseY = m_iMouseZ = 0;

	m_bMouseMove = false;

	memset(m_bMouseClickOnce,0,sizeof(m_bMouseClickOnce));
}

void DirectInput::Poll(const MsgProcData& data)
{
	switch(data.msg)
	{
		case WM_INPUT: 
		{
			UINT dwSize = 40;
			BYTE lpb[40];
    
			GetRawInputData((HRAWINPUT)data.lparam, RID_INPUT, 
			lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if(raw->header.dwType == RIM_TYPEMOUSE)
			{
				ReadMouse(raw->data.mouse);
			}
			else
			{
				ReadKeyboard(raw->data.keyboard);
			}

			break;
		}
		case WM_SETCURSOR:
			SetCursor(m_cursors[m_uiCurrentCursor]);
			break;
		case WM_MOUSEMOVE:
			{

			RECT R;
			GetClientRect(GetActiveWindow(),&R);

			m_bMouseMove = true;
			m_MousePos.x  = LOWORD(data.lparam);
			m_MousePos.y = HIWORD(data.lparam);

			const float W = R.right - R.left;
			const float H = R.bottom - R.top;

			m_tpos = 50.0f * D3DXVECTOR2((2.0f * m_MousePos.x / W - 1.0f),-2.0f*m_MousePos.y / H + 1.0f);

			}

			// the mouse pos in this case goes from [0,0] to [X,Y]
			// I want the mouse to be translated to [-1,1] to [1,-1]
			break;
		case WM_KEYDOWN:
			m_cKeyDownOnce = data.wParam;
			m_cKeyDown[m_cKeyDownOnce] = true;
			break;
		case WM_KEYUP:
			m_cKeyDown[data.wParam] = false;
			m_cKeyDownOnce = -1;
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
			m_selectedPos = m_tpos;
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

void DirectInput::ReadKeyboard(const RAWKEYBOARD& keyboard)
{
	//this->m_cKeyDown = keyboard.Message;
}

POINT DirectInput::MousePos()
{
	return m_MousePos;
}

const D3DXVECTOR2& DirectInput::GetTransformedMousePos() const
{
	return m_tpos;
}

bool DirectInput::KeyDown(unsigned char Key, bool once)
{
	return once ? (m_cKeyDownOnce == Key) : m_cKeyDown[Key];
}
bool DirectInput::IsKeyDown() const
{
	return (m_cCharDown != (unsigned char)(-1)) && (m_cCharDown >= 32);
}
unsigned char DirectInput::GetKeyDown() const
{
	return m_cCharDown;
	//return m_Key;
}
bool DirectInput::MouseClick(int iButton, bool once) const
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

bool DirectInput::GetSelectedRect(Math::FRECT& out)
{
	if(!MouseClick(0,false))
		return false;

	D3DXVec2Minimize(&out.topLeft,&m_selectedPos,&m_tpos);
	D3DXVec2Maximize(&out.bottomRight,&m_selectedPos,&m_tpos);
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
	DBAS(pEngine->RegisterObjectMethod("IKMInput","void SetMouseState(int)",asMETHOD(DirectInput,SetMouseState),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("IKMInput input",this));

	pEngine->EndConfigGroup();

	pEngine->Release();
}