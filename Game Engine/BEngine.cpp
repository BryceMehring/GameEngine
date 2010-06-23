
#include "BEngine.h"
#include "VertexStream.h"

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

// Define static data
BEngine* BEngine::s_pInstance;

BEngine::BEngine(HINSTANCE hInstance,const string& winCaption)
{
	// Default values
	m_bPaused = false;
	m_ClearBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	// initialize timer 
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;

	// Initialize
	try
	{
		InitializeWindows(hInstance,winCaption);
		InitializeDirectX();
	}
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}

	// post-Initialize
	m_p3Device->AddRef();
	InitializeVertexStreams(m_p3Device); // might not need to be a global function

	// Create pool to share parameters
	D3DXCreateEffectPool(&m_pEffectPool);

	// create transformation matrices
	float X = (float)m_D3DParameters.BackBufferWidth;
	float Y = (float)m_D3DParameters.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&m_Proj,D3DX_PI * 0.25f,X/Y,1.0f,5000.0f);

}

BEngine::~BEngine()
{
	m_pDirect3D->Release();
	m_p3Device->Release();
	m_pEffectPool->Release();

	DeleteVertexStreams();
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

IDirect3DDevice9* BEngine::GetDevice()
{
	m_p3Device->AddRef();
	return m_p3Device;
}

bool BEngine::Initialize(HINSTANCE hInstance,const string& winCaption)
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new BEngine(hInstance,winCaption);
		return true;
	}
	return false;
}

bool BEngine::Delete()
{
	if(s_pInstance)
	{
		delete s_pInstance;
		return true;
	}

	return false;
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
    }

	return true;
}

bool BEngine::Begin()
{
	if( !IsDeviceLost() )
	{
		m_p3Device->Clear(0,0,m_ClearBuffers,0,1.0f,0);
		m_p3Device->BeginScene();

		__int64 prevTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

		m_fStartCount = prevTimeStamp;

		return true;
	}
	return false;
}

bool BEngine::End()
{
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	m_fDT = (currTimeStamp - m_fStartCount)*m_fSecsPerCount;

	m_p3Device->EndScene();
		
	return true;
}

void BEngine::Present()
{
	m_p3Device->Present(0,0,0,0);
}

bool BEngine::LoadEffect(UINT iID,const char* pFile)
{
	if(m_Effects.find(iID) == m_Effects.end())
	{
		ID3DXEffect* pEffect = 0;
		ID3DXBuffer* pErrors = 0;
		D3DXCreateEffectFromFile(m_p3Device,pFile,0,0,D3DXSHADER_DEBUG,m_pEffectPool,&pEffect,&pErrors);

		if(pErrors)
		{
			char* pMsg = static_cast<char*>(pErrors->GetBufferPointer());
			MessageBox(0,pMsg,0,0);
			PostQuitMessage(0);

			return false;
		}

		m_Effects.insert(make_pair(iID,pEffect));

		return true;
	}

	return false;
}

bool BEngine::RenderMesh(UINT iID) const
{
	const Mesh& mesh = m_Meshes.find(iID)->second;
	ID3DXEffect* pEffect = mesh.pEffect;

	// Loop through all of the subsets
	for(int j = 0; j < m_Meshes.size(); ++j)
	{
		pEffect->ApplyParameterBlock(mesh.mrtl[j].ParamBlock);

		UINT i = 0;
		pEffect->Begin(&i,0);

		for(int p = 0; p < i; ++p)
		{
			pEffect->BeginPass(p);

			mesh.pMesh->DrawSubset(j);
		
			pEffect->EndPass();
		}
		pEffect->End();
		pEffect->EndParameterBlock();
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

void BEngine::InitializeDirectX()
{
	// Step 1: Create the IDirect3D9 object.

    m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_pDirect3D ) { throw std::string("Direct3DCreate9 Failed"); }

	// Fill out the D3DPRESENT_PARAMETERS structure.
	
	m_D3DParameters.BackBufferWidth            = 0;
	m_D3DParameters.BackBufferHeight           = 0;
	m_D3DParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
	m_D3DParameters.BackBufferCount            = 1;
	m_D3DParameters.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_D3DParameters.MultiSampleQuality         = 0;
	m_D3DParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	m_D3DParameters.hDeviceWindow              = m_hWindowHandle;
	m_D3DParameters.Windowed                   = true;
	m_D3DParameters.EnableAutoDepthStencil     = true;
	m_D3DParameters.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_D3DParameters.Flags                      = 0;
	m_D3DParameters.FullScreen_RefreshRateInHz = 0;
	m_D3DParameters.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT

	HRESULT hPass = m_pDirect3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,false);

	if(hPass == D3DERR_NOTAVAILABLE) { throw string("Device not supported."); }
	
	//Create the device.

	m_pDirect3D->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,           // device type
		m_hWindowHandle,          // window associated with device
		D3DCREATE_HARDWARE_VERTEXPROCESSING,   // vertex processing
	    &m_D3DParameters,            // present parameters
	    &m_p3Device);      // return created device
}

void BEngine::CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** ppClonedMesh , DWORD options)
{
	// New Vertex decl
	D3DVERTEXELEMENT9 elements[MAXD3DDECLLENGTH];
	UINT iElements = 0;
	VertexNPos::m_pVertexDecl->GetDeclaration(elements,&iElements);

	// new cloned interface
	pMesh->CloneMesh(options,elements,m_p3Device,ppClonedMesh);

	// Free memory
	pMesh->Release();
}

bool BEngine::IsDeviceLost()
{
	// Get the state of the graphics device.
	HRESULT hr = m_p3Device->TestCooperativeLevel();

	// If the device is lost and cannot be reset yet then
	// sleep for a bit and we'll try again on the next 
	// message loop cycle.
	if( hr == D3DERR_DEVICELOST )
	{
		Sleep(20);
		return true;
	}
	// Driver error, exit.
	else if( hr == D3DERR_DRIVERINTERNALERROR )
	{
		MessageBox(0, "Internal Driver Error...Exiting", 0, 0);
		PostQuitMessage(0);
		return true;
	}
	// The device is lost but we can reset and restore it.
	else if( hr == D3DERR_DEVICENOTRESET )
	{
		OnLostDevice();
		m_p3Device->Reset(&m_D3DParameters);
		OnResetDevice();
		return false;
	}
	else
	{
		return false;
	}
}

void BEngine::OnLostDevice()
{
	 
}

void BEngine::OnResetDevice()
{

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