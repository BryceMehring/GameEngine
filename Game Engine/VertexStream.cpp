
#include "StdAfx.h"
#include "VertexStream.h"

// define static data
IDirect3DVertexDeclaration9* VertexNPos::m_pVertexDecl = 0;

void InitializeVertexStreams(IDirect3DDevice9* pDevice)
{
	// Define Vertex Decl
	D3DVERTEXELEMENT9 VertexPosElement[] =
	{
		{0,0,D3DDECLTYPE_FLOAT3,0,D3DDECLUSAGE_POSITION,0},
		{0,12,D3DDECLTYPE_FLOAT3,0,D3DDECLUSAGE_NORMAL,0},
		{0,24,D3DDECLTYPE_FLOAT2,0,D3DDECLUSAGE_TEXCOORD,0},
		D3DDECL_END()
	};

	pDevice->CreateVertexDeclaration(VertexPosElement,&VertexNPos::m_pVertexDecl);

	pDevice->Release();
}

void DeleteVertexStreams()
{
	VertexNPos::m_pVertexDecl->Release();
}