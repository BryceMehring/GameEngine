#define GLM_SWIZZLE
#include "SpriteEngine.h"
#include "GenerateBuffers.h"

#include <cassert>
#include <stddef.h>
#include <algorithm>

SpriteEngine::SpriteEngine(ResourceManager* pRm, IndexedVertexBuffer* pVertexStruct, Camera* pCam) :
	m_pRM(pRm), m_pVertexBuffer(pVertexStruct), m_pCamera(pCam), m_iCurrentLength(0)
{
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
							  const glm::vec3& color,
							  const glm::vec2& tiling,
							  unsigned int iCellId
							  )
{
	if(m_iCurrentLength < m_pVertexBuffer->GetLength())
	{
		IResource* pShader = m_pRM->GetResource(tech);
		IResource* pTex = m_pRM->GetResource(texture);

		if( pShader != nullptr && pTex != nullptr)
		{
			int iZorder = (int)(floor(transformation[3].z));
			Layer& layer = m_spriteLayers[iZorder];
			layer.sprites[tech][texture].push_back(Sprite(transformation,color,tiling,iCellId));
			++m_iCurrentLength;
		}
	}
	else
	{
		// todo: create some error msg
	}
}

void SpriteEngine::FillVertexBuffer()
{
	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());
	SpriteVertex* pVert = static_cast<SpriteVertex*>(glMapBufferRange(GL_ARRAY_BUFFER , 0, m_pVertexBuffer->GetSize(), GL_MAP_WRITE_BIT));

	// Loop over all of the layers
	for(auto& layerIter : m_spriteLayers)
	{
		// Loop over all sprites
		for(auto& iter : layerIter.second.sprites)
		{
			for(auto& subIter : iter.second)
			{
				TextureInfo texInfo;
				m_pRM->GetTextureInfo(subIter.first,texInfo);

				// get the list of all sprites that use the same tech and texture
				std::vector<Sprite>& sprites = subIter.second;

				for(unsigned int i = 0; i < sprites.size(); ++i)
				{
					int x = (sprites[i].iCellId % texInfo.uiCellsWidth);
					int y = (int)(sprites[i].iCellId / (float)texInfo.uiCellsWidth);

					// tex coords
					glm::vec2 topLeft(x / (float)(texInfo.uiCellsWidth),y / (float)(texInfo.uiCellsHeight));
					glm::vec2 bottomRight((x+1) / (float)(texInfo.uiCellsWidth),(y+1) / (float)(texInfo.uiCellsHeight));

					// filling in the vertices
					pVert[0].pos = (sprites[i].T * glm::vec4(-0.5f,0.5f,0.0f,1.0f)).xyz();
					pVert[0].tex = topLeft * sprites[i].tiling;
					pVert[0].color = sprites[i].color;

					pVert[1].pos = (sprites[i].T * glm::vec4(-0.5f,-0.5f,0.0,1.0f)).xyz();
					pVert[1].tex = glm::vec2(topLeft.x,bottomRight.y) * sprites[i].tiling;
					pVert[1].color = sprites[i].color;

					pVert[2].pos = (sprites[i].T * glm::vec4(0.5f,0.5f,0.0,1.0f)).xyz();
					pVert[2].tex = glm::vec2(bottomRight.x,topLeft.y) * sprites[i].tiling;
					pVert[2].color = sprites[i].color;

					pVert[3].pos = (sprites[i].T * glm::vec4(0.5f,-0.5f,0.0,1.0f)).xyz();
					pVert[3].tex = bottomRight * sprites[i].tiling;
					pVert[3].color = sprites[i].color;

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

	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_pVertexBuffer->GetIndexBuffer());

	unsigned long uiStartingIndex = 0; // Starting index to the vertex buffer for rendering multiple textures within one shader pass

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

		// Loop over all of the layers
	for(auto& layerIter : m_spriteLayers)
	{
		// Loop over all sprites with the same tech
		for(auto& techIter : layerIter.second.sprites)
		{
			// Apply the shader tech

			const TexturedShader* pShader = static_cast<const TexturedShader*>(m_pRM->GetResource(techIter.first));
			const TexturedShader::UnifromMap& atribMap = pShader->GetAtribs();

			glUseProgram(pShader->GetID());

			glVertexAttribPointer(atribMap.at("vertexPosition_modelspace"),3,GL_FLOAT,GL_FALSE,sizeof(SpriteVertex),0);
			glVertexAttribPointer(atribMap.at("vertexUV"),2,GL_FLOAT,GL_FALSE,sizeof(SpriteVertex),reinterpret_cast<void*>(sizeof(glm::vec3)));
			glVertexAttribPointer(atribMap.at("vertexColor"),3,GL_FLOAT,GL_FALSE,sizeof(SpriteVertex),reinterpret_cast<void*>(offsetof(SpriteVertex, color)));

			glActiveTexture(GL_TEXTURE0);

			// set shader parameters
			glUniformMatrix4fv(pShader->GetMVP(),1,false,&m_pCamera->viewProj()[0][0]);

			// Render all sprites that use the same tech and texture
			for(auto& spriteIter : techIter.second)
			{
				const Texture* pTexture = static_cast<const Texture*>(m_pRM->GetResource(spriteIter.first));

				glBindTexture(GL_TEXTURE_2D, pTexture->GetID());
				glUniform1i(pShader->GetTextureSamplerID(),0);

				glDrawElements(GL_TRIANGLES, spriteIter.second.size() * 6, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(uiStartingIndex * 12));

				// Increment the index to the dynamic buffer for rendering a new batch of sprites
				uiStartingIndex += spriteIter.second.size();
			}
		}
	}

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	m_spriteLayers.clear();
	m_iCurrentLength = 0;
}




