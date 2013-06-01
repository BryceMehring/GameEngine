#include "SpriteManager.h"
#include "Streams.h"

#include <cassert>

SpriteManager::SpriteManager(IDirect3DDevice9* pDevice, ResourceManager* pTm, int maxLength) : 
m_pDevice(pDevice), m_pRM(pTm), m_pVertexGeoBuffer(nullptr), m_pIndexGeoBuffer(nullptr),
m_iMaxLength(maxLength), m_iCurrentLength(0)
{
	CreateIndexBuffer();
	OnResetDevice();
}

SpriteManager::~SpriteManager()
{
	m_pIndexGeoBuffer->Release();

	OnLostDevice();
}

void SpriteManager::OnLostDevice()
{
	if(m_pVertexGeoBuffer != nullptr)
	{
		m_pVertexGeoBuffer->Release();
		m_pVertexGeoBuffer = nullptr;
	}
}

void SpriteManager::OnResetDevice()
{
	if(m_pVertexGeoBuffer == nullptr)
	{
		assert(m_pDevice->CreateVertexBuffer(sizeof(SpriteVertex)*m_iMaxLength,D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_pVertexGeoBuffer,0) == S_OK);
	}
}

void SpriteManager::CreateIndexBuffer()
{
	assert(m_pDevice->CreateIndexBuffer(6*m_iMaxLength*sizeof(WORD),0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pIndexGeoBuffer,0) == S_OK);

	WORD* index = 0;
	assert(m_pIndexGeoBuffer->Lock(0,0,(void**)&index,0) == S_OK);

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

	assert(m_pIndexGeoBuffer->Unlock() == S_OK);
}

void SpriteManager::DrawSprite(const std::string& tech, const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId, float dx, float dy)
{
	if(m_iCurrentLength < m_iMaxLength)
	{
		m_sprites[tech][texture].push_back(Sprite(transformation,iCellId,dx,dy));
		++m_iCurrentLength;
	}
	else
	{
		// todo: create some error msg
	}
}

void SpriteManager::FillVertexBuffer()
{
	SpriteVertex* pVert = nullptr;
	assert(m_pVertexGeoBuffer->Lock(0,0,(void**)&pVert,D3DLOCK_DISCARD) == S_OK);

	// Loop over all sprites
	for(auto iter = m_sprites.begin(); iter != m_sprites.end(); ++iter)
	{
		for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
		{
			TextureInfo texInfo;
			m_pRM->GetTextureInfo(subIter->first,texInfo);
			
			// get the list of all sprites that use the same tech and texture
			std::vector<Sprite>& sprites = subIter->second;

			for(unsigned int i = 0; i < sprites.size(); ++i)
			{
				int x = (sprites[i].iCellId % texInfo.uiCellsWidth);
				int y = (sprites[i].iCellId / texInfo.uiCellsHeight);

				// tex coords
				D3DXVECTOR2 topLeft((float)x / (float)texInfo.uiCellsWidth,(float)y / (float)texInfo.uiCellsHeight);
				D3DXVECTOR2 bottomRight((float)(x+1) / (float)texInfo.uiCellsWidth,(float)(y+1) / (float)texInfo.uiCellsHeight);

				// filling in the vertices

				D3DXVec3TransformCoord(&pVert[0].pos,&D3DXVECTOR3(-0.5f,0.5f,0.0),&sprites[i].T);
				pVert[0].tex = topLeft;
				pVert[0].dx = sprites[i].dx;
				pVert[0].dy = sprites[i].dy;

				D3DXVec3TransformCoord(&pVert[1].pos,&D3DXVECTOR3(-0.5f,-0.5f,0.0),&sprites[i].T);
				pVert[1].tex = D3DXVECTOR2(topLeft.x,bottomRight.y);
				pVert[1].dx = sprites[i].dx;
				pVert[1].dy = sprites[i].dx;

				D3DXVec3TransformCoord(&pVert[2].pos,&D3DXVECTOR3(0.5f,0.5f,0.0),&sprites[i].T);
				pVert[2].tex = D3DXVECTOR2(bottomRight.x,topLeft.y);
				pVert[2].dx = sprites[i].dx;
				pVert[2].dy = sprites[i].dx;

				D3DXVec3TransformCoord(&pVert[3].pos,&D3DXVECTOR3(0.5f,-0.5f,0.0),&sprites[i].T);
				pVert[3].tex = bottomRight;
				pVert[3].dx = sprites[i].dx;
				pVert[3].dy = sprites[i].dy;

				pVert += 4;
			}
		}
	}
	
	assert(m_pVertexGeoBuffer->Unlock() == S_OK);
}

void SpriteManager::DrawSprite(const D3DXMATRIX& transformation, const std::string& texture, unsigned int iCellId, float dx, float dy, DWORD color)
{
	//DrawSprite("ColorAnimatedSprite",transformation,texture,iCellId,dx,dy);
}

void SpriteManager::Render()
{
	// First we must fill the dynamic vertex buffer with all of the sprites sorted by their tech and texture  
	FillVertexBuffer();

	assert(m_pDevice->SetStreamSource(0, m_pVertexGeoBuffer, 0, sizeof(SpriteVertex)) == S_OK);
	assert(m_pDevice->SetIndices(m_pIndexGeoBuffer) == S_OK);
	assert(m_pDevice->SetVertexDeclaration(SpriteVertex::m_pVertexDecl) == S_OK);

	ResourceManager::Shader shader;
	m_pRM->GetResource("2dshader",shader);

	shader.pEffect->SetMatrix(shader.parameters["gWorldViewProj"],&(m_pCamera->viewProj()));
	
	unsigned int uiStartingIndex = 0; // Starting index to the vertex buffer for rendering multiple textures within one shader pass
	
	// Loop over all sprites with the same tech
	for(auto iter = m_sprites.begin(); iter != m_sprites.end(); ++iter)
	{
		// Apply the shader tech

		shader.pEffect->SetTechnique(shader.tech[iter->first]);

		UINT P = 0; 
		shader.pEffect->Begin(&P,0);
		for(UINT i = 0; i < P; ++i)
		{
			shader.pEffect->BeginPass(i);

			// Render all sprites that use the same tech and texture
			for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
			{
				ResourceManager::Texture tex;
				m_pRM->GetResource(subIter->first,tex);

				shader.pEffect->SetTexture(shader.parameters["gTex"],tex.pTexture);
				shader.pEffect->CommitChanges();

				assert(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,4*subIter->second.size(),6*uiStartingIndex,2*subIter->second.size()) == S_OK);

				// Increment the index to the dynamic buffer for rendering a new batch of sprites
				uiStartingIndex += subIter->second.size();
			}

			shader.pEffect->EndPass();
		}

		shader.pEffect->End();
	}

	m_sprites.clear();
	m_iCurrentLength = 0;
}

				
		
	
