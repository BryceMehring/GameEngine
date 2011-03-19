// Programmed By Bryce Mehring

#ifndef _DX9_PLUGIN_
#define _DX9_PLUGIN_
#pragma once

#define PLUGIN_EXPORTS
#include "PluginManager.h"

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

// implement a deferred renderer
//http://www.ogre3d.org/tikiwiki/Deferred+Shading
//http://www.catalinzima.com/tutorials/deferred-rendering-in-xna/

class DX9Render : public IRenderingPlugin
{
public:

	// returns the name of the plug-in
	virtual void GetName(std::string& name);

	// displays a info box about the plug-in
	virtual void About();

	// mesh functions
	//virtual int LoadMesh(char* pFile);
	//virtual void DrawMesh(int id, int x, int y, int z);

	//shaders
	//virtual int LoadShader(char* pFile);

	virtual void Begin();
	virtual void End();
	virtual void Present();
	virtual void DrawString(const char* str, const POINT& point, DWORD color);
	//virtual void DrawSprite();

protected:

	DX9Render(PluginManager& ref);
	virtual ~DX9Render();

	// ===== Data members =====

	/*hash_map<UINT,Mesh> m_Meshes;
	hash_map<UINT,Effect> m_Effects;
	hash_map<UINT,IDirect3DTexture9*> m_Textures;

	vector<D3DDISPLAYMODE> m_mode;*/

	/*
	Enter something here!!!///////
	*/

	PluginManager& m_mgr;

	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;
	ID3DXFont* m_pFont;
	ID3DXSprite* m_pSprite;

	DWORD m_ClearBuffers;

	D3DPRESENT_PARAMETERS m_D3DParameters;


	// ===== Helper Funcrions =====

	void InitializeDirectX();

	//void InitalizeVertexFormat(); //???

	//void CloneMesh(ID3DXMesh* pMesh, ID3DXMesh** pClonedMesh, DWORD options);

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

	//void EnableFullscreen(bool b);

	//void RenderOptions();

	//void EnumerateDisplayAdaptors();

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);

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

#endif