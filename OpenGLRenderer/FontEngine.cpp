#include "FontEngine.h"
#include "GenerateBuffers.h"
#include "VertexStructures.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

FontEngine::FontEngine(ResourceManager* pRm, VertexBuffer* pVertexStructure, Camera* pCam) :
	m_pRm(pRm), m_pVertexBuffer(pVertexStructure), m_pCamera(pCam)
{
}

void FontEngine::GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	if(str == nullptr)
		return;

	const Charset* font = static_cast<const Charset*>(m_pRm->GetResource("font"));
	GetStringRec(font,str,scale,out);
}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec3& pos, float scale, const glm::vec4& color, FontAlignment options)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets[font].push_back(DrawTextInfo(str,pos,scale,color,options));
}

void FontEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void FontEngine::GetStringRec(const Charset* font,const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	// todo: need to reimplement 
	/*glm::vec2 topLeft(0.0f,0.0f);
	glm::vec2 bottomRight(topLeft);
	bottomRight.y -= 2.0f*scale.y;

	glm::vec2 pos(topLeft.x,bottomRight.y);

	while(*str)
	{
		float fAdvance = 0.2f;

		// todo: need to newline calculations
		if(*str != ' ' && *str != '\n')
		{
			unsigned int index = *str;
			fAdvance = scale.x * font->GetCharDescriptor()[index].Width / (float)(font->GetWidth());
			pos.x += fAdvance * scale.x + 0.5f;
		}
		else if(*str == '\n')
		{
			pos.x = 0.0f;
			pos.y -= 1.2f*scale.y;
		}

		bottomRight.x = glm::max(bottomRight.x,pos.x);
		bottomRight.y = glm::min(bottomRight.y,pos.y);

		str++;
	}

	out = Math::FRECT(topLeft,bottomRight);*/

}

void FontEngine::FillVertexBuffer(std::vector<unsigned int>& output)
{
	output.resize(m_textSubsets.size());

	m_pVertexBuffer->Bind();

	// get the vertex buffer memory to write to
	VertexPCT* v = static_cast<VertexPCT*>(glMapBufferRange(GL_ARRAY_BUFFER , 0, m_pVertexBuffer->GetLength(), GL_MAP_WRITE_BIT));

	unsigned int iCurrentVert = 0;
	unsigned int iSubsetCounter = 0;

	// Loop over all texture subsets and write them to the vertex buffer
	for(auto& textureIter : m_textSubsets)
	{
		// The length in renderable characters
		unsigned int iSubsetLength = 0;

		// Loop over all fonts with the current texture
		for(auto& iter : textureIter.second)
		{
			// Get the current font
			const Charset* font = static_cast<const Charset*>(m_pRm->GetResource(textureIter.first));
			const char* str = iter.text.c_str();

			// World pos of aligned text to be rendered
			glm::vec3 posW(iter.pos);

			// If the text needs to be aligned to center or right
			if(iter.options != FontAlignment::Left)
			{
				// todo: reimplement
				/*Math::FRECT drawRec;
				GetStringRec(font,str,iter.scale,drawRec);

				float fHalfWidth = (drawRec.Width() / 2.0f);

				posW.x -= fHalfWidth;

				if (iter.options == FontAlignment::Right)
				{
					posW.x -= fHalfWidth;
				}*/
			}
			else
			{
				// left, do nothing
			}

			// write the entire string to the vertex buffer
			while((iCurrentVert < m_pVertexBuffer->GetLength()) && *str)
			{
				if(*str != ' ' && *str != '\n' && *str != '\t')
				{
					// font info about the character to draw
					const CharDescriptor& charInfo = font->GetCharDescriptor()[(unsigned int)(*str)]; 

					// calculate texture coordinates
					glm::vec2 texTopLeft(charInfo.x / (float)(font->GetWidth()),charInfo.y / (float)(font->GetHeight()));
					glm::vec2 texBottomRight(((charInfo.x+charInfo.Width) / (float)(font->GetWidth())),(charInfo.y+charInfo.Height) / (float)(font->GetHeight()));

					// calculate position
					glm::vec3 posTopLeft(posW.x + charInfo.XOffset * iter.scale,posW.y - charInfo.YOffset * iter.scale,posW.z);
					glm::vec3 posBottomRight(posTopLeft.x + charInfo.Width * iter.scale,posTopLeft.y - charInfo.Height * iter.scale,posW.z);

					int index = iCurrentVert * 4;

					v[index].pos = posTopLeft;
					v[index].color = iter.color;
					v[index].tex = texTopLeft;

					v[index + 1].pos = glm::vec3(posTopLeft.x,posBottomRight.y,posW.z);
					v[index + 1].color = iter.color;
					v[index + 1].tex = glm::vec2(texTopLeft.x,texBottomRight.y);

					v[index + 2].pos = glm::vec3(posBottomRight.x,posTopLeft.y,posW.z);
					v[index + 2].color = iter.color;
					v[index + 2].tex = glm::vec2(texBottomRight.x,texTopLeft.y);	

					v[index + 3].pos = posBottomRight;
					v[index + 3].color = iter.color;
					v[index + 3].tex = texBottomRight;

					posW.x += charInfo.XAdvance * iter.scale;

					++iCurrentVert;
					++iSubsetLength;
				}
				else
				{
					if(*str == '\n')
					{
						posW.x = iter.pos.x;
						posW.y -= iter.scale * font->GetLineHeight();
					}
					else
					{
						float advance = 20 * iter.scale;
						int spaceCount = 1;

						if(*str == '\t')
						{
							spaceCount = 4;
						}

						posW.x += advance * spaceCount;
					}
				}

				str++;
			}
		}

		output[iSubsetCounter++] = iSubsetLength;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_textSubsets.empty())
		return;

	std::vector<unsigned int> subsetLength;
	FillVertexBuffer(subsetLength);

	m_pVertexBuffer->BindVAO();

	// Enables one texture
	glActiveTexture(GL_TEXTURE0);

	// get the shader to use
	const TexturedShader* pShader = static_cast<TexturedShader*>(m_pRm->GetResource("textShader"));

	// use the shader
	glUseProgram(pShader->GetID());

	// set the transformation matrix
	glUniformMatrix4fv(pShader->GetMVP(),1,false,&(m_pCamera->viewProj()[0][0]));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int uiStartingIndex = 0;
	unsigned int uiSubset = 0;

	// loop over all text subsets
	for(auto& iter : m_textSubsets)
	{
		const IResource* pCurrentTexture = m_pRm->GetResource(iter.first);

		// Set the texture to use
		glBindTexture(GL_TEXTURE_2D, pCurrentTexture->GetID());
		glUniform1i(pShader->GetTextureSamplerID(),0);

		glDrawElements(GL_TRIANGLES, subsetLength[uiSubset] * 6, GL_UNSIGNED_SHORT, reinterpret_cast<void*>(uiStartingIndex * 12));

		uiStartingIndex += subsetLength[uiSubset++];
	}

	glDisable(GL_BLEND);

	m_textSubsets.clear();
}

