#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "d3dx9.h"
#include "ResourceManager.h"
#include "Camera.h"

struct LineData
{
	LineData() : pArray(nullptr) {}
	LineData(const D3DXVECTOR2* p, unsigned int l, const D3DXVECTOR4& c, float a) : pArray(p), length(l), color(c), angle(a) {}

	const D3DXVECTOR2* pArray;
	unsigned int length;
	D3DXVECTOR4 color;
	float angle;
};

class LineEngine
{
public:

	LineEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength);
	~LineEngine();

	void DrawLine(const D3DXVECTOR2* pArray, unsigned int length, const D3DXVECTOR4& color, float angle);

	void SetCamera(Camera* pCam) { m_pCamera = pCam; }

	void OnLostDevice();
	void OnResetDevice(); 

	void Render();

private:

	IDirect3DDevice9* m_pDevice;
	ResourceManager* m_pRM;
	IDirect3DVertexBuffer9* m_pVertexBuffer;

	Camera* m_pCamera;
	unsigned int m_iCurrentLength;
	const unsigned int m_iMaxLength;

	std::vector<LineData> m_lines;

	void CreateBuffers();
	void FillVertexBuffer();

};

#endif