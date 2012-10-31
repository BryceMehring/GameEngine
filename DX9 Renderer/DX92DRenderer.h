#ifndef _DX92DRENDERER_
#define _DX92DRENDERER_

#include "IRenderer.h"
#include "TextureManager.h"
#include "PooledAllocator.h"
#include <list>
#include <vector>

struct Sprite
{
	// default ctor
	Sprite()
	{
	}

	Sprite(const D3DXMATRIX& T, const std::string& str, DWORD color, unsigned int Priority, unsigned int cell) :
	T(T), texture(str), Color(color), uiPriority(Priority), uiCell(cell)
	{
	}

	D3DXMATRIX T;
	DWORD Color;
	unsigned int uiPriority;
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

class SpriteSorter
{
public:

	// todo: could implement switch to switch draw order

	bool operator()(const Sprite& a, const Sprite& b) const
	{
		return a.uiPriority > b.uiPriority;
	}

};

class DX92DRenderer : public I2DRenderer
{
public:

	DX92DRenderer(IDirect3DDevice9* pDevice, TextureManager* pTm);
	virtual ~DX92DRenderer();

	// Line
	virtual void DrawLine(const D3DXVECTOR2* pVertexList, DWORD dwVertexListCount, D3DCOLOR color);
	virtual void DrawLine(const D3DXVECTOR3* pVertexList, DWORD dwVertexListCount, D3DCOLOR color);

	// Fonts
	virtual void GetStringRec(const char* str, RECT& out);
	virtual void DrawString(const char* str, POINT P, DWORD color); // not clipped
	virtual void DrawString(const char* str, RECT& R, DWORD color, bool calcRect = true); // clipped to rect

	// sprites
	virtual void DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iPriority, unsigned int iCellId = 0, DWORD color = 0xffffffff);
	

	void Begin();
	void End();

	void OnLostDevice();
	void OnResetDevice();

protected:

	IDirect3DDevice9* m_pDevice;
	TextureManager* m_pTextureManager;

	ID3DXSprite* m_pSprite;
	ID3DXLine* m_pLine;
	ID3DXFont* m_pFont;

	typedef std::vector<DrawTextInfo> TextContainerType;
	TextContainerType m_text;

	std::list<Sprite,PooledAllocator<Sprite>> m_sprites;

	void Render();
	void RenderSprites();
	void RenderText();
	void InitializeLine();
	void InitializeSprite();
	void InitializeFont();
};

#endif // _DX9_2DRENDERER_