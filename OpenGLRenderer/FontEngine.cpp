#include "FontEngine.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

FontEngine::FontEngine(ResourceManager* pRm, VertexBuffer* pVertexBuffer, Camera* pCam) :
m_pRm(pRm), m_pVertexBuffer(pVertexBuffer), m_pCamera(pCam)
{
	assert(pVertexBuffer->GetVertexSize() == sizeof(VertexPT));
}

void FontEngine::GetStringRect(const char* str, float scale, FontAlignment alignment, Math::FRECT& out) const
{
	if (str != nullptr)
	{
		Font* pFont = static_cast<Font*>(m_pRm->GetResource("font", ResourceType::Font));
		assert(pFont != nullptr);

		NormalizeScaling(pFont, scale);

		GetStringRect(str, pFont, scale, alignment, out);
	}
}

void FontEngine::DrawString(const char* str, const char* font, const glm::vec3& pos, float scale, const glm::vec4& color, FontAlignment alignment)
{
	if (str != nullptr)
	{
		if (font == nullptr)
		{
			font = "font";
		}
		
		Font* pFont = static_cast<Font*>(m_pRm->GetResource(font, ResourceType::Font));
		assert(pFont != nullptr);

		NormalizeScaling(pFont, scale);
		
		m_strings[font].push_back(DrawTextInfo(str, pos, scale, color, alignment));
	}
}

void FontEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void FontEngine::GetStringRect(const char* str, const Font* fnt, float scale, FontAlignment alignment, Math::FRECT& inout) const
{
	unsigned int lineHeight = fnt->GetLineHeight();

	inout.bottomRight = inout.topLeft;
	inout.bottomRight.y -= scale * lineHeight;

	glm::vec3 pos(inout.topLeft.x, inout.bottomRight.y, 0.0f);

	while(*str)
	{
		if (fnt->IsValidCharacter(*str))
		{
			if (!IsSpecialCharacter(*str))
			{
				pos.x += scale * fnt->GetCharDescriptor(*str).XAdvance;
			}
			else
			{
				ProccessSpecialCharacter(*str, scale, lineHeight, glm::vec3(inout.topLeft, 0.0f), pos);

				inout.bottomRight.y = glm::min(inout.bottomRight.y, pos.y);
			}

			inout.bottomRight.x = glm::max(inout.bottomRight.x, pos.x);
		}
		
		str++;
	}

	if (alignment != FontAlignment::Left)
	{
		glm::vec2 offset;
		AlignTextPos(inout.bottomRight.x - inout.topLeft.x, alignment, offset);

		inout.topLeft += offset;
		inout.bottomRight += offset;
	}

}

void FontEngine::AlignTextPos(float width, FontAlignment alignment, glm::vec2& out) const
{
	float fHalfWidth = (width / 2.0f);

	out.x -= fHalfWidth;

	if (alignment == FontAlignment::Right)
	{
		out.x -= fHalfWidth;
	}
}

bool FontEngine::IsSpecialCharacter(char c) const
{
	return ((c == ' ') || (c == '\n') || (c == '\t'));
}

void FontEngine::ProccessSpecialCharacter(char c, float scale, unsigned int lineHeight, const glm::vec3& oldPos, glm::vec3& currentPos) const
{
	if (c == '\n')
	{
		currentPos.x = oldPos.x;
		currentPos.y -= scale * lineHeight;
	}
	else
	{
		float advance = 25 * scale;
		int spaceCount = 1;

		if (c == '\t')
		{
			spaceCount = 4;
		}

		currentPos.x += advance * spaceCount;
	}
}

