#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "Camera.h"
#include "IRenderer.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"
#include "IRenderable.h"
#include <map>

// Defines how each string will be rendered
class DrawTextInfo : public IRenderable
{
public:
	DrawTextInfo(const std::string& t,
		const glm::vec3 p,
		float s,
		const glm::vec4& c,
		FontAlignment o) : text(t), pos(p),
		scale(s), color(c), alignment(o) {}

	// Gets the string rect from the specified font
	static void GetStringRect(const char* str, const Font* fnt, float scale, FontAlignment alignment, Math::FRECT& out);
	
	virtual void Setup(ApplyTexturedShader& shader, const IResource &resource);
	virtual void Render(ApplyTexturedShader& shader, const IResource& resource);

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

// This class manages the rendering of fonts
class FontEngine
{
public:

	FontEngine(ResourceManager* pRm, VertexBuffer* pVertexBuffer, Camera* pCam = nullptr);

	void GetStringRect(const char* str, float scale, FontAlignment alignment, Math::FRECT& inout) const;
	void DrawString(const char* str, const char* font, const glm::vec3& pos, float scale, const glm::vec4& color, FontAlignment alignment);
	void SetCamera(Camera* pCam);

	// Renders all of the cached strings
	void Render();

private:

	ResourceManager* m_pRm;
	VertexBuffer* m_pVertexBuffer;
	Camera* m_pCamera;

	// The key is the texture for the vector of DrawTextInfo
	std::map<std::string, std::vector<DrawTextInfo>> m_strings;
};

#endif // _FONTENGINE_
