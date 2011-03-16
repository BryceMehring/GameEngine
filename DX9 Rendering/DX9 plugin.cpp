
#include "DX9 plugin.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"Plugin Manager.lib")

PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr)
{
	return new DX9Render(mgr);
}

DX9Render::DX9Render(PluginManager& ref) : m_mgr(ref)
{
	m_p3Device = NULL;
	m_pDirect3D = NULL;
	m_pFont = NULL;

	ZeroMemory(&m_D3DParameters,sizeof(D3DPRESENT_PARAMETERS));
	m_ClearBuffers = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	DX9Render::InitializeDirectX();

	// post-Initialize
	D3DXFONT_DESC desc;
	ZeroMemory(&desc,sizeof(desc));
	D3DXCreateFontIndirect(m_p3Device,&desc,&m_pFont);
}
DX9Render::~DX9Render()
{
	m_pFont->Release();
	m_p3Device->Release();
	m_pDirect3D->Release();
}

void DX9Render::About()
{

}

void DX9Render::GetName(std::string& name)
{

}

void DX9Render::InitializeDirectX()
{
	// Step 1: Create the IDirect3D9 object.

	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( !m_pDirect3D ) { throw std::string("Direct3DCreate9 Failed"); }

	//EnumerateDisplayAdaptors();

	HWND windowHandle = m_mgr.GetWindowHandle();

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
	m_D3DParameters.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;//D3DPRESENT_INTERVAL_IMMEDIATE, D3DPRESENT_INTERVAL_DEFAULT

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
}

void DX9Render::DrawString(const char* str, const POINT& point, DWORD color)
{
	RECT r = {point.x,point.y,point.x+20,point.y+20};
	m_pFont->DrawText(0,str,-1,&r,DT_NOCLIP,color);
}
/*void DX9Render::DrawSprite()
{
	m_pSprite->Begin(0);
}*/

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
	 m_pFont->OnLostDevice();
}

void BEngine::OnResetDevice()
{
	m_pFont->OnResetDevice();
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

	pFont->DrawText(0,mode.str().c_str(),-1,&rec,DT_WORDBREAK,D3DCOLOR_XRGB(255,255,255));

	*/
void DX9Render::Begin()
{
	//if( !IsDeviceLost() )
	{
		m_p3Device->Clear(0,0,m_ClearBuffers,0,1.0f,0);
		m_p3Device->BeginScene();
	}
}

void DX9Render::End()
{
	m_p3Device->EndScene();
}

void DX9Render::Present()
{
	// Before we present the screen to the screen, we must stop rendering to the back buffer
	m_p3Device->Present(0,0,0,0);
}

// 
/*void BEngine::EnumerateDisplayAdaptors()
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

bool BEngine::LoadEffect(UINT iID,const char* pFile)
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