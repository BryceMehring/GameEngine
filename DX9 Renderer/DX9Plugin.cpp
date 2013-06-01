

#include "DX9Plugin.h"
#include "PluginManager.h"
#include "FileManager.h"
#include "asVM.h"
#include "VecMath.h"
#include "TextureManager.h"
#include "D3d9types.h"
#include "WindowManager.h"
#include "Streams.h"

#include <sstream>

#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"Game Engine.lib")

using namespace std;

PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DX9Render(mgr);
}

IDirect3DVertexDeclaration9* VertexP::m_pVertexDecl = nullptr;
IDirect3DVertexDeclaration9* VertexPT::m_pVertexDecl = nullptr;
IDirect3DVertexDeclaration9* SpriteVertex::m_pVertexDecl = nullptr;

struct DisplayMode
{
	D3DDISPLAYMODE mode;
	float ratio;
	std::string str;
};

unsigned int color(unsigned char r, unsigned char g, unsigned char b)
{
	return D3DCOLOR_XRGB(r,g,b);
}

DX9Render::DX9Render(PluginManager& ref) : m_mgr(ref), m_p3Device(nullptr),
m_pDirect3D(nullptr), m_pResourceManager(nullptr), m_pCamera(nullptr)
{
	// todo: need to organize constructor
	ZeroMemory(&m_D3DParameters,sizeof(D3DPRESENT_PARAMETERS));
	m_ClearBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

	try
	{
		DX9Render::InitializeDirectX();
	}
	catch(string what)
	{
		FileManager& fm = ::FileManager::Instance();
		fm.WriteToLog(what);
		exit(0);
	}

	Streams::Initalize(m_p3Device);

	// Load Textures
	m_p3Device->AddRef();
	m_pResourceManager = ::new ResourceManager(m_p3Device);

	m_p3Device->AddRef();
	m_p2DRenderer = ::new DX92DRenderer(m_p3Device,m_pResourceManager);

	EnumerateDisplayAdaptors();

	InitCamera();

	RegisterScript();

	//this->SetDisplayMode(0);
}
DX9Render::~DX9Render()
{
	asIScriptEngine* pEngine = m_mgr.GetAngelScript().GetScriptEngine();
	DBAS(pEngine->RemoveConfigGroup("Renderer"));
	pEngine->Release();

	//delete m_pCamera;
	Streams::Destroy();
	ReleaseCamera(m_pCamera);

	delete m_p2DRenderer;
	delete m_pResourceManager;

	m_p3Device->Release();
	m_pDirect3D->Release();
}

int DX9Render::GetVersion() const
{
	return 0;
}

void DX9Render::About() const
{
	MessageBox(nullptr,"DX9 DLL\nProgrammed By Bryce Mehring","About",MB_OK);
}

DLLType DX9Render::GetPluginType() const
{
	return RenderingPlugin;
}

void DX9Render::InitializeDirectX()
{
	// Step 1: Create the IDirect3D9 object.

	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_pDirect3D ) { throw std::string("Direct3DCreate9 Failed"); }

	//EnumerateDisplayAdaptors();
	HWND windowHandle = m_mgr.GetWindowManager().GetWindowHandle();

	// todo: need to check device caps 

	// Fill out the D3DPRESENT_PARAMETERS structure.

	DWORD levels;
	// D3DMULTISAMPLE_NONE
	// D3DMULTISAMPLE_2_SAMPLES
	m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,true,D3DMULTISAMPLE_2_SAMPLES,&levels);
	//m_pDirect3D->GetDeviceCaps(
	
	m_D3DParameters.BackBufferWidth            = 0;
	m_D3DParameters.BackBufferHeight           = 0;
	m_D3DParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
	m_D3DParameters.BackBufferCount            = 1;
	m_D3DParameters.MultiSampleType            = D3DMULTISAMPLE_2_SAMPLES;
	m_D3DParameters.MultiSampleQuality         = levels - 1;
	m_D3DParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	m_D3DParameters.hDeviceWindow              = windowHandle;
	m_D3DParameters.Windowed                   = true;
	m_D3DParameters.EnableAutoDepthStencil     = true;
	m_D3DParameters.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_D3DParameters.Flags                      = 0;
	m_D3DParameters.FullScreen_RefreshRateInHz = 0;
	m_D3DParameters.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;//D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT

	HRESULT hPass = m_pDirect3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,false);

	if(hPass == D3DERR_NOTAVAILABLE) { throw std::string("Device not supported."); }
	
	//Create the device.

	m_pDirect3D->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,           // device type
		windowHandle,          // window associated with device
		D3DCREATE_HARDWARE_VERTEXPROCESSING,   // vertex processing
		&m_D3DParameters,            // present parameters
		&m_p3Device);      // return created device

	//::ShowCursor(TRUE);
}

