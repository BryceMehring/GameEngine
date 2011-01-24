#include "StdAfx.h"

#include "BEngine.h"
#include "VertexStream.h"
#include "BFont.h"

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

BEngine* BEngine::s_pInstance;

BEngine::BEngine(HINSTANCE hInstance,const string& winCaption)
{
	// Default values
	m_hInstance = hInstance;
	m_bPaused = false;
	m_ClearBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	// initialize timer 
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	m_fSecsPerCount = 1.0f / (float)cntsPerSec;

	// Initialize
	try
	{
		InitializeWindows(m_hInstance,winCaption);
		InitializeDirectX();
	}
	catch(string error)
	{
		MessageBox(0,error.c_str(),0,0);
		exit(0);
	}

	// post-Initialize

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
	m_pDirect3D->Release();
	m_p3Device->Release();
	//m_pEffectPool->Release();

	//DeleteVertexStreams();

	/*BCamera::Delete();
	BFont::Delete();*/
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

void BEngine::Initialize(HINSTANCE hInstance,const string& winCaption)
{
	if(s_pInstance == NULL)
	{
		s_pInstance = new BEngine(hInstance,winCaption);
	}
}

void BEngine::Delete()
{
	delete s_pInstance;
	s_pInstance = NULL;
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

void BEngine::RenderOptions()
{
	// Draw font, need to add this to a function
	/*BFont& font = BFont::Instance();
	ID3DXFont* pFont = font.GetFont("default");

	RECT rec = {0,0,800,800};

	ostringstream mode;

	for(int i = 0; i < m_mode.size(); ++i)
	{
		mode << m_mode[i].Width << " x " << m_mode[i].Height << endl;
	}

	pFont->DrawText(0,mode.str().c_str(),-1,&rec,DT_WORDBREAK,D3DCOLOR_XRGB(255,255,255));*/
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
	// Before we present the screen to the screen, we must stop rendering to the back buffer
	End();

	m_p3Device->Present(0,0,0,0);
}

// 
void BEngine::EnumerateDisplayAdaptors()
{
	D3DDISPLAYMODE mode;
	for(int i = 0; i < m_pDirect3D->GetAdapterCount(); ++i)
	{
		UINT n = m_pDirect3D->GetAdapterModeCount(i,D3DFMT_X8R8G8B8);

		for(int j = 0; j < n; ++j)
		{
			m_pDirect3D->EnumAdapterModes(i,D3DFMT_X8R8G8B8,j,&mode);
			m_mode.push_back(mode);
		}
	}
}

/*bool BEngine::LoadEffect(UINT iID,const char* pFile)
{
	// 1. Check to see if the effect is already loaded
	if(m_Effects.find(iID) == m_Effects.end())
	{
		// 2. Load the effect
		ID3DXEffect* pEffect = 0;
		ID3DXBuffer* pErrors = 0;
		D3DXCreateEffectFromFile(m_p3Device,pFile,0,0,D3DXSHADER_DEBUG,m_pEffectPool,&pEffect,&pErrors);

		// 3. If there are errors, deal with them
		if(pErrors)
		{
			char* pMsg = static_cast<char*>(pErrors->GetBufferPointer());
			MessageBox(0,pMsg,0,0);
			PostQuitMessage(0);

			return false;
		}

		// 4. Insert effect into data structure
		m_Effects.insert(make_pair(iID,pEffect));

		return true;
	}

	return false;
}
 
void BEngine::AddEffectTech(UINT iEffectID, const char* pName, UINT iSubset)
{
	Effect& effect = m_Effects[iEffectID];
	effect.m_hTech[iSubset] = effect.pEffect->GetTechniqueByName(pName);
}

void BEngine::AddEffectParameter(UINT iEffectID, const char* pName, const char* pKey)
{
	Effect& effect = m_Effects[iEffectID];
	effect.m_hParameters[pKey] = effect.pEffect->GetParameterByName(0,pName);
}

// Load a mesh along with its mtrls and effects.
bool BEngine::LoadXFile(UINT iXID, UINT iEffectID, const char* pFile)
{
	Mesh mesh;

	ID3DXMesh* temp = 0;
	ID3DXMesh* pMesh = 0;

	ID3DXBuffer* pTempMtrl = 0;
	ID3DXBuffer* pAdj = 0;

	DWORD iNumMtrl = 0;

	// Load Mesh from file
	D3DXLoadMeshFromX(pFile,D3DXMESH_SYSTEMMEM,m_p3Device,&pAdj,&pTempMtrl,0,&iNumMtrl,&pMesh);

	// change vertex format
	CloneMesh(pMesh,&temp,D3DXMESH_SYSTEMMEM);
	
	pMesh = temp;
	temp = 0;

	// optimize mesh
	pMesh->Optimize(D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
		static_cast<DWORD*>(pAdj->GetBufferPointer()),0,0,0,&temp);

	pMesh = temp;
	temp = 0;

	mesh.pMesh = pMesh;

	// Get the size of the attribute buffer
	DWORD iAttribSize = 0;
	pMesh->GetAttributeTable(0,&iAttribSize);

	// alloc enough memory for all of the mtrls
	mesh.mrtl.resize(iAttribSize + 1);

	// Get Effect
	{
		Effect& eff = m_Effects[iEffectID];
		mesh.pEffect->pEffect = eff.pEffect;
	}

	// Get Pointer to mtrl to extract it
	D3DXMATERIAL* pMtrl = static_cast<D3DXMATERIAL*>(pTempMtrl->GetBufferPointer());

	for(DWORD i = 0; i < iAttribSize; ++i)
	{
		// Load Texture
		IDirect3DTexture9* pTexture = 0;

		// If There is a texture
		if(pMtrl[i].pTextureFilename)
		{
			D3DXCreateTextureFromFile(m_p3Device,pMtrl[i].pTextureFilename,&pTexture);
		}

		mesh.mrtl[i].mtrl = pMtrl[i].MatD3D;
		mesh.mrtl[i].pTexture = pTexture;
	}

	m_Meshes.insert(make_pair(iXID,mesh));
	return true;
}

bool BEngine::RenderMesh(UINT iID) const
{
	auto& mesh = m_Meshes.find(iID)->second;
	auto* pEffect = mesh.pEffect->pEffect;

	auto& parameters = mesh.pEffect->m_hParameters;
	auto& tech = mesh.pEffect->m_hTech;

	// Create World Matrix
	D3DXMATRIX W;
	D3DXMatrixTranslation(&W,mesh.pos.x,mesh.pos.y,mesh.pos.z);

	// World Inverse Transpose
	D3DXMATRIX WIT;
	D3DXMatrixInverse(&WIT,NULL,&W);
	D3DXMatrixTranspose(&WIT,&WIT);

	pEffect->SetMatrix(parameters["World"],&W);
	pEffect->SetMatrix(parameters["WIT"],&WIT);
	//pEffect->SetMatrix(parameters["WVP"],&(*pVP * W));

	// Loop through all of the subsets
	for(int j = 0; j < m_Meshes.size(); ++j)
	{
		pEffect->SetTechnique(tech[j]);

		pEffect->SetValue(parameters["gObjectMrtl"],(void*)&mesh.mrtl[j].mtrl,sizeof(D3DMATERIAL9));
		pEffect->SetTexture(parameters["gTex"],mesh.mrtl[j].pTexture);

		UINT i = 0;
		pEffect->Begin(&i,0);

		for(int p = 0; p < i; ++p)
		{
			pEffect->BeginPass(p);

			mesh.pMesh->DrawSubset(j);
		
			pEffect->EndPass();
		}
		pEffect->End();
	}

	return true;
}*/

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

	EnumerateDisplayAdaptors();

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

/*void BEngine::CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** ppClonedMesh , DWORD options)
{
	// New Vertex decl
	D3DVERTEXELEMENT9 elements[MAXD3DDECLLENGTH];
	UINT iElements = 0;
	VertexNPos::m_pVertexDecl->GetDeclaration(elements,&iElements);

	// new cloned interface
	pMesh->CloneMesh(options,elements,m_p3Device,ppClonedMesh);

	// Free memory
	pMesh->Release();
}*/

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
	 m_pFont->OnLostDevice();
}

void BEngine::OnResetDevice()
{
	m_pFont->OnResetDevice();
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