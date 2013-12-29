#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "Camera.h"
#include "IRenderer.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"
#include <map>

struct DrawTextInfo
{
	DrawTextInfo(const std::string& t, const glm::vec3 p, float s, const glm::vec4& c, FontAlignment o) : text(t), pos(p), scale(s), color(c), alignment(o) {}

	std::string text;
	glm::vec3 pos;
	float scale;
	glm::vec4 color;
	FontAlignment alignment;
};

// This class manages the rendering of fonts
class FontEngine
{
public:

	FontEngine(ResourceManager* pRm, VertexBuffer* pVertexStructure, Camera* pCam = nullptr);

	void GetStringRec(const char* str, float scale, FontAlignment alignment, Math::FRECT& inout) const;

	void DrawString(const char* str, const char* font, const glm::vec3& pos, float scale, const glm::vec4& color, FontAlignment alignment);

	void Render();

	void SetCamera(Camera* pCam);

private:

	ResourceManager* m_pRm;
	VertexBuffer* m_pVertexBuffer;
	Camera* m_pCamera;

	// the key is the texture for the vector of DrawTextInfo
	std::map<std::string,std::vector<DrawTextInfo>> m_textSubsets;

	void AlignTextPos(float width, FontAlignment alignment, glm::vec2& out) const;

	// returns true if c is a space, newline, or tab character
	bool IsSpecialCharacter(char c) const;

	void ProccessSpecialCharacter(char c, float scale, unsigned int lineHeight, const glm::vec3& oldPos, glm::vec3& currentPos) const;

	// Fills the vertex buffer with all of the text subsets
	// output is the number of characters to draw in each subset
	void FillVertexBuffer(std::vector<unsigned int>&);

	void GetStringRec(const Charset* font, const char* str, float scale, FontAlignment alignment, Math::FRECT& out) const;
};

#endif // _FONTENGINE_
