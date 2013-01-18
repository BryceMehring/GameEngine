#ifndef _DX92DRENDERER_
#define _DX92DRENDERER_

#include "IRenderer.h"
#include "FontEngine.h"
#include "LineEngine.h"
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


class DX92DRenderer : public I2DRenderer
{
public:

	DX92DRenderer(IDirect3DDevice9* pDevice, ResourceManager* pTm);
	virtual ~DX92DRenderer();

	// Line
	virtual void DrawLine(const D3DXVECTOR3* pArray, unsigned int length);
	virtual void DrawLine(const D3DXVECTOR3* pArray, unsigned int length, const D3DXVECTOR4& color, const D3DXMATRIX& t);

	// Fonts
	virtual void GetStringRec(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out);
	virtual void DrawString(const char* str, const D3DXVECTOR2& pos,  const D3DXVECTOR2& scale = D3DXVECTOR2(4.0f,4.0f), const D3DXVECTOR4& color = D3DXVECTOR4(1.0f,1.0f,1.0f,1.0f)); 

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

	FontEngine m_fonts;
	LineEngine m_lines;

	Camera* m_pCamera;

	std::vector<Sprite> m_sprites;

	void Render();
	void RenderSprites();
	void InitializeSprite();
};

#endif // _DX9_2DRENDERER_