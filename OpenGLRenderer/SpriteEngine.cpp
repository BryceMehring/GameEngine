#include "SpriteEngine.h"
#include "VertexStructures.h"
#include "ApplyShader.h"

#include <cassert>
#include <algorithm>

SpriteEngine::SpriteEngine(ResourceManager* pRm, VertexBuffer* pVertexBuffer, Camera* pCam) :
m_pRM(pRm), m_pVertexBuffer(pVertexBuffer), m_pCamera(pCam)
{
	assert(pVertexBuffer->GetVertexSize() == sizeof(VertexPT));
}



void SpriteEngine::DrawSprite(const std::string& tech,
							  const std::string& texture,
							  const glm::mat4& transformation,
							  const glm::vec4& color,
							  const glm::vec2& tiling,
							  unsigned int iCellId
							  )
{
	int iZorder = { (int)floor(transformation[3].z) };
	m_spriteLayers[iZorder][tech][texture].push_back({ transformation, color, tiling, iCellId });
}

void SpriteEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void SpriteEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_spriteLayers.empty())
		return;

	m_pVertexBuffer->BindVAO();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// Loop over all of the layers
	for(auto& layerIter : m_spriteLayers)
	{
		// Loop over all sprites with the same tech
		for(auto& techIter : layerIter.second)
		{
			// Apply the shader tech
			ApplyTexturedShader currentShader = static_cast<TexturedShader*>(m_pRM->GetResource(techIter.first, ResourceType::TexturedShader));

			currentShader->SetMVP(m_pCamera->ViewProj());

			// Loop over all sprites with the same texture
			for(auto& texIter : techIter.second)
			{
				const Texture* pTexture = static_cast<Texture*>(m_pRM->GetResource(texIter.first, ResourceType::Texture));
				assert(pTexture != nullptr);

				currentShader->BindTexture(*pTexture);
				currentShader->SetValue("animationTiles",glm::vec2(pTexture->GetCellsWidth(), pTexture->GetCellsHeight()));

				// Render all sprites that use the same tech and texture
				for (auto& spriteIter : texIter.second)
				{
					currentShader->SetColor(spriteIter.color);
					currentShader->SetValue("transformation",spriteIter.T);
					currentShader->SetValue("tiling",spriteIter.tiling);
					currentShader->SetValue("tileIndex",spriteIter.iCellId);

					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
				}
			}
		}
	}

	glDisable(GL_BLEND);

	m_spriteLayers.clear();
}




