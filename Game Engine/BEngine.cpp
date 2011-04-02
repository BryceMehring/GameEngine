#include "StdAfx.h"

#include "BEngine.h"
#include "VertexStream.h"
#include "Singleton.h"
#include "asVM.h"

#define MAX_CONSOLE_LINES 500

IBaseEngine* IBaseEngine::s_pInstance = NULL;

DWORD WINAPI LuaConsoleInput(void* parameter)
{
	static char buffer[256];

	cin.clear();
	std::cin.getline(buffer,256,';');

	return 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	IBaseEngine* pEngine = IBaseEngine::Instance();
	
	// Don't start processing messages until the application has been created.
	if(pEngine)
	{
		pEngine->MsgProc(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

IBaseEngine::IBaseEngine(HINSTANCE hInstance,const string& winCaption) : m_hInstance(hInstance)
{
	s_pInstance = this;

	// Default values
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

	// initialize timer 
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;

	RedirectIOToConsole();

	asVM* pVM = asVM::Instance();

	/*m_hConsole = GetConsoleWindow();
	ShowWindow( m_hConsole, SW_HIDE );

	// Create Thread
	m_luaConsole = CreateThread(NULL,NULL,LuaConsoleInput,NULL,CREATE_SUSPENDED,NULL);

	// streams
	m_p3Device->AddRef();
	InitializeVertexStreams(m_p3Device);*/ // might not need to be a global function

	// Create pool to share parameters
	//D3DXCreateEffectPool(&m_pEffectPool);

	// Camera initialize 
	/*BCamera& cam = BCamera::Instance();
	cam.BuildProjectMatrix(m_D3DParameters.BackBufferHeight,m_D3DParameters.BackBufferHeight);
	cam.BuildViewMatrix(D3DXVECTOR3(3,3,3));*/

}

IBaseEngine::~IBaseEngine()
{
	
}

IBaseEngine* IBaseEngine::Instance()
{
	return s_pInstance;
}

void IBaseEngine::Delete()
{
	delete s_pInstance;
	s_pInstance = NULL;
}

HINSTANCE IBaseEngine::GetHINSTANCE() const
{
	return m_hInstance;
}

HWND IBaseEngine::GetWindowHandle() const 
{ 
	return m_hWindowHandle;
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
		WS_OVERLAPPEDWINDOW , 100, 100, m_rect.right, m_rect.bottom, 
		0, 0, hInstance, 0); 

	if( !m_hWindowHandle ) { throw std::string("RegisterClass Failed"); }

	ShowWindow(m_hWindowHandle, SW_SHOW);
	UpdateWindow(m_hWindowHandle);
}


void IBaseEngine::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
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

void IBaseEngine::InitializeAngelScript()
{

}

void IBaseEngine::ScriptingConsole(bool bOpen)
{
	/*if(bOpen)
	{
		ResumeThread(m_luaConsole);
		ShowWindow( m_hConsole, SW_SHOW );
	}
	else
	{
		ShowWindow( m_hConsole, SW_HIDE );
		SuspendThread(m_luaConsole);
	}*/
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