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

	VertexP* pVB = nullptr;
	m_pVertexBuffer->Lock(0,0,(void**)&pVB,0);
	//m_pVertexBuffer->Lock(m_iCurrentLength * sizeof(VertexP),length * sizeof(VertexP),(void**)&pVB,D3DLOCK_NOOVERWRITE);

	D3DXVec3TransformCoordArray((D3DXVECTOR3*)(pVB + m_iCurrentLength),sizeof(VertexP),pArray,sizeof(D3DXVECTOR3),&t,length);

	m_pVertexBuffer->Unlock();

	m_iCurrentLength = uiNewMaxSize;
	m_lines.push_back(LineData(t,color,length));
}

void LineEngine::FillVertexBuffer()
{
	
}


/*void LineEngine::DrawLine(const D3DXVECTOR2* pArray, unsigned int length, const D3DXVECTOR4& color)
{
	assert(length != 0 && pArray != nullptr);

	unsigned int bytes = sizeof(D3DXVECTOR2) * length;
	D3DXVECTOR2* pMemory = (D3DXVECTOR2*)Bnew(bytes);
	memcpy(pMemory,pArray,bytes);

	m_lines.push_back(LineData(pMemory,length,color));
}

void LineEngine::FillVertexBuffer()
{
	VertexP* pVB = nullptr;
	m_pVertexBuffer->Lock(0,0,(void**)&pVB,D3DLOCK_DISCARD);

	unsigned int index = 0;

	for(auto iter = m_lines.begin(); (index < m_iMaxLength) && iter != m_lines.end(); ++iter)
	{
		for(unsigned int i = 0; (index < m_iMaxLength) && (i < iter->length); ++i, ++index)
		{
			pVB[index].pos = D3DXVECTOR3(iter->pArray[i].x,iter->pArray[i].y,0.0f);
		}

		BDelete((void*)iter->pArray);
	}

	m_pVertexBuffer->Unlock();

	m_iCurrentLength = index - 1;
}*/

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

		unsigned int startVertex = 0; 
		for(auto iter = m_lines.begin(); iter != m_lines.end(); ++iter)
		{
			shader.pEffect->SetVector(shader.parameters["gColor"],&iter->color);
			shader.pEffect->CommitChanges();

			m_pDevice->DrawPrimitive(D3DPT_LINESTRIP,startVertex,(iter->length - 1));
			startVertex += iter->length;
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


