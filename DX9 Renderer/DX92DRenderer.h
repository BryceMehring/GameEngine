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

	Sprite(const D3DXMATRIX& T, const std::string& str, DWORD color, unsigned int cell) :
	T(T), texture(str), Color(color), uiCell(cell)
	{
	}

	D3DXMATRIX T;
	DWORD Color;
	unsigned int uiCell;
	std::string texture;
		
};

struct DrawTextInfo
{
	DrawTextInfo() {}
	DrawTextInfo(const std::string& str, const RECT& rect, DWORD c, DWORD f) 
		: text(str), R(rect),color(c),format(f) {}

	std::string text;
	RECT R;
	DWORD color;
	DWORD format;
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
	virtual void DrawString(const char* str, D3DXVECTOR2 pos, DWORD color); // world space
	virtual void DrawString(const char* str, const POINT& P, DWORD color);  // screen space

	// sprites
	virtual void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId = 0, DWORD color = 0xffffffff);
	
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
	ID3DXFont* m_pFont;

	Camera* m_pCamera;

	typedef std::vector<DrawTextInfo> TextContainerType;
	TextContainerType m_text;

	std::list<Sprite/*,PooledAllocator<Sprite>*/> m_sprites;

	void Render();
	void RenderSprites();
	void RenderText();
	void InitializeLine();
	void InitializeSprite();
	void InitializeFont();
};

#endif // _DX9_2DRENDERER_