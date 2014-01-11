#define GLM_SWIZZLE
#include "SpriteEngine.h"
#include "VertexStructures.h"

#include <cassert>
#include <algorithm>

SpriteEngine::SpriteEngine(ResourceManager* pRm, VertexBuffer* pVertexBuffer, Camera* pCam) :
m_pRM(pRm), m_pVertexBuffer(pVertexBuffer), m_iCurrentLength(0), m_pCamera(pCam)
{
	assert(pVertexBuffer->GetVertexSize() == sizeof(VertexPT));
}

SpriteEngine::~SpriteEngine()
{
}

void SpriteEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void SpriteEngine::DrawSprite(const std::string& tech,
							  const std::string& texture,
							  const glm::mat4& transformation,
							  const glm::vec4& color,
							  const glm::vec2& tiling,
							  unsigned int iCellId
							  )
{
	if(m_iCurrentLength < m_pVertexBuffer->GetLength())
	{
		int iZorder = { (int)floor(transformation[3].z) };
		m_spriteLayers[iZorder][tech][texture].push_back({ transformation, color, tiling, iCellId });
		m_iCurrentLength += 4;
	}
}

void SpriteEngine::FillVertexBuffer()
{
	m_pVertexBuffer->BindVBO();

	VertexPT* pVert = static_cast<VertexPT*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, m_pVertexBuffer->GetSize(), GL_MAP_WRITE_BIT));

	// Loop over all of the layers
	for (auto& layerIter : m_spriteLayers)
	{
		// Loop over all sprites
		for (auto& techIter : layerIter.second)
		{
			for (auto& textureIter : techIter.second)
			{
				const Texture* pTex = static_cast<Texture*>(m_pRM->GetResource(textureIter.first, ResourceType::Texture));

				// write all sprites that use the same tech and texture to the buffer
				for (auto& iter : textureIter.second)
				{
					unsigned int x = iter.iCellId % pTex->GetCellsWidth();
					unsigned int y = iter.iCellId / pTex->GetCellsWidth();

					// tex coords
					glm::vec2 topLeft(x / (float)(pTex->GetCellsWidth()), y / (float)(pTex->GetCellsHeight()));
					glm::vec2 bottomRight((x + 1) / (float)(pTex->GetCellsWidth()), (y + 1) / (float)(pTex->GetCellsHeight()));

					// filling in the vertices
					pVert[0].pos = (iter.T * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)).xyz();
					pVert[0].tex = topLeft * iter.tiling;
					
					pVert[1].pos = (iter.T * glm::vec4(-0.5f, -0.5f, 0.0, 1.0f)).xyz();
					pVert[1].tex = glm::vec2(topLeft.x, bottomRight.y) * iter.tiling;
					
					pVert[2].pos = (iter.T * glm::vec4(0.5f, 0.5f, 0.0, 1.0f)).xyz();
					pVert[2].tex = glm::vec2(bottomRight.x, topLeft.y) * iter.tiling;

					pVert[3].pos = (iter.T * glm::vec4(0.5f, -0.5f, 0.0, 1.0f)).xyz();
					pVert[3].tex = bottomRight * iter.tiling;

					pVert += 4;
				}
			}
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

}

void SpriteEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_spriteLayers.empty())
		return;

	// First we must fill the dynamic vertex buffer with all of the sprites sorted by their tech and texture
	FillVertexBuffer();

	m_pVertexBuffer->BindVAO();

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// Starting index to the vertex buffer for rendering multiple textures within one shader pass
	unsigned int uiStartingIndex = 0;

	// Loop over all of the layers
	for(auto& layerIter : m_spriteLayers)
	{
		// Loop over all sprites with the same tech
		for(auto& techIter : layerIter.second)
		{
			// Apply the shader tech
			const TexturedShader* pShader = static_cast<const TexturedShader*>(m_pRM->GetResource(techIter.first, ResourceType::TexturedShader));
			assert(pShader != nullptr);

			pShader->UseShader();
			pShader->SetMVP(m_pCamera->ViewProj());

			// Loop over all sprites with the same texture
			for(auto& texIter : techIter.second)
			{
				const IResource* pTexture = m_pRM->GetResource(texIter.first, ResourceType::Texture);
				assert(pTexture != nullptr);

				pShader->BindTexture(*pTexture);

				// Render all sprites that use the same tech and texture
				for (auto& spriteIter : texIter.second)
				{
					pShader->SetColor(spriteIter.color);

					glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0, uiStartingIndex * 4);

					++uiStartingIndex;
				}
			}
		}
	}

	glDisable(GL_BLEND);

	m_spriteLayers.clear();
	m_iCurrentLength = 0;
}