void DX9Render::Reset()
{
	OnLostDevice();
	m_p3Device->Reset(&m_D3DParameters);
	OnResetDevice();
}

IResourceManager& DX9Render::GetResourceManager()
{
	return *m_pResourceManager;
}

I2DRenderer& DX9Render::Get2DRenderer()
{
	return *m_p2DRenderer;
}

void DX9Render::SetCamera(Camera* pCam)
{
	if(pCam != nullptr)
	{
		if(m_pCamera != nullptr)
		{
			m_pCamera->Release();
		}

		m_pCamera = pCam;
		m_pCamera->AddRef();

		m_p2DRenderer->SetCamera(pCam);
	}
}

/*void DX9Render::DrawSprite()
{
	m_pSprite->Begin(0);
}*/

/*void WindowManager::CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** ppClonedMesh , DWORD options)
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
*/
bool DX9Render::IsDeviceLost()
{
	// Get the state of the graphics device.
	HRESULT hr = m_p3Device->TestCooperativeLevel();

	switch(hr)
	{
	case D3DERR_DEVICELOST:
		Sleep(20);
		return true;
	case D3DERR_DRIVERINTERNALERROR:
		MessageBox(0, "Internal Driver Error...Exiting", 0, 0);
		PostQuitMessage(0);
		return true;
	case D3DERR_DEVICENOTRESET:
		Reset();
	default:
		return false;
	};
}

void DX9Render::OnLostDevice()
{
	m_pResourceManager->OnLostDevice();
	m_p2DRenderer->OnLostDevice();
}

void DX9Render::OnResetDevice()
{
	m_pResourceManager->OnResetDevice();
	m_p2DRenderer->OnResetDevice();
}

void DX9Render::ClearScreen()
{
	m_p3Device->Clear(0,0,m_ClearBuffers,0x0,1.0f,0);
}

void DX9Render::Begin()
{
	if( !IsDeviceLost() )
	{
		ClearScreen();
		m_p3Device->BeginScene();
		// todo: these flags need to be modifiable
		
		m_p2DRenderer->Begin();
	}
}

void DX9Render::End()
{
	m_p2DRenderer->End();
	m_p3Device->EndScene();
}

void DX9Render::Present()
{
	// Before we present the screen to the screen, we must stop rendering to the back buffer
	m_p3Device->Present(0,0,0,0);
}

void DX9Render::EnumerateDisplayAdaptors()
{
	m_DisplayModes.clear();

	DisplayMode mode;

	float X = (float)GetSystemMetrics(SM_CXSCREEN);
	float Y = (float)GetSystemMetrics(SM_CYSCREEN);
	float fBaseRatio = X/Y;

	for(unsigned int i = 0; i < m_pDirect3D->GetAdapterCount(); ++i)
	{
		int n = m_pDirect3D->GetAdapterModeCount(i,D3DFMT_X8R8G8B8) - 1;

		for(int j = n; j >= 0; --j)
		{
			m_pDirect3D->EnumAdapterModes(i,D3DFMT_X8R8G8B8,j,&mode.mode);

			UINT width = mode.mode.Width;
			UINT height = mode.mode.Height;
			float ratio = (float)width / height;

			//if(fabsf(ratio - fBaseRatio) < 0.000001f)
			{
				UINT hz = mode.mode.RefreshRate;

				mode.ratio = ratio;

				ostringstream oss;
				oss << width << " x " << height << " " << hz << "Hz" << endl;

				mode.str = oss.str();

				//UPOINT P = { width, height };
				//m_DisplayModes.insert(make_pair(P,mode));
				m_DisplayModes.push_back(mode);
			}
		}
	}
}
UINT DX9Render::GetNumDisplayAdaptors() const
{
	return m_DisplayModes.size();
}

