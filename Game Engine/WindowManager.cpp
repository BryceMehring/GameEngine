

#include "WindowManager.h"
#include "FileManager.h"
#include "asVM.h"

using namespace std;

// static variables
WindowManager* WindowManager::s_pThis = nullptr;

WindowManager::WindowManager(HINSTANCE hInstance) :
m_hInstance(hInstance), m_hWindowHandle(NULL), m_bPaused(false)
{
	// Initialize
	
	InitializeWindows(hInstance);

	::FileManager::Instance().WriteToLog("Window Constructed");
	s_pThis = this;
}
WindowManager::~WindowManager()
{
}

void WindowManager::SetWinCaption(const std::string& str)
{
	SetWindowText(m_hWindowHandle,str.c_str());
}


int WindowManager::AddMsgListener(const MsgDelegate& d)
{
	return m_events.Attach(d);
}

void WindowManager::RemoveListener(int id)
{
	m_events.Detach(id);
}


bool WindowManager::Update()
{
	MSG msg;

	do
	{
		while(::PeekMessage(&msg,0,0,0,PM_REMOVE) > 0)
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );

			if(msg.message == WM_QUIT)
			{
				return false;
			}
		}
		
		if(m_bPaused)
		{	
			Sleep(20);
		}

	} while(m_bPaused);

	return true;
}
void WindowManager::InitializeWindows(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	
	gassert(RegisterClass(&wc),"RegisterClass Failed");

	// Default to a window with a client area rectangle of 800x600.
	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = 800;
	m_rect.bottom = 600;
	AdjustWindowRect(&m_rect, WS_OVERLAPPEDWINDOW, false);
	m_hWindowHandle = CreateWindow("D3DWndClassName", 0, 
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) , 100, 100, m_rect.right, m_rect.bottom, 
		0, 0, hInstance, 0); 

	ShowWindow(m_hWindowHandle, SW_SHOW);
	assert(UpdateWindow(m_hWindowHandle) != 0);
}
void WindowManager::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	switch( msg )
	{
		// WM_ACTIVE is sent when the window is activated or deactivated.
		// We pause the game when the main window is deactivated and 
		// un pause it when it becomes active.
		case WM_ACTIVATE:
			if( LOWORD(wParam) == WA_INACTIVE )
			{
				m_bPaused = true;
			}
			else
			{
				m_bPaused = false;
			}
			break;

		// WM_CLOSE is sent when the user presses the 'X' button in the
		// caption bar menu.
		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			//Quit();
			break;
	}

	MsgProcData data = {msg,wParam,lparam};
	m_events.Notify(data);
}
LRESULT CALLBACK WindowManager::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created.
	if(s_pThis)
	{
		s_pThis->MsgProc(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*void WindowManager::GetWindowRect(RECT& R)
{
	GetClientRect(m_hWindowHandle,&R);
}*/

void WindowManager::Quit()
{
	PostQuitMessage(0);
}

void WindowManager::RegisterScript(asIScriptEngine* pEngine)
{
	DBAS(pEngine->RegisterObjectType("WindowManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("WindowManager","void quit() const",asMETHOD(WindowManager,Quit),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("WindowManager wm",this));
}