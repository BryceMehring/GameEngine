
#include "BEngine.h"
#include "asVM.h"
#include "PluginManager.h"
#include <io.h>
#include <fcntl.h>

using namespace AngelScript;
using namespace std;

#define MAX_CONSOLE_LINES 500

IBaseEngine* g_pEngine = NULL;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created.
	if(g_pEngine)
	{
		g_pEngine->MsgProc(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

IBaseEngine::IBaseEngine(HINSTANCE hInstance,const string& winCaption) : m_hInstance(hInstance)
{
	
	

	// Default values
	m_pVM = NULL;
	m_pPM = NULL;
	m_pRenderer = NULL;
	m_pInput = NULL;
	m_hWindowHandle = NULL;
	m_fDT = m_fStartCount = m_fSecsPerCount = 0.0f;
	m_bPaused      = false;


	// Initialize
	try
	{
		InitializeWindows(hInstance,winCaption);
	}
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}

	// todo: need to fix this logic
	g_pEngine = this;

	// initialize timer. todo: I could put this code into its own method
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;

	RedirectIOToConsole();
	RegisterScript();
	InitializePlugins();
}

IBaseEngine::~IBaseEngine()
{
	// todo need to release data members
}

void IBaseEngine::StartCounter()
{
	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	m_fStartCount = prevTimeStamp;
}

void IBaseEngine::EndCounter()
{
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	m_fDT = (currTimeStamp - m_fStartCount)*m_fSecsPerCount;
}

bool IBaseEngine::Update()
{
	static MSG msg;
	static float dt = 0.01f;

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
		// If the application is paused then free some CPU cycles to other 
		// applications and then continue on to the next frame.
		if( m_bPaused )
		{
			Sleep(20);
			Update();
		}
	}

	return true;
}

void IBaseEngine::InitializeWindows(HINSTANCE hInstance, const string& winCaption)
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

void IBaseEngine::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	switch( msg )
	{
		case WM_SIZING:
		
		break;
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
				OnLostDevice();
				m_bPaused = true;
			}
			else
			{
				OnResetDevice();
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

string IBaseEngine::OpenFileName() const
{
	OPENFILENAME ofn = {0};
	char fileName[MAX_PATH] = "";
 
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWindowHandle;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
 
	string fileNameStr;
 
	if ( GetOpenFileName(&ofn) )
	{
		fileNameStr = fileName;
	}
 
	return fileNameStr;
}

void IBaseEngine::RedirectIOToConsole()
{

	int hConHandle;

	long lStdHandle;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	FILE *fp;

	// allocate a console for this app

	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;

	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console

	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "r" );

	*stdin = *fp;

	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console

	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stderr = *fp;

	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

	// point to console as well

	ios::sync_with_stdio();

}
void IBaseEngine::InitializePlugins()
{
	if(m_pPM == NULL)
	{
		m_pPM = new PluginManager();
	}

	// todo: to to write code to load all dll files in a folder, or even have the user
	// pick through OpenFileName().
	m_pRenderer = static_cast<IRenderingPlugin*>(m_pPM->LoadDLL("DX9 Rendering.dll"));
	m_pInput = static_cast<IKMInput*>(m_pPM->LoadDLL("DirectX Input DLL.dll"));
}

asVM& IBaseEngine::GetScriptVM() const
{
	return *m_pVM;
}
IKMInput& IBaseEngine::GetInput() const
{
	return *m_pInput;
}
IRenderingPlugin& IBaseEngine::GetRenderer() const
{
	return *m_pRenderer;
}

void IBaseEngine::AddObjectToRenderList(IRender* pObject)
{
	if(pObject)
	{
		m_toRender.push_back(pObject);
	}
}
void IBaseEngine::Quit()
{
	PostQuitMessage(0);
}
void IBaseEngine::RegisterScript()
{
	if(m_pVM == NULL)
	{
		m_pVM = new asVM();
	}

	asIScriptEngine& scriptEngine = GetScriptVM().GetScriptEngine();

	DBAS(scriptEngine.RegisterObjectType("IEngine",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","void Quit()",asMETHOD(IBaseEngine,Quit),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","string OpenFileName()",asMETHOD(IBaseEngine,OpenFileName),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("IEngine engine",this));

	scriptEngine.Release();
}