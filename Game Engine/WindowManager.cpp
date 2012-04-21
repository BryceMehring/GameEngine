

#include "WindowManager.h"
#include "FileManager.h"

using namespace std;

#define MAX_CONSOLE_LINES 500

class DisplayError
{
public:

	DisplayError(HWND hWnd, const char* lpText, const char* lpCaption, UINT uType)
	{
		MessageBox(hWnd,lpText,lpCaption,uType);
	}
	~DisplayError()
	{
		exit(0);
	}

private:
 
	// Nothing goes here!

};


// static variables
WindowManager* WindowManager::s_pThis = nullptr;

WindowManager::WindowManager(HINSTANCE hInstance,const string& winCaption) :
m_hInstance(hInstance), m_hWindowHandle(NULL), m_bPaused(false)
{
	// Initialize
	try
	{
		InitializeWindows(hInstance,winCaption);
		//RedirectIOToConsole();
		RegisterScript();
	}
	// catch any errors
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}
	catch(bad_alloc ba)
	{
		string buffer = "bad_alloc caught: ";
		buffer += ba.what();
		DisplayError error(0,buffer.c_str(),0,0);
	}
	catch(DWORD winError)
	{
		string buffer = "System Error: ";
		buffer += (int)winError;
		DisplayError error(0,buffer.c_str(),0,0);
	}

	::FileManager::Instance().WriteToLog("Window Constructed");
	s_pThis = this;
	//SendMessage(m_hWindowHandle,0,0,0);

	// todo: is this the best location for this?
	
}
WindowManager::~WindowManager()
{
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
		// If there are Window messages then process them.
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );

			if(msg.message == WM_QUIT)
			{
				return false;
			}
		}
		else if( m_bPaused == true)
		{
			Sleep(20);
		}

	} while(m_bPaused);

	return true;
}
void WindowManager::InitializeWindows(HINSTANCE hInstance, const string& winCaption)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "D3DWndClassName";

	if( !RegisterClass(&wc) ) { throw std::string("RegisterClass Failed"); }

	// Default to a window with a client area rectangle of 800x600.
	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = 800;
	m_rect.bottom = 600;
	AdjustWindowRect(&m_rect, WS_OVERLAPPEDWINDOW, false);
	m_hWindowHandle = CreateWindow("D3DWndClassName", winCaption.c_str(), 
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX) , 100, 100, m_rect.right, m_rect.bottom, 
		0, 0, hInstance, 0); 

	if( !m_hWindowHandle ) { throw std::string("RegisterClass Failed"); }

	ShowWindow(m_hWindowHandle, SW_SHOW);
	assert(UpdateWindow(m_hWindowHandle) != 0);
}
void WindowManager::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	MsgProcData data = {msg,wParam,lparam};
	m_events.Notify(data);

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
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE )
			{
				//::PostQuitMessage(0);
			}
			break;
	}
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

void WindowManager::Quit()
{
	PostQuitMessage(0);
}

void WindowManager::RegisterScript()
{
	// todo: maybe I could separate this from this class

	/*m_vm.reset(new asVM());

	asIScriptEngine* pEngine = m_vm->GetScriptEngine();

	asVM::RegisterScript(pEngine);
	UI::RegisterScript(pEngine);

	asIScriptEngine& scriptEngine = GetScriptVM().GetScriptEngine();

	// IEngine
	// todo: where should the registering go?

	// what should I ware today
	DBAS(scriptEngine.RegisterObjectType("IEngine",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","void Quit()",asMETHOD(WindowManager,Quit),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","string OpenFileName()",asMETHOD(WindowManager,OpenFileName),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("IEngine engine",this));

	scriptEngine.Release();*/
}