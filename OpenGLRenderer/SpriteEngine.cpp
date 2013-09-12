#define GLM_SWIZZLE
#include "SpriteEngine.h"
#include "GenerateBuffers.h"

#include <cassert>
#include <stddef.h>
#include <algorithm>

SpriteEngine::SpriteEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam) :
	m_pRM(pRm), m_pCamera(pCam), m_uiVertexBuffer(0), m_uiIndexBuffer(0), m_iCurrentLength(0), m_iMaxLength(maxLength)
{
	OnReset();
}

SpriteEngine::~SpriteEngine()
{
}
void SpriteEngine::CreateIndexBuffer()
{
	m_uiIndexBuffer = CreateQuadIndexBuffer(m_iMaxLength);
}

void SpriteEngine::CreateVertexBuffer()
{
	glGenBuffers(1,&m_uiVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_uiVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(SpriteVertex) * m_iMaxLength,0,GL_DYNAMIC_DRAW);
}

void SpriteEngine::DrawSprite(const std::string& tech,
							  const std::string& texture,
							  const glm::mat4& transformation,
							  const glm::vec3& color,
							  const glm::vec2& tiling,
							  unsigned int iCellId)
{
	if(m_iCurrentLength < m_iMaxLength)
	{
		IResource* pShader = m_pRM->GetResource(tech);
		IResource* pTex = m_pRM->GetResource(texture);

		if( pShader != nullptr && pTex != nullptr)
		{
			Layer& layer = m_spriteLayers[(int)transformation[3].z];
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
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);
	SpriteVertex* pVert = (SpriteVertex*)glMapBufferRange(GL_ARRAY_BUFFER , 0, m_iMaxLength * sizeof(SpriteVertex), GL_MAP_WRITE_BIT);

	// Loop over all of the layers
	for(auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		// Loop over all sprites
		for(auto iter = layerIter->second.sprites.begin(); iter != layerIter->second.sprites.end(); ++iter)
		{
			for(auto subIter = iter->second.begin(); subIter != iter->second.end(); ++subIter)
			{
				TextureInfo texInfo;
				m_pRM->GetTextureInfo(subIter->first,texInfo); // todo: this function is not yet implemented

				// get the list of all sprites that use the same tech and texture
				std::vector<Sprite>& sprites = subIter->second;

				for(unsigned int i = 0; i < sprites.size(); ++i)
				{
					int x = (sprites[i].iCellId % texInfo.uiCellsWidth);
					int y = (sprites[i].iCellId / (float)texInfo.uiCellsWidth);

					// tex coords
					glm::vec2 topLeft((float)x / (float)texInfo.uiCellsWidth,(float)y / (float)texInfo.uiCellsHeight);
					glm::vec2 bottomRight((float)(x+1) / (float)texInfo.uiCellsWidth,(float)(y+1) / (float)texInfo.uiCellsHeight);

					// filling in the vertices
					pVert[0].pos = (sprites[i].T * glm::vec4(-0.5f,0.5f,0.0f,1.0f)).xyz();
					pVert[0].tex = topLeft;
					pVert[0].color = sprites[i].color;
					pVert[0].tiling = sprites[i].tiling;
					//pVert[0].dy = sprites[i].dy;


					pVert[1].pos = (sprites[i].T * glm::vec4(-0.5f,-0.5f,0.0,1.0f)).xyz();
					pVert[1].tex = glm::vec2(topLeft.x,bottomRight.y);
					pVert[1].color = sprites[i].color;
					pVert[1].tiling = sprites[i].tiling;
					//pVert[1].dx = sprites[i].dx;
					//pVert[1].dy = sprites[i].dx;

					pVert[2].pos = (sprites[i].T * glm::vec4(0.5f,0.5f,0.0,1.0f)).xyz();
					pVert[2].tex = glm::vec2(bottomRight.x,topLeft.y);
					pVert[2].color = sprites[i].color;
					pVert[2].tiling = sprites[i].tiling;
					//pVert[2].dx = sprites[i].dx;
					//pVert[2].dy = sprites[i].dx;

					pVert[3].pos = (sprites[i].T * glm::vec4(0.5f,-0.5f,0.0,1.0f)).xyz();
					pVert[3].tex = bottomRight;
					pVert[3].color = sprites[i].color;
					pVert[3].tiling = sprites[i].tiling;
					// pVert[3].dx = sprites[i].dx;
					// pVert[3].dy = sprites[i].dy;

					pVert += 4;
				}
			}
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

}

void SpriteEngine::OnReset()
{
	CreateIndexBuffer();
	CreateVertexBuffer();
}

void SpriteEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_spriteLayers.empty())
		return;

	// First we must fill the dynamic vertex buffer with all of the sprites sorted by their tech and texture
	FillVertexBuffer();

	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_uiIndexBuffer);

	unsigned long uiStartingIndex = 0; // Starting index to the vertex buffer for rendering multiple textures within one shader pass

	// Enable blending
	glEnable(GL_BLEND);
	//glBlendFunc (GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	// Loop over all the layers
	for(auto layerIter = m_spriteLayers.begin(); layerIter != m_spriteLayers.end(); ++layerIter)
	{
		// Loop over all sprites with the same tech
		for(auto techIter = layerIter->second.sprites.begin(); techIter != layerIter->second.sprites.end(); ++techIter)
		{
			// Apply the shader tech

			const TexturedShader* pShader = static_cast<const TexturedShader*>(m_pRM->GetResource(techIter->first));

			GLuint vertexPosition_modelspaceID = glGetAttribLocation(pShader->GetID(), "vertexPosition_modelspace");
			GLuint vertexUV = glGetAttribLocation(pShader->GetID(), "vertexUV");
			GLuint vertexColor = glGetAttribLocation(pShader->GetID(), "vertexColor");
			GLuint vertexTiling = glGetAttribLocation(pShader->GetID(), "vertexTiling");

			glUseProgram(pShader->GetID());

			glVertexAttribPointer(
						vertexPosition_modelspaceID, // attribute 0. No particular reason for 0, but must match the layout in the shader.
						3,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						sizeof(SpriteVertex),  // stride
						(void*)0            // array buffer offset
						);

			glVertexAttribPointer(
						vertexUV,					// attribute 1
						2,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						sizeof(SpriteVertex),  // stride
						(void*)sizeof(glm::vec3) // array buffer offset
						);

			glVertexAttribPointer(
						vertexColor,					// attribute 2
						3,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						sizeof(SpriteVertex),  // stride
						(void*)offsetof(struct SpriteVertex, color) // array buffer offset
						);

			glVertexAttribPointer(
						vertexTiling,					// attribute 3
						2,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						sizeof(SpriteVertex),  // stride
						(void*)offsetof(struct SpriteVertex, tiling) // array buffer offset
						);

			// set shader parameters
			glUniformMatrix4fv(pShader->GetMVP(),1,false,&m_pCamera->viewProj()[0][0]);

			glActiveTexture(GL_TEXTURE0);

			// Render all sprites that use the same tech and texture
			for(auto spriteIter = techIter->second.begin(); spriteIter != techIter->second.end(); ++spriteIter)
			{
				const Texture* pTexture = static_cast<const Texture*>(m_pRM->GetResource(spriteIter->first));

				glBindTexture(GL_TEXTURE_2D, pTexture->GetID());
				glUniform1i(pShader->GetTextureSamplerID(),0);

				glDrawElements(
							GL_TRIANGLES,      // mode
							spriteIter->second.size() * 6,    // count 4
							GL_UNSIGNED_SHORT,   // type
							(GLvoid*)(uiStartingIndex * 12)  // element array buffer offset 6
							);
				// Increment the index to the dynamic buffer for rendering a new batch of sprites
				uiStartingIndex += spriteIter->second.size();
			}
		}
	}

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	m_spriteLayers.clear();
	m_iCurrentLength = 0;
}




