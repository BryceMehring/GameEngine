
#pragma once

#include <d3dx9.h>
#include <dinput.h>
#include <string>
#include <hash_map>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;
using namespace stdext;


struct Mtrl
{
	IDirect3DTexture9* pTexture;
	D3DXHANDLE ParamBlock;
	D3DMATERIAL9 mtrl;
};

struct Mesh
{
	ID3DXMesh* pMesh;
	ID3DXEffect* pEffect;
	vector<Mtrl> mrtl;
};


class BEngine
{
public:

	static BEngine* GetInstance();

	// Initializes DirextX along with Windows
	static bool Initialize(HINSTANCE hInstance,const string& winCaption);
	
	// Deletes everything from memory
	static bool Delete();
	
	bool Update();
	bool Begin();
	bool End();
	void Present();

	// effects              
	bool LoadEffect(UINT iID,const char* pFile);
	bool AddEffectTech(UINT iID, UINT iEffectID, const char* pName);
	bool AddEffectParameter(UINT iID, UINT iEffectID, const char* pName);

	// Mesh
	template< class T >
	bool LoadXFile(UINT iXID, UINT iEffectID, const char* pFile, T& funct);
	
	bool RenderMesh(UINT iID) const;
	
	// Texture
	bool LoadTexture(UINT iID, const char* pFile);
	bool AutoGenSphereTexCoords(UINT iMesh);
	bool AutoGenCyclTexCoords(UINT iMesh, int axis);

	// Input functions
	void Poll();
	bool KeyDown(char Key);
	bool MouseClick(int Button);

	int MouseX();
	int MouseY();
	int MouseZ();

	IDirect3DDevice9* GetDevice();

	void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

private:

	// Single Instance of the class.
	static BEngine* s_pInstance;

	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;
	ID3DXEffectPool* m_pEffectPool;


	D3DPRESENT_PARAMETERS m_D3DParameters;
	HWND m_hWindowHandle;

	DWORD m_ClearBuffers;

	bool m_bPaused;

	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	D3DXMATRIX m_View;
	D3DXMATRIX m_Proj;

	hash_map<UINT,Mesh> m_Meshes;
	hash_map<UINT,ID3DXEffect*> m_Effects;
	hash_map<UINT,IDirect3DTexture9*> m_Textures;

	// Direct input
	/*

	// Interface
	IDirectInput8* m_pDirectInput;

	// Keyboard
	IDirectInputDevice8* m_pKeyboard;
	char m_KeyboardState[256];

	// Mouse
	IDirectInputDevice8* m_pMouse;
	DIMOUSESTATE2 m_MouseState;
	D3DXVECTOR3 m_Pos;*/


	/*
	Enter something here!!!///////
	*/




	BEngine(HINSTANCE hInstance,const string& winCaption);
	~BEngine();

	// Prevent copies from being made.
	BEngine(const BEngine&);
	BEngine operator =(const BEngine&);

	void InitializeWindows(HINSTANCE hInstance, const string& winCaption);
	void InitializeDirectX();

	void InitalizeVertexFormat(); //???

	void CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** pClonedMesh, DWORD options);
	
	bool IsDeviceLost();
	void OnLostDevice();
	void OnResetDevice();

	void EnableFullscreen(bool b);

};

// Load a mesh along with its mtrls and effects.
template< class T >
bool BEngine::LoadXFile(UINT iXID, UINT iEffectID, const char* pFile, T& funct)
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
	ID3DXEffect* pEffect = m_Effects[iEffectID];
	mesh.pEffect = pEffect;

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

		// Record parameters to set for this mesh
		pEffect->BeginParameterBlock();
		funct(pEffect,mesh.mrtl[i],i);
		mesh.mrtl[i].ParamBlock = pEffect->EndParameterBlock();

	}

	m_Meshes.insert(make_pair(iXID,mesh));
	return true;
}

/*

// Switch to fullscreen mode.
	if( enable )
	{
		// Are we already in fullscreen mode?
		if( !m_D3DParameters.Windowed ) 
			return;

		int width  = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		m_D3DParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_D3DParameters.BackBufferWidth  = width;
		m_D3DParameters.BackBufferHeight = height;
		m_D3DParameters.FullScreen_RefreshRateInHz = 60;
		m_D3DParameters.Windowed         = false;

		// Change the window style to a more fullscreen friendly style.
		SetWindowLongPtr(m_hWindowHandle, GWL_STYLE, WS_POPUP);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_hWindowHandle, HWND_TOP, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);	
	}
	// Switch to windowed mode.
	else
	{
		// Are we already in windowed mode?
		if( m_D3DParameters.Windowed ) 
			return;

		RECT R = {0, 0, 800, 600};
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		m_D3DParameters.BackBufferFormat = D3DFMT_UNKNOWN;
		m_D3DParameters.BackBufferWidth  = 800;
		m_D3DParameters.BackBufferHeight = 600;
		m_D3DParameters.FullScreen_RefreshRateInHz = 0;
		m_D3DParameters.Windowed         = true;
	
		// Change the window style to a more windowed friendly style.
		SetWindowLongPtr(m_hWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);

		// If we call SetWindowLongPtr, MSDN states that we need to call
		// SetWindowPos for the change to take effect.  In addition, we 
		// need to call this function anyway to update the window dimensions.
		SetWindowPos(m_hWindowHandle, HWND_TOP, 100, 100, R.right, R.bottom, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	// Reset the device with the changes.
	OnLostDevice();
	g_p3dDevice->Reset(&m_D3DParameters);
	OnResetDevice();*/