
#ifndef _STREAMS_
#define _STREAMS_

#include "d3dx9.h"

struct VertexPT
{
	VertexPT() : pos(0.0f, 0.0f, 0.0f) {}
	VertexPT(const D3DXVECTOR3& p, const D3DXVECTOR2& t) : pos(p), tex(t) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};

class Streams
{
public:

	static void Initalize(IDirect3DDevice9* pDevice)
	{
		D3DVERTEXELEMENT9 VertexElements[] = 
		{
			{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
			{0,12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
			D3DDECL_END()
		};

		pDevice->CreateVertexDeclaration(VertexElements,&VertexPT::m_pVertexDecl);
	}
	static void Destroy()
	{
		VertexPT::m_pVertexDecl->Release();
	}
};

#endif // _STREAMS_
