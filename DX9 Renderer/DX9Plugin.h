// Programmed By Bryce Mehring

#ifndef _DX9_PLUGIN_
#define _DX9_PLUGIN_
#pragma once

#define PLUGIN_EXPORTS

#include "IRenderer.h"
#include "PluginManager.h"
#include "ResourceManager.h"
#include "DX92DRenderer.h"
#include "Camera.h"

#include <d3d9.h>
#include <vector>

struct DisplayMode;

enum InterfaceType;

struct DrawTextInfo;

// todo: implement a deferred renderer
//http://www.ogre3d.org/tikiwiki/Deferred+Shading
//http://www.catalinzima.com/tutorials/deferred-rendering-in-xna/

//todo: I should subclass this class
class DX9Render : public IRenderer
{
public:

	//friend class IScripted<DX9Render>;

	// returns the name of the plug-in
	virtual DLLType GetPluginType() const;
	virtual const char* GetName() const { return "DX9Render"; }

	virtual int GetVersion() const;

	// displays a info box about the plug-in
	virtual void About() const;

	virtual void ClearScreen();
	virtual void Begin();
	virtual void End();
	virtual void Present();

	virtual void Reset();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual bool IsDeviceLost();

	virtual IResourceManager& GetResourceManager();
	virtual I2DRenderer& Get2DRenderer();
	//virtual I3DRenderer& Get3DRenderer();

	virtual void SetCamera(Camera*);

	// options
	virtual void ToggleFullscreen();
	virtual void EnumerateDisplayAdaptors();
	virtual UINT GetNumDisplayAdaptors() const;
	virtual void SetDisplayMode(UINT i);
	virtual const std::string& GetDisplayModeStr(UINT i) const;
	virtual void GetWinSize(POINT&) const;

	//virtual UINT GetRatioSize() const;
	//virtual float GetRatio(UINT n);

private:

	DX9Render(class PluginManager& ref);
	virtual ~DX9Render();

	class PluginManager& m_mgr;

	// Interfaces
	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;

	DX92DRenderer* m_p2DRenderer;
	ResourceManager* m_pResourceManager;

	//QuadTree m_quadTree;

	//std::vector<IDirect3DVertexDeclaration9*> m_VertexDecl;

	// Vertex Buffers
	//std::vector<IDirect3DVertexBuffer9*> m_VertexBuffers;

	//std::map<UPOINT,DisplayMode> m_DisplayModes;
	std::vector<DisplayMode> m_DisplayModes;

	DWORD m_ClearBuffers;

	D3DPRESENT_PARAMETERS m_D3DParameters;

	Camera* m_pCamera;

	// ===== Helper Funcrions =====
	void InitializeDirectX();

	void RegisterScript();

	// this method renders everything that was cached in the end function
	void RenderScene();

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

	void InitCamera();

	// todo: move this method into the window manager class.
	void SetWindowStyle();

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);


};

#endif