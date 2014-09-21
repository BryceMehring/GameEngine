#ifndef _FONTRENDERER_
#define _FONTRENDERER_

#include "Camera.h"
#include "IRenderer.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"
#include "IRenderable.h"
#include <map>

// Defines how each string will be rendered
class FontRenderable : public IRenderable
{
public:
	FontRenderable(const std::string& t,
		const glm::vec3 p,
		float s,
		const glm::vec4& c,
		FontAlignment o) : text(t), pos(p),
		scale(s), color(c), alignment(o) {}

	// Gets the string rect from the specified font
	static void GetStringRect(const char* str, const Font* fnt, float scale, FontAlignment alignment, Math::FRECT& out);
	
	void Setup(ApplyShader& shader, const IResource* resource) override;
	void Render(ApplyShader& shader, const IResource* resource) override;

private:

	// Returns true if c is a space, newline, or tab character
	static bool IsSpecialCharacter(char c);

	// Moves the currentPos to take into consideration a non-renderable character
	// c = the non-renderable character
	// scale = scaling of the font
	// lineHeight = height of the line
	// oldPos = origin of the font rendering
	// currentPos = position that needs offsetting
	static void ProccessSpecialCharacter(char c, float scale, unsigned int lineHeight, const glm::vec3& oldPos, glm::vec3& currentPos);

	// Calculates the offset for text alignment
	// width = width of the font
	// alignment = where the text should be placed
	// out = offset to be aligned
	static void AlignTextPos(float width, FontAlignment alignment, glm::vec2& out);

	// Normalize scaling to the height of the font
	static void NormalizeScaling(const Font* pFont, float& scale);

private:

	// Text that will be drawn
	std::string text;

	// Position of the text
	glm::vec3 pos;

	// Scaling of the text
	float scale;

	// Color of the text
	glm::vec4 color;

	// How the text should be placed
	FontAlignment alignment;
};

#endif // _FONTRENDERER_