void FontEngine::FillVertexBuffer()
{
	m_pVertexBuffer->BindVBO();

	// Get the vertex buffer memory to write to
	VertexPT* v = static_cast<VertexPT*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, m_pVertexBuffer->GetSize(), GL_MAP_WRITE_BIT));

	unsigned int iCurrentVert = 0;

	// Loop over all texture subsets and write them to the vertex buffer
	for(auto& textureIter : m_strings)
	{
		// The length in renderable characters
		unsigned int iSubsetLength = 0;

		// Loop over all fonts with the current texture
		for(auto& iter : textureIter.second)
		{
			// Get the current font
			const Font* fnt = static_cast<const Font*>(m_pRm->GetResource(textureIter.first, ResourceType::Font));
			const char* str = iter.text.c_str();

			// World pos of aligned text to be rendered
			glm::vec3 posW(iter.pos);

			// If the text needs to be aligned to center or right
			if(iter.alignment != FontAlignment::Left)
			{
				Math::FRECT drawRect(glm::vec2(posW.x, posW.y));
				GetStringRect(str, fnt, iter.scale, iter.alignment, drawRect);

				posW.x = drawRect.topLeft.x;
			}

			char prevChar = -1;

			// Write the entire string to the vertex buffer
			while(*str && ((iCurrentVert + 4) < m_pVertexBuffer->GetLength()))
			{
				if (fnt->IsValidCharacter(*str))
				{
					if (!IsSpecialCharacter(*str))
					{
						// Font info about the character to draw
						const CharDescriptor& charInfo = fnt->GetCharDescriptor(*str);

						int kerningOffset = fnt->GetKerningPairOffset(prevChar, *str);

						// Calculate texture coordinates
						glm::vec2 texTopLeft(charInfo.x / (float)(fnt->GetWidth()), charInfo.y / (float)(fnt->GetHeight()));
						glm::vec2 texBottomRight(((charInfo.x + charInfo.Width) / (float)(fnt->GetWidth())), (charInfo.y + charInfo.Height) / (float)(fnt->GetHeight()));

						// Calculate position
						glm::vec3 posTopLeft(posW.x + (charInfo.XOffset + kerningOffset) * iter.scale, posW.y - charInfo.YOffset * iter.scale, posW.z);
						glm::vec3 posBottomRight(posTopLeft.x + charInfo.Width * iter.scale, posTopLeft.y - charInfo.Height * iter.scale, posW.z);

						v[0].pos = posTopLeft;
						v[0].tex = texTopLeft;

						v[1].pos = glm::vec3(posTopLeft.x, posBottomRight.y, posW.z);
						v[1].tex = glm::vec2(texTopLeft.x, texBottomRight.y);

						v[2].pos = glm::vec3(posBottomRight.x, posTopLeft.y, posW.z);
						v[2].tex = glm::vec2(texBottomRight.x, texTopLeft.y);

						v[3].pos = posBottomRight;
						v[3].tex = texBottomRight;

						posW.x += charInfo.XAdvance * iter.scale;

						v += 4;
						iCurrentVert += 4;
						++iSubsetLength;
					}
					else
					{
						ProccessSpecialCharacter(*str, iter.scale, fnt->GetLineHeight(), iter.pos, posW);
					}

					prevChar = *str;
				}
				str++;
			}

			m_stringLength.push_back(iSubsetLength);
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void FontEngine::Render()
{
	// If there is nothing to draw, do nothing
	if(m_strings.empty())
		return;

	FillVertexBuffer();

	m_pVertexBuffer->BindVAO();

	// Get the shader to use
	ApplyTexturedShader currentShader = static_cast<TexturedShader*>(m_pRm->GetResource("textShader", ResourceType::TexturedShader));

	// Set the transformation matrix
	currentShader->SetMVP(m_pCamera->ViewProj());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int uiStartingIndex = 0;
	unsigned int uiSubset = 0;

	// Render all strings
	for(auto& iter : m_strings)
	{
		const IResource* pCurrentTexture = m_pRm->GetResource(iter.first, ResourceType::Font);
		assert(pCurrentTexture != nullptr);

		currentShader->BindTexture(*pCurrentTexture);

		// Render all strings with the same texture
		unsigned int j = 0;
		for(unsigned int i = 0; i < iter.second.size(); ++i, ++uiSubset)
		{
			// Set the color of the text
			currentShader->SetColor(iter.second[i].color);

			// Render all characters that have the same texture
			for (; j < m_stringLength[uiSubset]; ++j, ++uiStartingIndex)
			{
				glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0, 4 * uiStartingIndex);
			}

		}

	}

	glDisable(GL_BLEND);

	m_strings.clear();
	m_stringLength.clear();
}

void FontEngine::NormalizeScaling(const Font* pFont, float& scale) const
{
	scale /= (pFont->GetLineHeight());
}

