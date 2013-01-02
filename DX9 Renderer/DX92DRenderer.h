#ifndef _DX92DRENDERER_
#define _DX92DRENDERER_

#include "IRenderer.h"
#include "ResourceManager.h"
#include "PooledAllocator.h"
#include <list>
#include <vector>

struct Sprite
{
	// default ctor
	Sprite()
	{
	}

	Sprite(const D3DXMATRIX& T, const std::string& str, unsigned int cell, float dx, float dy, DWORD color) :
	T(T), texture(str), Color(color), uiCell(cell), dx(dx), dy(dy)
	{
	}

	D3DXMATRIX T;
	DWORD Color;
	unsigned int uiCell;
	float dx;
	float dy;
	std::string texture;
		
};

struct DrawTextInfo
{
	DrawTextInfo() {}
	DrawTextInfo(const std::string& str, const D3DXVECTOR2& p, const ::D3DXVECTOR4& c) : text(str), pos(p), color(c), length(0) {}

	string text;
	D3DXVECTOR2 pos;
	D3DXVECTOR4 color;
	unsigned int length;
};

class FontEngine
{
public:

	FontEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength);
	~FontEngine();

	void DrawString(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR4& color);

	void Render(); 

	void OnLostDevice();
	void OnResetDevice(); 

	void SetCamera(Camera* pCam) { m_pCamera = pCam; }

private:

	IDirect3DDevice9* m_pDevice;
	ResourceManager* m_pRM;
	IDirect3DVertexBuffer9* m_pVertexBuffer;
	IDirect3DIndexBuffer9* m_pIndexBuffer;
	Camera* m_pCamera;
	const unsigned int m_iMaxLength;

	std::vector<DrawTextInfo> m_textSubsets;

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateBuffers();

	void FillVertexBuffer();

};

class DX92DRenderer : public I2DRenderer
{
public:

	DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm);
	virtual ~DX92DRenderer();

	// Line
	//virtual void DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color);
	virtual void DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, float angle, D3DCOLOR color);

	// Fonts
	virtual void GetStringRec(const char* str, RECT& out);
	virtual void DrawString(const char* str, D3DXVECTOR2 pos, const D3DXVECTOR4& color = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f)); 

	// sprites
	virtual void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId = 0, float dx = 1.0f, float dy = 1.0f, DWORD color = 0xffffffff);
	
	void Begin();
	void End();

	void OnLostDevice();
	void OnResetDevice();

	void SetCamera(Camera* pCam);

protected:

	IDirect3DDevice9* m_pDevice;
	ResourceManager* m_pResourceManager;

	ID3DXMesh* m_pMesh;
	ID3DXLine* m_pLine;

	FontEngine m_fonts;

	Camera* m_pCamera;

	std::vector<Sprite> m_sprites;

	void Render();
	void RenderSprites();
	void InitializeLine();
	void InitializeSprite();
};

#endif // _DX9_2DRENDERER_