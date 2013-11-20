#include "FontEngine.h"
#include "GenerateBuffers.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

FontEngine::FontEngine(ResourceManager* pRm, IndexedVertexBuffer* pVertexStructure, Camera* pCam) :
	m_pRm(pRm), m_pVertexBuffer(pVertexStructure), m_pCamera(pCam) //m_iMaxLength(2*maxLength)
{
}

void FontEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void FontEngine::GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	if(str == nullptr)
		return;

	const Charset* font = static_cast<const Charset*>(m_pRm->GetResource("font"));
	GetStringRec(font,str,scale,out);
}

void FontEngine::GetStringRec(const Charset* font,const char* str, const glm::vec2& scale, Math::FRECT& out) const
{
	glm::vec2 topLeft(0.0f,0.0f);
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

	out = Math::FRECT(topLeft,bottomRight);

}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& color, FontAlignment options)
{
	if(str == nullptr)
		return;

	if(font == nullptr)
		font = "font";

	m_textSubsets[font].push_back(DrawTextInfo(str,pos,scale,color,options));
}


void FontEngine::FillVertexBuffer(std::vector<unsigned int>& output)
{
	output.resize(m_textSubsets.size());

	// bind the vertex buffer that we are going to write to
	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());

	// get the vertex buffer memory to write to
	FontVertex* v = static_cast<FontVertex*>(glMapBufferRange(GL_ARRAY_BUFFER , 0, m_pVertexBuffer->GetSize(), GL_MAP_WRITE_BIT));

	unsigned int iCurrentVert = 0;
	unsigned int iSubsetIndex = 0;

	// Loop over all texture subsets and write them to the vertex buffer
	for(auto& iter : m_textSubsets)
	{
		unsigned int iSubsetLength = 0;

		// Loop over all fonts with the current texture
		for(auto& subIter : iter.second)
		{
			// Get the current font
			const Charset* font = static_cast<const Charset*>(m_pRm->GetResource(iter.first));
			const char* str = subIter.text.c_str();

			// If the text needs to be aligned to center or right
			if(subIter.options != FontAlignment::Left)
			{
				Math::FRECT drawRec;
				GetStringRec(font,str,subIter.scale,drawRec);

				float fHalfWidth = (drawRec.Width() / 2.0f);

				subIter.pos.x -= fHalfWidth;

				if (subIter.options == FontAlignment::Right)
				{
					subIter.pos.x -= fHalfWidth;
				}
			}
			else
			{
				// left, do nothing
			}

			// World pos of aligned text to be rendered
			glm::vec3 posW(subIter.pos); // World pos of where the text will be drawn

			// Loop over the entire string and write to the vertex buffer
			while((iCurrentVert < m_pVertexBuffer->GetLength()) && *str)
			{
				char character = *str++; // character to draw

				if(character != ' ' && character != '\n' && character != '\t')
				{
					int index = iCurrentVert * 4;

					const CharDescriptor& charInfo = font->GetCharDescriptor()[(unsigned int)(character)]; // font info about the character to draw

					float fAdvance = (subIter.scale.x * charInfo.Width / (float)font->GetWidth()); // How much we will advance from the current character

					// calc tex coords
					glm::vec2 topLeft((charInfo.x / (float)(font->GetWidth())),charInfo.y / (float)(font->GetHeight()));
					glm::vec2 bottomRight(((charInfo.x+charInfo.Width) / (float)(font->GetWidth())),(charInfo.y+charInfo.Height) / (float)(font->GetHeight()));

					v[index].pos = glm::vec3(posW.x,0.5f * subIter.scale.y + posW.y,posW.z);
					v[index].uv = topLeft;
					v[index].color = subIter.color;

					v[index + 1].pos = glm::vec3(posW.x,-0.5f * subIter.scale.y + posW.y,posW.z);
					v[index + 1].uv = glm::vec2(topLeft.x,bottomRight.y);
					v[index + 1].color = subIter.color;

					v[index + 2].pos = glm::vec3(fAdvance * subIter.scale.x,0.5f * subIter.scale.y,0.0f) + posW;
					v[index + 2].uv = glm::vec2(bottomRight.x,topLeft.y);
					v[index + 2].color = subIter.color;

					v[index + 3].pos = glm::vec3(fAdvance * subIter.scale.x,-0.5f * subIter.scale.y,0.0f) + posW;
					v[index + 3].uv = bottomRight;
					v[index + 3].color = subIter.color;

					posW.x += fAdvance * subIter.scale.x;

					++iCurrentVert;
					++iSubsetLength;
				}
				else
				{
					float fAdvance = 0.2f; // How much we will advance from the current character

					if(character == '\n')
					{
						posW.x = subIter.pos.x;
						posW.y -= 1.2f*subIter.scale.y;
					}
					else
					{
						if(character == '\t')
						{
							fAdvance *= 4.0f;
						}

						posW.x += fAdvance * subIter.scale.x + 0.5f;
					}
				}
			}
		}

		output[iSubsetIndex++] = iSubsetLength;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_textSubsets.empty())
		return;

	std::vector<unsigned int> subsetLength;

	// first we fill the vertex buffer with the text to render
	FillVertexBuffer(subsetLength);

	// get the shader to use
	const TexturedShader* pShader = static_cast<TexturedShader*>(m_pRm->GetResource("textShader"));

	// use the shader
	glUseProgram(pShader->GetID());

	// Create our vertex structure in OpenGL
	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
				pShader->GetAtribs().find("vertexPosition_modelspace")->second,
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(FontVertex),  // stride
				0            // array buffer offset
				);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
				pShader->GetAtribs().find("vertexUV")->second,
				2,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(FontVertex),  // stride
				reinterpret_cast<void*>(sizeof(glm::vec3)) // array buffer offset
				);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
				pShader->GetAtribs().find("vertexColor")->second,	
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				sizeof(FontVertex),  // stride
				reinterpret_cast<void*>(offsetof(struct FontVertex,color))  // array buffer offset
				);

	// Bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_pVertexBuffer->GetIndexBuffer());

	// Enables one texture
	glActiveTexture(GL_TEXTURE0);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int uiStartingIndex = 0;
	unsigned int uiSubset = 0;

	// set the transformation matrix
	glUniformMatrix4fv(pShader->GetMVP(),1,false,&(m_pCamera->viewProj()[0][0]));

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

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	m_textSubsets.clear();
}

