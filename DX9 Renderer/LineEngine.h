#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "d3dx9.h"
#include "ResourceManager.h"
#include "Camera.h"

struct LineData
{
	LineData() {}
	LineData(const D3DXMATRIX& t, const D3DXVECTOR4& c, unsigned int l) : T(t), color(c), length(l) {}

	D3DXMATRIX T;
	D3DXVECTOR4 color;
	unsigned int length;
};

class LineEngine
{
public:

	LineEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength);
	~LineEngine();

	void DrawLine(const D3DXVECTOR3* pArray, unsigned int length, const D3DXVECTOR4& color, const D3DXMATRIX& t);

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

};

#endif