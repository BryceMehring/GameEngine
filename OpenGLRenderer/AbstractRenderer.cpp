#include "AbstractRenderer.h"
#include "SpriteRenderer.h"
#include "FontEngine.h"
#include "VertexStructures.h"
#include "ApplyShader.h"

#include <cassert>
#include <algorithm>

AbstractRenderer::AbstractRenderer(ResourceManager *pRm, VertexBuffer *pVertexBuffer, Camera *pCam) :
	m_pRM(pRm), m_pVertexBuffer(pVertexBuffer), m_pCamera(pCam)
{
}

void AbstractRenderer::DrawSprite(const std::string& tech,
							  const std::string& texture,
							  const glm::mat4& transformation,
							  const glm::vec4& color,
							  const glm::vec2& tiling,
							  unsigned int iCellId
							  )
{
	int iZorder = { (int)floor(transformation[3].z) };
	m_spriteLayers[iZorder][tech][texture].emplace_back(new SpriteRenderable{transformation, color, tiling, iCellId});
}

void AbstractRenderer::DrawString(const char* str,
							  const char* font,
							  const glm::vec3& pos,
							  float scale,
							  const glm::vec4& color,
							  FontAlignment alignment
							  )
{
	if (font == nullptr)
	{
		font = "font";
	}

	int iZorder = pos.z;
	m_spriteLayers[iZorder]["textShader"][font].emplace_back(new FontRenderable{ str, pos, scale, color, alignment });
}

void AbstractRenderer::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void AbstractRenderer::Render()
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

			//currentShader->set
			currentShader->SetMVP(m_pCamera->ViewProj());

			// Loop over all sprites with the same texture
			for(auto& texIter : techIter.second)
			{
				const IResource* pResource = m_pRM->GetResource(texIter.first);
				assert(pResource != nullptr);

				//spriteIter->Setup(currentShader, pResource);

				//currentShader->BindTexture(*pTexture);
				//currentShader->SetValue("animationTiles",glm::vec2(pTexture->GetCellsWidth(), pTexture->GetCellsHeight()));

				//spriteIter->Setup(currentShader, *pResource);

				// Render all sprites that use the same tech and texture
				for (auto& spriteIter : texIter.second)
				{
					//spriteIter->Setup(currentShader, )
					spriteIter->Render(currentShader, *pResource);
				}
			}
		}
	}

	glDisable(GL_BLEND);

	m_spriteLayers.clear();
}




