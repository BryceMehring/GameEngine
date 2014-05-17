#include "FontEngine.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

void DrawTextInfo::Setup(ApplyTexturedShader& shader, const IResource &resource)
{
	const Font* fnt = static_cast<const Font*>(resource.QueryInterface(ResourceType::Font));
	assert("Invalid resource selected" && (fnt != nullptr));

	shader->BindTexture(*fnt);
	shader->SetValue("fontSize", glm::vec2(fnt->GetWidth(), fnt->GetHeight()));
	shader->SetColor(color);
}

void DrawTextInfo::Render(ApplyTexturedShader& shader, const IResource& resource)
{
	const Font* fnt = static_cast<const Font*>(resource.QueryInterface(ResourceType::Font));
	assert("Invalid resource selected" && (fnt != nullptr));

	Setup(shader, resource);

	NormalizeScaling(fnt, scale);

	// Text to be rendered
	const char* str = text.c_str();

	//int currentScale = 

	// World pos of aligned text to be rendered
	glm::vec3 posW(pos);

	// If the text needs to be aligned to center or right
	if (alignment != FontAlignment::Left)
	{
		Math::FRECT drawRect(glm::vec2(posW.x, posW.y));
		GetStringRect(str, fnt, scale, alignment, drawRect);

		posW.x = drawRect.topLeft.x;
	}

	char prevChar = -1;

	while (*str)
	{
		if (fnt->IsValidCharacter(*str))
		{
			if (!IsSpecialCharacter(*str))
			{
				// Font info about the character to draw
				const CharDescriptor& charInfo = fnt->GetCharDescriptor(*str);
				int kerningOffset = fnt->GetKerningPairOffset(prevChar, *str);

				// Calculate position
				glm::vec3 posTopLeft(posW.x + (charInfo.XOffset + kerningOffset) * scale, posW.y - charInfo.YOffset * scale, posW.z);
				glm::vec3 posBottomRight(posTopLeft.x + charInfo.Width * scale, posTopLeft.y - charInfo.Height * scale, posW.z);

				// Advance position after the current character
				posW.x += charInfo.XAdvance * scale;

				// Build transformation matrix to give to the shader
				glm::mat4 T = glm::translate(glm::vec3((posTopLeft + posBottomRight) / 2.0f));
				T = glm::scale(T, glm::vec3(glm::abs(posBottomRight - posTopLeft)));

				// Give data to shader
				shader->SetValue("transformation", T);
				shader->SetValue("charInfo.pos", glm::vec2(charInfo.x, charInfo.y));
				shader->SetValue("charInfo.size", glm::vec2(charInfo.Width, charInfo.Height));

				// Render a single character of the string
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
			}
			else
			{
				ProccessSpecialCharacter(*str, scale, fnt->GetLineHeight(), pos, posW);
			}

			prevChar = *str;
		}
		str++;
	}
}

bool DrawTextInfo::IsSpecialCharacter(char c)
{
	return ((c == ' ') || (c == '\n') || (c == '\t'));
}

void DrawTextInfo::ProccessSpecialCharacter(char c, float scale, unsigned int lineHeight, const glm::vec3& oldPos, glm::vec3& currentPos)
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

void DrawTextInfo::GetStringRect(const char* str, const Font* fnt, float scale, FontAlignment alignment, Math::FRECT& inout)
{
	unsigned int lineHeight = fnt->GetLineHeight();

	inout.bottomRight = inout.topLeft;
	inout.bottomRight.y -= scale * lineHeight;

	glm::vec3 pos(inout.topLeft.x, inout.bottomRight.y, 0.0f);

	while (*str)
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

void DrawTextInfo::AlignTextPos(float width, FontAlignment alignment, glm::vec2& out)
{
	float fHalfWidth = (width / 2.0f);

	out.x -= fHalfWidth;

	if (alignment == FontAlignment::Right)
	{
		out.x -= fHalfWidth;
	}
}

void DrawTextInfo::NormalizeScaling(const Font* pFont, float& scale)
{
	scale /= (pFont->GetLineHeight());
}

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

		//NormalizeScaling(pFont, scale);

		DrawTextInfo::GetStringRect(str, pFont, scale, alignment, out);
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

		//NormalizeScaling(pFont, scale);
		
		m_strings[font].push_back(DrawTextInfo(str, pos, scale, color, alignment));
	}
}

void FontEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void FontEngine::Render()
{
	// If there is nothing to draw, do nothing
	if(m_strings.empty())
		return;

	m_pVertexBuffer->BindVAO();

	// Get the shader to use
	ApplyTexturedShader currentShader = static_cast<TexturedShader*>(m_pRm->GetResource("textShader", ResourceType::TexturedShader));

	// Set the transformation matrix
	currentShader->SetMVP(m_pCamera->ViewProj());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render all strings
	for(auto& fontIter : m_strings)
	{
		// Get the current font
		const Font* fnt = static_cast<Font*>(m_pRm->GetResource(fontIter.first, ResourceType::Font));
		assert(fnt != nullptr);

		currentShader->BindTexture(*fnt);
		currentShader->SetValue("fontSize",glm::vec2(fnt->GetWidth(),fnt->GetHeight()));

		// Render all strings with the current font
		for(auto& iter : fontIter.second)
		{
		}
	}

	glDisable(GL_BLEND);

	m_strings.clear();
}