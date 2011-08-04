
#include "StdAfx.h"
#include "BEngine.h"
#include "asVM.h"
#include "PluginManager.h"
#include "UI.h"

using namespace std;

#define MAX_CONSOLE_LINES 500


// static variables
BEngine* BEngine::s_pThis = nullptr;

BEngine::BEngine(HINSTANCE hInstance,const string& winCaption) :
m_hInstance(hInstance), m_pRenderer(nullptr), m_pInput(nullptr), m_hWindowHandle(NULL),
m_bPaused(false), m_fDT(0.0f), m_fStartCount(0.0f), m_fSecsPerCount(0.0f)
{
	// Initialize
	try
	{
		InitializeWindows(hInstance,winCaption);
		//RedirectIOToConsole();
		InitializeTimer();
		RegisterScript();
		InitializeUI();
		InitializePlugins();
	}
	// catch any errors
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}
	catch(bad_alloc& ba)
	{
		string buffer = "bad_alloc caught: ";
		buffer += ba.what();
		MessageBox(0,buffer.c_str(),0,0);
		exit(0);
	}
	catch(DWORD winError)
	{
		string buffer = "System Error: ";
		buffer += winError;
		MessageBox(0,buffer.c_str(),0,0);
		exit(0);
	}

	// todo: is this the best location for this?
	s_pThis = this;
}
BEngine::~BEngine()
{
	FreeConsole();
}
void BEngine::GetStringRec(const char* str, RECT& out)
{
	m_pRenderer->GetStringRec(str,out);
}
asVM& BEngine::GetScriptVM() const
{
	return *m_vm;
}
IKMInput* BEngine::GetInput() const
{
	return m_pInput;
}
void BEngine::StartCounter()
{
	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	m_fStartCount = (float)prevTimeStamp;
}
void BEngine::EndCounter()
{
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	m_fDT = (currTimeStamp - m_fStartCount)*m_fSecsPerCount;
}

bool BEngine::Update()
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
void BEngine::InitializeWindows(HINSTANCE hInstance, const string& winCaption)
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
void BEngine::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	m_pInput->Poll(msg,wParam,lparam);

	switch( msg )
	{
		/*case WM_CHAR:
		{
			//m_pUI->KeyDownCallback(char(wParam));
		
			break;
		}
		case WM_KEYUP:
		{
			system("pause");

			break;
		}*/
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
				//m_bPaused = true;
			}
			else
			{
				//m_bPaused = false;
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

// Thread Safe
string BEngine::OpenFileName() const
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
LRESULT CALLBACK BEngine::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created.
	if(s_pThis)
	{
		s_pThis->MsgProc(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void BEngine::RedirectIOToConsole()
{

	int hConHandle;

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

	ios::sync_with_stdio();

}
void BEngine::InitializeTimer()
{
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;
}
void BEngine::AddObjectToRenderList(IRender* pObject)
{
	if(pObject)
	{
		m_toRender.push_back(pObject);
	}
}
void BEngine::Quit()
{
	PostQuitMessage(0);
}
void BEngine::InitializePlugins()
{
	m_pm.reset(new PluginManager(this));

	// todo: to to write code to load all dll files in a folder, or even have the user
	// pick through OpenFileName().

	if(m_pm->LoadAllPlugins("../DLL/",".dll"))
	{
		m_pRenderer = static_cast<IRenderingPlugin*>(m_pm->GetPlugin(Rendering));
		m_pInput = static_cast<IKMInput*>(m_pm->GetPlugin(Input));
	}
}
void BEngine::InitializeConsole()
{
	UIData* pData = new UIData();
	pData->rect.left = pData->rect.top = 0;
	pData->rect.right = 600;
	pData->rect.bottom = 600;
	pData->str = "Console: ";

	m_console.reset(new asConsole(pData,m_pUI.get()));
}
void BEngine::InitializeUI()
{
	m_pUI.reset(new UI(this));

	InitializeConsole();

	UIGraph* pGraph = m_pUI->GetGraph();
	UINode* pNode = pGraph->AddVertex();
	pNode->AddElement(m_console.get());

	m_pUI->SetCurrentNode(pNode);

	pGraph->Release();
}
void BEngine::RegisterScript()
{
	m_vm.reset(new asVM());

	asIScriptEngine& scriptEngine = GetScriptVM().GetScriptEngine();

	// IEngine
	DBAS(scriptEngine.RegisterObjectType("IEngine",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","void Quit()",asMETHOD(BEngine,Quit),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterObjectMethod("IEngine","string OpenFileName()",asMETHOD(BEngine,OpenFileName),asCALL_THISCALL));
	DBAS(scriptEngine.RegisterGlobalProperty("IEngine engine",this));

	scriptEngine.Release();
}