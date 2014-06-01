#include "FontRenderer.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

enum
{
	TRANSFORMATION_LOCATION = 1,
	CHAR_INFO_LOCATION,
	FONT_SIZE_LOCATION = 4,
};

void FontRenderable::Setup(ApplyTexturedShader& shader, const IResource &resource)
{
	const Font* fnt = static_cast<const Font*>(resource.QueryInterface(ResourceType::Font));
	assert("Invalid resource selected" && (fnt != nullptr));

	shader->BindTexture(*fnt);
	shader->SetValue(FONT_SIZE_LOCATION, glm::vec2(fnt->GetWidth(), fnt->GetHeight()));
}

void FontRenderable::Render(ApplyTexturedShader& shader, const IResource& resource)
{
	const Font* fnt = static_cast<const Font*>(resource.QueryInterface(ResourceType::Font));
	assert("Invalid resource selected" && (fnt != nullptr));

	// Text to be rendered
	const char* str = text.c_str();

	glm::vec3 oldPos = pos;

	// If the text needs to be aligned to center or right
	if (alignment != FontAlignment::Left)
	{
		Math::FRECT drawRect(glm::vec2(oldPos.x, oldPos.y));
		GetStringRect(str, fnt, scale, alignment, drawRect);

		oldPos.x = drawRect.topLeft.x;
	}

	// World pos of aligned text to be rendered
	glm::vec3 posW = oldPos;

	char prevChar = -1;

	NormalizeScaling(fnt, scale);

	shader->SetColor(color);

	// Loop over the entire string
	while (*str)
	{
		if (fnt->IsValidCharacter(*str))
		{
			if (!IsSpecialCharacter(*str))
			{
				// Font info about the character to draw
				const CharDescriptor& charInfo = fnt->GetCharDescriptor(*str);
				int kerningOffset = fnt->GetKerningPairOffset(prevChar, *str);

				// Calculate position of text
				glm::vec3 posTopLeft(posW.x + (charInfo.XOffset + kerningOffset) * scale, posW.y - charInfo.YOffset * scale, posW.z);
				glm::vec3 posBottomRight(posTopLeft.x + charInfo.Width * scale, posTopLeft.y - charInfo.Height * scale, posW.z);

				// Advance position after the current character
				posW.x += charInfo.XAdvance * scale;

				// Build transformation matrix to give to the shader
				glm::mat4 T = glm::translate(glm::vec3((posTopLeft + posBottomRight) / 2.0f));
				T = glm::scale(T, glm::vec3(glm::abs(posBottomRight - posTopLeft)));

				// Give data to shader
				shader->SetValue(TRANSFORMATION_LOCATION, T);
				shader->SetValue(CHAR_INFO_LOCATION    , glm::vec2(charInfo.x, charInfo.y));
				shader->SetValue(CHAR_INFO_LOCATION + 1, glm::vec2(charInfo.Width, charInfo.Height));

				// Render a single character of the string
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
			}
			else
			{
				ProccessSpecialCharacter(*str, scale, fnt->GetLineHeight(), oldPos, posW);
			}

			prevChar = *str;
		}
		str++;
	}
}

bool FontRenderable::IsSpecialCharacter(char c)
{
	return ((c == ' ') || (c == '\n') || (c == '\t'));
}

void FontRenderable::ProccessSpecialCharacter(char c, float scale, unsigned int lineHeight, const glm::vec3& oldPos, glm::vec3& currentPos)
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

void FontRenderable::GetStringRect(const char* str, const Font* fnt, float scale, FontAlignment alignment, Math::FRECT& inout)
{
	unsigned int lineHeight = fnt->GetLineHeight();

	NormalizeScaling(fnt, scale);

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

void FontRenderable::AlignTextPos(float width, FontAlignment alignment, glm::vec2& out)
{
	float fHalfWidth = (width / 2.0f);

	out.x -= fHalfWidth;

	if (alignment == FontAlignment::Right)
	{
		out.x -= fHalfWidth;
	}
}

void FontRenderable::NormalizeScaling(const Font* pFont, float& scale)
{
	scale /= (pFont->GetLineHeight());
}
