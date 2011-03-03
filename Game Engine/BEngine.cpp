#include "StdAfx.h"

#include "BEngine.h"
#include "VertexStream.h"

IMPL_SINGLETON(BEngine);

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BEngine* pEngine = BEngine::GetInstance();

	// Don't start processing messages until the application has been created.
	if(pEngine)
	{
		pEngine->MsgProc(msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BEngine::BEngine(HINSTANCE hInstance,const string& winCaption)
{
	// Default values
	m_hInstance    = hInstance;
	m_bPaused      = false;
	

	// initialize timer 
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;

	// Initialize
	try
	{
		InitializeWindows(m_hInstance,winCaption);
	}
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}

	

	// streams
	/*m_p3Device->AddRef();
	InitializeVertexStreams(m_p3Device);*/ // might not need to be a global function

	// Create pool to share parameters
	//D3DXCreateEffectPool(&m_pEffectPool);

	// Camera initialize 
	/*BCamera& cam = BCamera::Instance();
	cam.BuildProjectMatrix(m_D3DParameters.BackBufferHeight,m_D3DParameters.BackBufferHeight);
	cam.BuildViewMatrix(D3DXVECTOR3(3,3,3));*/

}

BEngine::~BEngine()
{
	
}

BEngine* BEngine::GetInstance()
{
	BEngine* pInstance = NULL;

	if(s_pInstance)
	{
		pInstance = s_pInstance;
	}

	return pInstance;
}

void BEngine::Initialize(HINSTANCE hInstance,const string& winCaption)
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new BEngine(hInstance,winCaption);
	}
}

HINSTANCE BEngine::GetHINSTANCE() const
{
	return m_hInstance;
}

HWND BEngine::GetWindowHandle() const 
{ 
	return m_hWindowHandle;
}



bool BEngine::Update()
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
		else
		{
			// clear the screen, get ready to render
			Begin();
		}
	}

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

	RECT R = {0, 0, 800, 600};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_hWindowHandle = CreateWindow("D3DWndClassName", winCaption.c_str(), 
		WS_OVERLAPPEDWINDOW , 100, 100, R.right, R.bottom, 
		0, 0, hInstance, 0); 

	if( !m_hWindowHandle ) { throw std::string("RegisterClass Failed"); }

	ShowWindow(m_hWindowHandle, SW_SHOW);
	UpdateWindow(m_hWindowHandle);
}


void BEngine::MsgProc(UINT msg, WPARAM wParam, LPARAM lparam)
{
	switch( msg )
	{

		case WM_MOVE:
		{
			//RECT R;

			/*(GetWindowRect(g_pD3dApp->GetWindowHandle(),&R);
			SetCursorPos(R.right / 2 + R.left / 2,R.bottom / 2 + R.top / 2);*/

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