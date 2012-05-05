
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

DirectInput::DirectInput(PluginManager& mgr) : m_mgr(mgr), m_bLeftMouseClick(false)
{
	WindowManager* pWinMgr = m_mgr.GetWindowManager();
	m_eventId = pWinMgr->AddMsgListener(WindowManager::MsgDelegate(this,&DirectInput::Poll));
}


DirectInput::~DirectInput()
{
	WindowManager* pWinMgr = m_mgr.GetWindowManager();
	pWinMgr->RemoveListener(m_eventId);
	//m_mgr.GetMsgProcEvent().Detach(m_id);
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
	m_bLeftMouseClick = false;
	m_cKeyDown = m_cCharDown = -1;
}

void DirectInput::ReadKeyboard()
{
}

void DirectInput::Poll(const MsgProcData& data)
{
	switch(data.msg)
	{
		/*case WM_INPUT:
		{
			UINT dwSize;
			 
			GetRawInputData((HRAWINPUT)data.lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL) 
			{
				break;
			}
			if (GetRawInputData((HRAWINPUT)data.lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
			 OutputDebugString (TEXT("GetRawInputData does not return correct size !\n")); 
			
			RAWINPUT* raw = (RAWINPUT*)lpb;

			if(raw->header.dwType == RIM_TYPEMOUSE)
			{
				m_iMouseX = raw->data.mouse.lLastX;
				m_iMouseX = raw->data.mouse.lLastY;
			}
	
			hResult = StringCchPrintf(szTempOutput, STRSAFE_MAX_CCH, TEXT("Mouse: usFlags=%04x ulButtons=%04x usButtonFlags=%04x usButtonData=%04x ulRawButtons=%04x lLastX=%04x lLastY=%04x ulExtraInformation=%04x\r\n"), 
				raw->data.mouse.usFlags, 
				raw->data.mouse.ulButtons, 
				raw->data.mouse.usButtonFlags, 
				raw->data.mouse.usButtonData, 
				raw->data.mouse.ulRawButtons, 
				raw->data.mouse.lLastX, 
				raw->data.mouse.lLastY, 
				raw->data.mouse.ulExtraInformation);

			if (FAILED(hResult))
			{
			// TODO: write error handler
			}
			OutputDebugString(szTempOutput);

			delete[] lpb;
			break;
		}*/
		case WM_KEYDOWN:
		{
			// if this is the first WM_KEYDOWN msg
			if((data.lparam & (1 << 30)) == 0)
			{
				m_cKeyDown = data.wParam;
			}
			break;
		}
		case WM_CHAR:
			m_cCharDown = data.wParam;
			break;
		case WM_LBUTTONDOWN:
			m_bLeftMouseClick = true;
			break;
	}

	// mouse coords
	HWND h = m_mgr.GetWindowHandle();
	GetCursorPos(&m_MousePos);
	ScreenToClient(h,&m_MousePos);	
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
	return m_bLeftMouseClick;
	//return ((m_MouseState.rgbButtons[iButton] & 0x80) != 0);
}
int DirectInput::MouseX()
{
	return 0;
	//return m_MouseState.lX;
}

int DirectInput::MouseY()
{
	return 0;
	//return m_MouseState.lY;
}

int DirectInput::MouseZ()
{
	return 0;
	//return m_MouseState.lZ;
}

void DirectInput::About()
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