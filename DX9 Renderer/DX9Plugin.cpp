

#include "DX9Plugin.h"
#include "PluginManager.h"
#include "FileManager.h"
#include "asVM.h"
#include "VecMath.h"
#include "TextureManager.h"
#include "D3d9types.h"
#include "WindowManager.h"

#include <sstream>

#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"Game Engine.lib")

using namespace std;

PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DX9Render(mgr);
}

struct DisplayMode
{
	D3DDISPLAYMODE mode;
	float ratio;
	std::string str;
};

enum InterfaceType
{
	Font,
	Line,
};



unsigned int color(unsigned char r, unsigned char g, unsigned char b)
{
	return D3DCOLOR_XRGB(r,g,b);
}

DX9Render::DX9Render(PluginManager& ref) : m_mgr(ref), m_p3Device(nullptr),
m_pDirect3D(nullptr),
m_pTextureManager(nullptr)
{
	// todo: need to organize constructor
	ZeroMemory(&m_D3DParameters,sizeof(D3DPRESENT_PARAMETERS));
	m_ClearBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

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

	// Load Textures
	m_p3Device->AddRef();
	m_pTextureManager = new TextureManager(m_p3Device);
	m_pTextureManager->LoadAllTexturesFromFolder("..\\Textures");

	m_p3Device->AddRef();
	m_p2DRenderer = new DX92DRenderer(m_p3Device,m_pTextureManager);

	EnumerateDisplayAdaptors();

	RegisterScript();
}
DX9Render::~DX9Render()
{
	delete m_pTextureManager;
	delete m_p2DRenderer;

	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();
	DBAS(pEngine->RemoveConfigGroup("Renderer"));
	pEngine->Release();

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

DLLType DX9Render::GetType() const
{
	return Rendering;
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
	
	m_D3DParameters.BackBufferWidth            = 0;
	m_D3DParameters.BackBufferHeight           = 0;
	m_D3DParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
	m_D3DParameters.BackBufferCount            = 1;
	m_D3DParameters.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_D3DParameters.MultiSampleQuality         = 0;
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

ITextureManager& DX9Render::GetTextureManager()
{
	return *m_pTextureManager;
}

I2DRenderer& DX9Render::Get2DRenderer()
{
	return *m_p2DRenderer;
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
	m_p2DRenderer->OnLostDevice();
}

void DX9Render::OnResetDevice()
{
	m_p2DRenderer->OnResetDevice();
}

void DX9Render::ClearScreen()
{
	m_p3Device->Clear(0,0,m_ClearBuffers,0,1.0f,0);
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

	for(int i = 0; i < m_pDirect3D->GetAdapterCount(); ++i)
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

void DX9Render::SetWindowStyle()
{
	HWND h = m_mgr.GetWindowManager().GetWindowHandle();

	if(m_D3DParameters.Windowed)
	{
		RECT R = {0,0,800,600}; 
		AdjustWindowRect(&R,WS_OVERLAPPEDWINDOW,false);

		SetWindowLong(h, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(h, HWND_TOP, 100, 100, R.right, R.bottom,
			SWP_SHOWWINDOW | SWP_NOZORDER);
	}
	else
	{
		SetWindowLong(h, GWL_STYLE, WS_POPUPWINDOW);
		SetWindowPos(h, HWND_TOP, 0, 0, 0, 0,
		SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
	}
}

void DX9Render::SetDisplayMode(UINT i)
{
	if( i > m_DisplayModes.size() )
		return;

	HWND h = m_mgr.GetWindowManager().GetWindowHandle();

	UINT Width = m_DisplayModes[i].mode.Width;
	UINT Height = m_DisplayModes[i].mode.Height;

	m_D3DParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_D3DParameters.BackBufferWidth  = Width;
	m_D3DParameters.BackBufferHeight = Height;
	m_D3DParameters.FullScreen_RefreshRateInHz = m_DisplayModes[i].mode.RefreshRate;
	m_D3DParameters.Windowed         = false;

	// todo: look at this:
	//HWND h1 = SetActiveWindow(h);
	//HWND h2 = SetFocus(h);
	//::GetActiveWindow();

	SetWindowStyle();
	
	Reset();
	//Reset();
}

void DX9Render::ToggleFullscreen()
{
	m_D3DParameters.Windowed = !m_D3DParameters.Windowed;

	if(m_D3DParameters.Windowed)
	{
		m_D3DParameters.BackBufferWidth            = 0;
		m_D3DParameters.BackBufferHeight           = 0;
		m_D3DParameters.BackBufferFormat           = D3DFMT_UNKNOWN;
		m_D3DParameters.BackBufferCount            = 1;
		m_D3DParameters.MultiSampleType            = D3DMULTISAMPLE_NONE;
		m_D3DParameters.MultiSampleQuality         = 0;
		m_D3DParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
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



/*UINT DX9Render::CreateVertexBuffer(UINT bytes,DWORD flags)
{
	IDirect3DVertexBuffer9* pBuffer = nullptr;

	m_p3Device->CreateVertexBuffer(bytes,flags,0,D3DPOOL_DEFAULT,&pBuffer,0);

	m_VertexBuffers.push_back(pBuffer);

	return m_VertexBuffers.size() - 1;
}

void* DX9Render::WriteToVertexBuffer(UINT iBufferIndex)
{
	void* pBuffer = nullptr;
	m_VertexBuffers[iBufferIndex]->Lock(0,0,&pBuffer,D3DLOCK_DISCARD);
	return pBuffer;
}

void DX9Render::Unlock(UINT iIndex)
{
	m_VertexBuffers[iIndex]->Unlock();
}

void DX9Render::DrawVertexBuffer(UINT iIndex)
{ 
	// todo: need to fix:
	//m_VertexBuffers[iIndex]->
//	m_p3Device->DrawPrimitive(::D3DPT_POINTLIST,0,
}

UINT DX9Render::CreateVertexDecl(const VertexDeclaration& decl)
{
	IDirect3DVertexDeclaration9* pDecl = nullptr;
	m_p3Device->CreateVertexDeclaration(&decl.Elements.front(),&pDecl);
	m_VertexDecl.push_back(pDecl);

	return m_VertexDecl.size() - 1;
}*/

void DX9Render::RegisterScript()
{
	asIScriptEngine* pEngine = pEngine = m_mgr.GetAngelScript().GetScriptEngine();

	pEngine->BeginConfigGroup("Renderer");

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

/*bool WindowManager::LoadEffect(UINT iID,const char* pFile)
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
 
void WindowManager::AddEffectTech(UINT iEffectID, const char* pName, UINT iSubset)
{
	Effect& effect = m_Effects[iEffectID];
	effect.m_hTech[iSubset] = effect.pEffect->GetTechniqueByName(pName);
}

void WindowManager::AddEffectParameter(UINT iEffectID, const char* pName, const char* pKey)
{
	Effect& effect = m_Effects[iEffectID];
	effect.m_hParameters[pKey] = effect.pEffect->GetParameterByName(0,pName);
}

// Load a mesh along with its mtrls and effects.
bool WindowManager::LoadXFile(UINT iXID, UINT iEffectID, const char* pFile)
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

bool WindowManager::RenderMesh(UINT iID) const
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
