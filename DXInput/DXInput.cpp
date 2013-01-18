
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

// DirectInput ctor
DirectInput::DirectInput(PluginManager& mgr)
: m_mgr(mgr), m_iMouseX(0), m_iMouseY(0), m_uiCurrentCursor(0), m_bMouseMove(false)
{
	// todo: clean this up

	ShowCursor(FALSE);

	m_MousePos.x = m_MousePos.y = 0;

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

	// remove config group from script
	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();
	DBAS(pEngine->RemoveConfigGroup("Input"));
	pEngine->Release();

	// destroy all cursors
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
	Rid[0].dwFlags = RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = m_mgr.GetWindowManager().GetWindowHandle();

	RegisterRawInputDevices(Rid,1,sizeof(RAWINPUTDEVICE));
}

void DirectInput::LoadCursors()
{
	/*m_cursors.push_back(LoadCursor(NULL,IDC_ARROW));
	m_cursors.push_back(LoadCursor(NULL,IDC_HAND));
	m_cursors.push_back(LoadCursor(NULL,IDC_IBEAM));*/
}

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
    
			GetRawInputData((HRAWINPUT)data.lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
    
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if(raw->header.dwType == RIM_TYPEMOUSE)
			{
				ReadMouse(raw->data.mouse);
			}

			break;
		}
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
			m_iMouseZ = (short)(mouse.usButtonData) / 120;

			if(m_iMouseZ > 1)
			{
				m_iMouseZ = -1;
			}

			break;
		}

	}

	m_iMouseX = mouse.lLastX;
	m_iMouseY = -mouse.lLastY;

	m_MousePos.x += m_iMouseX;
	m_MousePos.y += m_iMouseY;

	RECT R;
	GetClientRect(GetActiveWindow(),&R);

	const float W = R.right - R.left;
	const float H = R.bottom - R.top;

	m_tpos = 100.0f * D3DXVECTOR2(m_MousePos.x / W,m_MousePos.y / H);

	SetCursorPos((int)(W / 2.0f),(int)(H / 2.0f));

	ClampMouse();
}

void DirectInput::ClampMouse()
{
	if(m_tpos.x < -50.0f)
	{
		m_MousePos.x -= m_iMouseX;
		m_tpos.x = -50.0f;
	}
	else if(m_tpos.x > 50.0f)
	{
		m_MousePos.x -= m_iMouseX;
		m_tpos.x = 50.0f;
	}

	if(m_tpos.y < -50.0f)
	{
		m_MousePos.y -= m_iMouseY;
		m_tpos.y = -50.0f;
	}
	else if(m_tpos.y > 50.0f)
	{
		m_MousePos.y -= m_iMouseY;
		m_tpos.y = 50.0f;
	}
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

	D3DXVECTOR2 minPoint;
	D3DXVECTOR2 maxPoint;

	D3DXVec2Minimize(&minPoint,&m_selectedPos,&m_tpos);
	D3DXVec2Maximize(&maxPoint,&m_selectedPos,&m_tpos);

	out.topLeft = D3DXVECTOR2(minPoint.x,maxPoint.y);
	out.bottomRight = D3DXVECTOR2(maxPoint.x,minPoint.y);

	return true;
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