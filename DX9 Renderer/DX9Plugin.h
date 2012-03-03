// Programmed By Bryce Mehring

#ifndef _DX9_PLUGIN_
#define _DX9_PLUGIN_
#pragma once

#define PLUGIN_EXPORTS

#include <d3d9.h>
#include <string>
#include <vector>
#include <map>

#include "IRenderer.h"
#include "IScripted.h"
#include "PluginManager.h"


struct DisplayMode
{
	D3DDISPLAYMODE mode;
	float ratio;
	std::string str;
	std::string ratioStr;
};

enum InterfaceType
{
	Font,
	Line,
};

struct DrawTextInfo
{
	const char* pText;
	//std::string text;
	RECT R;
	DWORD color;
	DWORD format;
};

// todo: implement a deferred renderer
//http://www.ogre3d.org/tikiwiki/Deferred+Shading
//http://www.catalinzima.com/tutorials/deferred-rendering-in-xna/

// I should subclass this class
class DX9Render : public IRenderer, public IScripted<DX9Render>
{
public:

	//friend class IScripted<DX9Render>;

	// returns the name of the plug-in
	virtual DLLType GetType() const;

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

	virtual void Reset();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual bool IsDeviceLost();

	// fonts
	virtual void GetStringRec(const char* str, RECT& out);
	
	virtual void DrawString(const char* str, POINT P, DWORD color);
	virtual void DrawString(const char* str, RECT& R, DWORD color, bool calcRect = true);
	
	// todo: need to implement?
	//template< class T >
	//virtual void DrawString(T&, const D3DXMATRIX& transform);

	//virtual void DrawSprite();

	// lines
	virtual void DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color);
	virtual void DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, D3DXMATRIX* pTransform , D3DCOLOR color);

	// effects
	// todo: need to implement
	virtual UINT CreateEffectFromFile(const char* file) { return 0; }
	virtual UINT GetTechnique(UINT n, const char* name) { return 0; }
	virtual void SetTechnique(UINT n) {}
	virtual void SetValue(void* pData, UINT bytes) {}

	// Meshes
	// todo: need to implement
	virtual UINT CreateMeshFromFile(const char* file) { return 0; }
	virtual UINT CreateTriGridMesh() { return 0; }

	// options
	virtual void EnumerateDisplayAdaptors();
	virtual void SetDisplayMode(float ratio, UINT i);
	virtual const std::string& GetDisplayModeStr(float ratio, UINT i) const;
	virtual UINT GetModeSize(float ratio) const;

	//virtual UINT GetRatioSize() const;
	//virtual float GetRatio(UINT n);

protected:

	DX9Render(class PluginManager& ref);
	virtual ~DX9Render();
	// ===== Data members =====

	/*hash_map<UINT,Mesh> m_Meshes;
	hash_map<UINT,Effect> m_Effects;
	hash_map<UINT,IDirect3DTexture9*> m_Textures;

	*/

	/*
	Enter something here!!!///////
	*/

	class PluginManager& m_mgr;

	D3DXMATRIX T;

	// Interfaces
	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;

	// fonts
	ID3DXFont* m_pFont; // todo: I need to match these with sprites!!! this will solve the problem of scrolling
	typedef std::vector<DrawTextInfo> TextContainerType; 
	TextContainerType m_text;
	
	ID3DXSprite* m_pSprite;
	ID3DXLine* m_pLine;

	typedef std::map<float,std::vector<DisplayMode>> DisplayModeContainerType;// todo: use this or the keyword auto?
	DisplayModeContainerType m_mode;
	std::vector<float> m_modeKeyIndex;

	DWORD m_ClearBuffers;

	D3DPRESENT_PARAMETERS m_D3DParameters;

	// ===== Helper Funcrions =====
	void InitializeFont();
	void InitializeLine();
	void InitializeSprite();
	void InitializeDirectX();

	static void _RegisterScript();

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

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);

};

#endif