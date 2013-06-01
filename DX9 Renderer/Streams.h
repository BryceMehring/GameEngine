
#ifndef _STREAMS_
#define _STREAMS_

#include "d3dx9.h"

struct VertexP
{
	VertexP() : pos(0.0f, 0.0f, 0.0f) {}
	VertexP(const D3DXVECTOR3& p) : pos(p) {}

	D3DXVECTOR3 pos;
	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};

struct VertexPT
{
	VertexPT() : pos(0.0f, 0.0f, 0.0f) {}
	VertexPT(const D3DXVECTOR3& p, const D3DXVECTOR2& t) : pos(p), tex(t) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};

struct SpriteVertex
{
	SpriteVertex() {}
	SpriteVertex(const D3DXVECTOR3& p, const D3DXVECTOR3& t, float x, float y) : pos(p), tex(t), dx(x), dy(y) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
	float dx;
	float dy;

	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};

/*struct VertexPTC
{
	VertexPTC() : pos(0.0f, 0.0f, 0.0f), tex(0.0f, 0.0f), color(0.0f, 0.0f,0.0f,0.0f) {}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
	D3DXVECTOR4 color;
	static IDirect3DVertexDeclaration9* m_pVertexDecl;
};*/

class Streams
{
public:

	static void Initalize(IDirect3DDevice9* pDevice)
	{
		D3DVERTEXELEMENT9 VertexElementsP[] = 
		{
			{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
			D3DDECL_END()
		};

		D3DVERTEXELEMENT9 VertexElementsPT[] = 
		{
			{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
			{0,12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
			D3DDECL_END()
		};

		D3DVERTEXELEMENT9 SpriteVertex[] = 
		{
			{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
			{0,12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
			{0,20,D3DDECLTYPE_FLOAT1,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,1},
			{0,24,D3DDECLTYPE_FLOAT1,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,2},
			D3DDECL_END()
		};

		pDevice->CreateVertexDeclaration(VertexElementsP,&VertexP::m_pVertexDecl);
		pDevice->CreateVertexDeclaration(VertexElementsPT,&VertexPT::m_pVertexDecl);
		pDevice->CreateVertexDeclaration(SpriteVertex,&SpriteVertex::m_pVertexDecl);
	}
	static void Destroy()
	{
		VertexP::m_pVertexDecl->Release();
		VertexPT::m_pVertexDecl->Release();
		SpriteVertex::m_pVertexDecl->Release();
	}
};

#endif // _STREAMS_
