#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "ResourceManager.h"
#include "camera.h"
#include "VecMath.h"

struct DrawTextInfo
{
	DrawTextInfo() {}
	DrawTextInfo(const std::string& str, const std::string& f, const D3DXVECTOR2& p, const D3DXVECTOR2& s, const ::D3DXVECTOR4& c) :
	text(str), font(f), pos(p), scale(s), color(c), length(0) {}

	std::string text;
	std::string font;
	D3DXVECTOR2 pos;
	D3DXVECTOR2 scale;
	D3DXVECTOR4 color;
	unsigned int length;
};

class FontEngine
{
public:

	FontEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength);
	~FontEngine();

	void GetStringRect(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out);
	void DrawString(const char* str, const char* font, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, const D3DXVECTOR4& color);

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

	D3DXVECTOR2 GetTextOffset(const D3DXVECTOR2& scale, unsigned int i, unsigned int j);

	void FillVertexBuffer();

};

#endif