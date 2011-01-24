
#pragma once

#include "StdAfx.h"

#include "BCamera.h"
#include "Interfaces.h"
#include "Singleton.h"

using namespace std;
using namespace stdext;

/*enum EEffect
{
	// Transformations
	World,
	WorldViewProj,
	WorldInverseTranspose,

};



struct Effect
{

	ID3DXEffect* pEffect;

	D3DXHANDLE* pTech;
	unsigned int iNumTech;

	// maybe can use ints in the future for a faster search, with enums - they are not dynamic ???
	hash_map<UINT,D3DXHANDLE> m_hTech;
	hash_map<string,D3DXHANDLE> m_hParameters;
};

struct Mesh
{
	ID3DXMesh* pMesh;
	Effect* pEffect;
	IFunct* pFunct;
	
	D3DXVECTOR3 pos;
	
	vector<Mtrl> mrtl;
};*/


// Base pugin


// ************************
// This really should just manage everything, do not implement it here
// All of the different sub parts of the engine should be independent, they should be their own dll.
// Then this class would load the dll's dynamically and be the leader of the game without knowing its implementation details.
// ***********************
class BEngine
{
public:

	static BEngine* GetInstance();

	// Initializes DirextX along with Windows
	static void Initialize(HINSTANCE hInstance,const string& winCaption);
	
	// Deletes everything from memory
	static void Delete();
	
	bool Update();
	void Present();

	void EnumerateDisplayAdaptors();

	// effects              
	/*bool LoadEffect(UINT iID,const char* pFile);
	
	void AddEffectTech(UINT iEffectID, const char* pName, UINT iSubset); // ?? questionable
	void AddEffectParameter(UINT iEffectID, const char* pName, const char* pKey);
	//bool AddEffectParameter(UINT iEffectID, const char* pName, UINT iIndex, const char* pKey);

	// Mesh
	bool LoadXFile(UINT iXID, UINT iEffectID, const char* pFile);
	bool RenderMesh(UINT iID) const;
	
	// Texture
	bool LoadTexture(UINT iID, const char* pFile);
	bool AutoGenSphereTexCoords(UINT iMesh);
	bool AutoGenCyclTexCoords(UINT iMesh, int axis);*/

	IDirect3DDevice9* GetDevice();

	void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	HINSTANCE GetHINSTANCE() const { return m_hInstance; }
	HWND GetWindowHandle() const { return m_hWindowHandle; }

private:

	// Single Instance of the class.
	static BEngine* s_pInstance;

	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;
	ID3DXFont* m_pFont;

	vector<D3DDISPLAYMODE> m_mode;

	D3DPRESENT_PARAMETERS m_D3DParameters;
	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;

	DWORD m_ClearBuffers;

	bool m_bPaused;

	float m_fDT;
	float m_fStartCount;
	float m_fSecsPerCount;

	/*hash_map<UINT,Mesh> m_Meshes;
	hash_map<UINT,Effect> m_Effects;
	hash_map<UINT,IDirect3DTexture9*> m_Textures;*/

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

	bool Begin();
	bool End();

	void RenderOptions();

	//void InitalizeVertexFormat(); //???

	//void CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** pClonedMesh, DWORD options);
	
	bool IsDeviceLost();
	void OnLostDevice();
	void OnResetDevice();

	//void EnableFullscreen(bool b);

};

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