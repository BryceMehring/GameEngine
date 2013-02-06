#include "LineEngine.h"
#include "Streams.h"
#include "BNew.h"
#include <cassert>

LineEngine::LineEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength) : 
m_pDevice(pDevice), m_pRM(pTm), m_pVertexBuffer(nullptr), m_iMaxLength(maxLength), m_iCurrentLength(0)
{
	CreateBuffers();
}
LineEngine::~LineEngine()
{
	OnLostDevice();
}

void LineEngine::CreateBuffers()
{
	m_pDevice->CreateVertexBuffer(m_iMaxLength * sizeof(VertexP),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_pVertexBuffer,0);
}

void LineEngine::DrawLine(const D3DXVECTOR3* pArray, unsigned int length, const D3DXVECTOR4& color, const D3DXMATRIX& t)
{
	unsigned int uiNewMaxSize = m_iCurrentLength + length;
	if(uiNewMaxSize >= m_iMaxLength)
		return;

	// If this is the first DrawLine calls before rendering, we need to get a new pool of memory, to disregard the previous data in the buffer
	// If not, we will keep appending to the buffer
	DWORD flags = m_iCurrentLength == 0 ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE;

	VertexP* pVB = nullptr;

	// Locking the buffer with the correct flag,
	// note: if we are locking with the D3DLOCK_DISCARD flag, the first two parameters are ignored
	m_pVertexBuffer->Lock(m_iCurrentLength * sizeof(VertexP),length * sizeof(VertexP),(void**)&pVB,flags);

	// Transform line as we copy it to the vertex buffer
	D3DXVec3TransformCoordArray((D3DXVECTOR3*)(&pVB->pos),sizeof(D3DXVECTOR3),pArray,sizeof(D3DXVECTOR3),&t,length);

	m_pVertexBuffer->Unlock();

	m_iCurrentLength = uiNewMaxSize;
	m_lines.push_back(LineData(t,color,length));
}

void LineEngine::Render()
{
	if(m_iCurrentLength == 0)
		return;

	m_pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VertexP));
	m_pDevice->SetVertexDeclaration(VertexP::m_pVertexDecl);

	ResourceManager::Shader shader;
	m_pRM->GetResource("2dshader",shader);

	shader.pEffect->SetTechnique(shader.tech["LineTech"]);
	shader.pEffect->SetMatrix(shader.parameters["gWorldViewProj"],&(m_pCamera->viewProj()));

	UINT p = 0;
	shader.pEffect->Begin(&p,0);

	for(UINT i = 0; i < p; ++i)
	{
		shader.pEffect->BeginPass(i);

		unsigned int startVertex = 0; // Index of the first Vertex to render

		// Loop over all text subsets
		for(auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
		{
			shader.pEffect->SetVector(shader.parameters["gColor"],&iter->color);
			shader.pEffect->CommitChanges();

			m_pDevice->DrawPrimitive(D3DPT_LINESTRIP,startVertex,(iter->length - 1));
			startVertex += (iter->length);
		}
		shader.pEffect->EndPass();
	}

	shader.pEffect->End();

	m_lines.clear();

	m_iCurrentLength = 0;
}

void LineEngine::OnLostDevice()
{
	if(m_pVertexBuffer != nullptr)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
}

void LineEngine::OnResetDevice()
{
	if(m_pVertexBuffer == nullptr)
	{
		CreateBuffers();
	}
}


