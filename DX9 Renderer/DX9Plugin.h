// Programmed By Bryce Mehring

#ifndef _DX9_PLUGIN_
#define _DX9_PLUGIN_
#pragma once

#define PLUGIN_EXPORTS

#include <d3d9.h>
#include <string>
#include <vector>
#include <map>
#include <queue>

#include "IRenderer.h"
#include "IScripted.h"
#include "PluginManager.h"

struct DisplayMode;

enum InterfaceType;

struct DrawTextInfo;

// todo: implement a deferred renderer
//http://www.ogre3d.org/tikiwiki/Deferred+Shading
//http://www.catalinzima.com/tutorials/deferred-rendering-in-xna/




// I should subclass this class
class DX9Render : public IRenderer
{
public:

	//friend class IScripted<DX9Render>;

	// returns the name of the plug-in
	virtual DLLType GetType() const;
	virtual const char* GetName() const { return "DX9Render"; }

	virtual int GetVersion() const;

	// displays a info box about the plug-in
	virtual void About() const;

	// mesh functions
	//virtual int LoadMesh(char* pFile);
	//virtual void DrawMesh(int id, int x, int y, int z);

	//shaders
	//virtual int LoadShader(char* pFile);
	virtual void ClearScreen();
	virtual void Begin();
	virtual void End();
	virtual void Present();

	virtual void Reset();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual bool IsDeviceLost();

	// strings

	// screen space
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

	// sprites
	//virtual void SetSpriteTransformation(float s, float r);
	virtual void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iPriority, DWORD color = 0xffffffff);

	virtual class TextureManager* GetTextureManager();

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
	virtual UINT GetNumDisplayAdaptors() const;
	virtual void SetDisplayMode(UINT i);
	virtual const std::string& GetDisplayModeStr(UINT i) const;
	virtual void GetWinSize(POINT&) const;

	// vertex buffer
	virtual UINT CreateVertexBuffer(UINT bytes,DWORD flags);
	virtual void* WriteToVertexBuffer(UINT iBufferIndex);
	virtual void Unlock(UINT iIndex);
	virtual void DrawVertexBuffer(UINT iIndex);

	virtual UINT CreateVertexDecl(const VertexDeclaration& decl);

	virtual void ToggleFullscreen();

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

	// Interfaces
	IDirect3DDevice9* m_p3Device;
	IDirect3D9* m_pDirect3D;

	ID3DXSprite* m_pSprite;
	ID3DXLine* m_pLine;

	IDirect3DTexture9* m_pTexture;

	// fonts

	ID3DXFont* m_pFont; // todo: I need to match these with sprites!!! this will solve the problem of scrolling
	typedef std::vector<DrawTextInfo> TextContainerType; 
	TextContainerType m_text;

	TextureManager* m_pTextureManager;

	struct Sprite
	{
		// default ctor
		Sprite()
		{
		}

		Sprite(const D3DXMATRIX& T, const std::string& str, DWORD color, unsigned int Priority) :
		T(T), texture(str), Color(color), uiPriority(Priority)
		{
		}

		D3DXMATRIX T;
		DWORD Color;
		unsigned int uiPriority;
		std::string texture;
		
	};

	class SpriteSorter
	{
	public:

		// todo: could implement switch to switch draw order

		bool operator()(const Sprite& a, const Sprite& b) const
		{
			return a.uiPriority > b.uiPriority;
		}

	};

	std::priority_queue<Sprite,std::vector<Sprite>,SpriteSorter> m_sprites;

	std::vector<IDirect3DVertexDeclaration9*> m_VertexDecl;

	// Vertex Buffers
	std::vector<IDirect3DVertexBuffer9*> m_VertexBuffers;

	std::vector<DisplayMode> m_DisplayModes;

	DWORD m_ClearBuffers;

	D3DPRESENT_PARAMETERS m_D3DParameters;

	// ===== Helper Funcrions =====
	void InitializeFont();
	void InitializeLine();
	void InitializeSprite();
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

	friend PLUGINDECL IPlugin* CreatePlugin(PluginManager&);

private:

	void RenderText();
	void RenderSprites();


};

#endif