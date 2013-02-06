
#include "FontEngine.h"
#include "Streams.h"
#include <cassert>

FontEngine::FontEngine(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength)
	: m_pDevice(pDevice), m_pRM(pTm), m_pVertexBuffer(nullptr), m_pIndexBuffer(nullptr),
	  m_iMaxLength(maxLength), m_pCamera(nullptr)
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

FontEngine::~FontEngine()
{
	if(m_pVertexBuffer != nullptr)
	{
		m_pVertexBuffer->Release();
	}

	if(m_pIndexBuffer != nullptr)
	{
		m_pIndexBuffer->Release();
	}
}

void FontEngine::GetStringRect(const char* str, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, Math::FRECT& out)
{
	// todo: fix this algorithm
	if(str == nullptr)
		return;

	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int uiCurrentMathWidth = 0;

	char c;
	while(c = *str++)
	{
		if(c == '\n')
		{
			if(i > uiCurrentMathWidth)
			{
				uiCurrentMathWidth = i;
			}

			i = 0;
			++j;
		}
		else
		{
			++i;
		}
	}

	if(i > uiCurrentMathWidth)
	{
		uiCurrentMathWidth = i;
	}

	::D3DXVECTOR2 newScale = 4.0f*scale;

	out.topLeft = pos + D3DXVECTOR2(-0.5f*newScale.x,0.5f*newScale.y);
	out.bottomRight = pos + GetTextOffset(newScale,uiCurrentMathWidth,j);
	out.bottomRight.y -= 1.0f;
}

// the amount of space between characters
D3DXVECTOR2 FontEngine::GetTextOffset(const D3DXVECTOR2& scale, unsigned int i, unsigned int j)
{
	return D3DXVECTOR2(scale.x * i/2.0f,-scale.y * j);
	//return D3DXVECTOR2(scale.x * i/ 1.5f,-scale.y * j);
}

void FontEngine::DrawString(const char* str, const char* font, const D3DXVECTOR2& pos, const D3DXVECTOR2& scale, const D3DXVECTOR4& color)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets.push_back(DrawTextInfo(str,font,pos,4*scale,color));
}

void FontEngine::FillVertexBuffer()
{
	VertexPT* v = nullptr;
	m_pVertexBuffer->Lock(0,0,(void**)&v,D3DLOCK_DISCARD);

	unsigned int iCurrentVerts = 0;

	// Loop over all of the subsets
	for(auto iter = m_textSubsets.begin(); iter != m_textSubsets.end(); ++iter)
	{
		const char* str = iter->text.c_str();

		int i = 0; // x pos
		int j = 0; // y pos
		unsigned int iVert = iCurrentVerts; // current vertex

		// Loop over entire string or until we have filled the buffer
		while((iVert < m_iMaxLength) && *str)
		{
			char c = *str++;

			if((c != '\n') && (c != '\t') && (c != ' '))
			{
				int x = c % 16; // Column
				int y = c / 16; // Row

				// tex coords
				D3DXVECTOR2 topLeft(x / 16.0f,y / 16.0f);
				D3DXVECTOR2 bottomRight((x+1) / 16.0f,(y+1) / 16.0f);

				// World Position
				D3DXVECTOR2 textOffset = GetTextOffset(iter->scale,i,j);
				D3DXVECTOR3 posW(iter->pos.x + textOffset.x,iter->pos.y + textOffset.y,0.0f);

				// todo: remove this, or add this functionality elsewhere
				if(posW.x > 40.0f)
				{
					i = -1;
					++j;
				}

				// cull characters that are off the screen
				if(posW.x >= -50.0f && posW.x <= 50.0f && posW.y <= 50.0f && posW.y >= -50.0f)
				{
					int index = iVert * 4;

					v[index].pos = D3DXVECTOR3(-0.5f * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index].tex = topLeft;

					v[index + 1].pos = D3DXVECTOR3(-0.5f * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 1].tex = D3DXVECTOR2(topLeft.x,bottomRight.y);

					v[index + 2].pos = D3DXVECTOR3(0.5f * iter->scale.x,0.5f * iter->scale.y,0.0f) + posW;
					v[index + 2].tex = D3DXVECTOR2(bottomRight.x,topLeft.y);

					v[index + 3].pos = D3DXVECTOR3(0.5f * iter->scale.x,-0.5f * iter->scale.y,0.0f) + posW;
					v[index + 3].tex = bottomRight;

					++iVert;
				}
			}
			else if(c == '\n')
			{
				i = -1;
				++j;
			}
			else if(c == '\t')
			{
				i += 10;
			}

			++i;
		}

		iter->length = iVert - iCurrentVerts;
		iCurrentVerts += iter->length;
	}

	m_pVertexBuffer->Unlock();

}

void FontEngine::Render()
{
	if(m_textSubsets.empty())
		return;

	FillVertexBuffer();

	m_pDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VertexPT));
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->SetVertexDeclaration(VertexPT::m_pVertexDecl);

	ResourceManager::Shader shader;
	m_pRM->GetResource("2dshader",shader);

	shader.pEffect->SetTechnique(shader.tech["TextTech"]);
	shader.pEffect->SetMatrix(shader.parameters["gWorldViewProj"],&(m_pCamera->viewProj()));

	UINT p = 0;
	shader.pEffect->Begin(&p,0);

	for(UINT i = 0; i < p; ++i)
	{
		shader.pEffect->BeginPass(i);

		unsigned int uiStartingIndex = 0;

		// Render all of the subsets
		for(unsigned int j = 0; j < m_textSubsets.size(); ++j)
		{
			ResourceManager::Texture tex;
			m_pRM->GetResource(m_textSubsets[j].font,tex);

			shader.pEffect->SetVector(shader.parameters["gColor"],&m_textSubsets[j].color);
			shader.pEffect->SetTexture(shader.parameters["gTex"],tex.pTexture);
			shader.pEffect->CommitChanges();

			assert(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4*m_textSubsets[j].length,6*uiStartingIndex,2*m_textSubsets[j].length) == S_OK);

			uiStartingIndex += m_textSubsets[j].length;
		}

		shader.pEffect->EndPass();
	}

	shader.pEffect->End();

	m_textSubsets.clear();
}

void FontEngine::OnLostDevice()
{
	if(m_pVertexBuffer != nullptr)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}
}

void FontEngine::OnResetDevice()
{
	if(m_pVertexBuffer == nullptr)
	{
		CreateVertexBuffer();
	}
}

void FontEngine::CreateVertexBuffer()
{
	if(m_pVertexBuffer == nullptr)
	{
		assert(m_pDevice->CreateVertexBuffer(m_iMaxLength*4 * sizeof(VertexPT),D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_pVertexBuffer,0) == S_OK);
	}
}

void FontEngine::CreateIndexBuffer()
{
	if(m_pIndexBuffer == nullptr)
	{
		assert(m_pDevice->CreateIndexBuffer(6 * m_iMaxLength * sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pIndexBuffer,0) == S_OK);

		WORD* index = nullptr;
		m_pIndexBuffer->Lock(0,0,(void**)&index,0);

		for(UINT i = 0; i < m_iMaxLength; ++i)
		{ 
			// Tri 1
			index[i*6] = 4*i + 2;
			index[i*6 + 1] =  4*i + 1;
			index[i*6 + 2] = 4*i;

			//Tri 2
			//
			index[i*6 + 3] = 4*i + 2;
			index[i*6 + 4] = 4*i + 3;
			index[i*6 + 5] = 4*i + 1;
		}
		m_pIndexBuffer->Unlock();
	}
}