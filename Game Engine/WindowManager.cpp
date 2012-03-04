

#include "WindowManager.h"

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
		buffer += winError;
		DisplayError error(0,buffer.c_str(),0,0);
	}

	// todo: is this the best location for this?
	s_pThis = this;
}
WindowManager::~WindowManager()
{
}

bool WindowManager::Update()
{
	static MSG msg;
	static float dt = 0.01f;

	do
	{
		if(msg.message == WM_QUIT)
		{
			return false;
		}

		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
		{
			// todo: I could optimize this more
			// If the application is paused then free some CPU cycles to other 
			// applications and then continue on to the next frame.
			if( m_bPaused == true) { Sleep(20); }
		}

	} while(m_bPaused);

	return true;
}
void WindowManager::InitializeWindows(HINSTANCE hInstance, const string& winCaption)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
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
	UpdateWindow(m_hWindowHandle);
}
void WindowManager::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	MsgProcData data = {msg,wParam,lparam};
	m_MsgProcEvent.Notify(data);

	// todo: need to get rid of this. 
	//m_pInput->Poll(msg,wParam,lparam);

	switch( msg )
	{
		case WM_EXITSIZEMOVE:
		{
			//GetWindowRect(m_hWindowHandle,&m_rect);

			break;
		}

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
		case WM_CLOSE:
			DestroyWindow(m_hWindowHandle);
			break;
		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if( wParam == VK_ESCAPE )
			{
				PostQuitMessage(0);
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

void WindowManager::RedirectIOToConsole()
{

	/*int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	bool success = AllocConsole();

	if(success == false)
	{
		throw GetLastError();
	}

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, nullptr, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	setvbuf( stdin, nullptr, _IONBF, 0 );

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, nullptr, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

	// point to console as well

	ios::sync_with_stdio();*/

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