void DX9Render::InitCamera()
{
	D3DXVECTOR3 pos(0.0f,0.0f,-10.0f);
	D3DXVECTOR3 target(0.0f,0.0f,0.0f);
	D3DXVECTOR3 up(0.0f,1.0f,0.0f);

	m_pCamera = CreateCamera();
	m_pCamera->setLens(100.0f,100.0f,1.0f,5000.0f);
	m_pCamera->lookAt(pos,target,up);
	m_pCamera->update(0.0f);

	m_p2DRenderer->SetCamera(m_pCamera);
}

void DX9Render::SetWindowStyle()
{
	HWND h = m_mgr.GetWindowManager().GetWindowHandle();

	if(m_D3DParameters.Windowed)
	{
		RECT R = {0,0,800,600}; 
		AdjustWindowRect(&R,WS_OVERLAPPEDWINDOW,false);

		SetWindowLongPtr(h, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(h, HWND_TOP, 100, 100, R.right, R.bottom,
			SWP_SHOWWINDOW | SWP_NOZORDER);
	}
	else
	{ 
		SetWindowLongPtr(h, GWL_STYLE, WS_POPUP);
		SetWindowPos(h, HWND_TOP, 0, 0, m_D3DParameters.BackBufferWidth, m_D3DParameters.BackBufferHeight, SWP_SHOWWINDOW | SWP_NOZORDER);
	}
}

void DX9Render::SetDisplayMode(UINT i)
{
	if( i > m_DisplayModes.size() )
		return;

	UINT Width = m_DisplayModes[i].mode.Width;
	UINT Height = m_DisplayModes[i].mode.Height;

	m_D3DParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_D3DParameters.BackBufferWidth  = Width;
	m_D3DParameters.BackBufferHeight = Height;
	m_D3DParameters.FullScreen_RefreshRateInHz = m_DisplayModes[i].mode.RefreshRate;
	m_D3DParameters.Windowed         = false;

	SetWindowStyle();
	Reset();
}

void DX9Render::ToggleFullscreen()
{
	m_D3DParameters.Windowed = !m_D3DParameters.Windowed;

	if(m_D3DParameters.Windowed)
	{
		m_D3DParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
		m_D3DParameters.BackBufferWidth            = 0;
		m_D3DParameters.BackBufferHeight           = 0;
		m_D3DParameters.FullScreen_RefreshRateInHz = 0;
	}
	else
	{
		UINT Width = m_DisplayModes[0].mode.Width;
		UINT Height = m_DisplayModes[0].mode.Height;

		m_D3DParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_D3DParameters.BackBufferWidth  = Width;
		m_D3DParameters.BackBufferHeight = Height;
		m_D3DParameters.FullScreen_RefreshRateInHz = m_DisplayModes[0].mode.RefreshRate;
	}

	SetWindowStyle();

	Reset();
}

const string& DX9Render::GetDisplayModeStr(UINT i) const
{
	return m_DisplayModes[i].str;
}

void DX9Render::GetWinSize(POINT& P) const
{
	P.x = m_D3DParameters.BackBufferWidth;
	P.y = m_D3DParameters.BackBufferHeight;
}

void DX9Render::RegisterScript()
{
	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();

	pEngine->BeginConfigGroup("Renderer");

	//pEngine->RegisterObjectMethod(
	DBAS(pEngine->RegisterGlobalFunction("uint color(uint8 red, uint8 green, uint8 blue)",::asFUNCTION(color),asCALL_CDECL));

	DBAS(pEngine->RegisterObjectType("DX9Render",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterObjectMethod("DX9Render","void EnumerateDisplayAdaptors()",asMETHOD(DX9Render,EnumerateDisplayAdaptors),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("DX9Render","uint GetNumDisplayAdaptors()",asMETHOD(DX9Render,GetNumDisplayAdaptors),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("DX9Render","void SetDisplayMode(uint)",asMETHOD(DX9Render,SetDisplayMode),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("DX9Render","const string& GetDisplayModeStr(uint) const",asMETHOD(DX9Render,GetDisplayModeStr),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod("DX9Render","void tf()",asMETHOD(DX9Render,ToggleFullscreen),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("DX9Render renderer",this));

	pEngine->EndConfigGroup();

	pEngine->Release();
}