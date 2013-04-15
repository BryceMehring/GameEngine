
#include "BEngine.h"

struct VertexNPos
{
	VertexNPos() : pos(0.0f, 0.0f, 0.0f) {}
	VertexNPos(float x, float y, float z) : pos(x,y,z) {}
	VertexNPos(const D3DXVECTOR3& v) : pos(v) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXVECTOR2 texcoord;
	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};


void InitializeVertexStreams(IDirect3DDevice9* pDevice);
void DeleteVertexStreams();