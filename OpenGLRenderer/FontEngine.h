#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "ResourceManager.h"
#include "Camera.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

struct DrawTextInfo
{
	DrawTextInfo() {}
	DrawTextInfo(const std::string& str, const std::string& f, const glm::vec2& p, const glm::vec2& s, const glm::vec4& c) :
	text(str), font(f), pos(p), scale(s), color(c), length(0) {}

	std::string text;
	std::string font;
	glm::vec2 pos;
	glm::vec2 scale;
	glm::vec4 color;
	unsigned int length;
};

// todo: move these to another header file
struct FontVertex
{
	glm::vec3 pos;
	glm::vec2 tex;
};


class FontEngine
{
public:

	FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~FontEngine();

	void Render();

	void DrawString(const char* str, const char* font, const glm::vec2& pos, const glm::vec2& scale, const glm::vec4& color);

private:

	ResourceManager* m_pRm;
	Camera* m_pCamera;
	unsigned int m_iMaxLength;

	std::vector<DrawTextInfo> m_textSubsets;
	std::vector<unsigned int> m_VertexBuffers;

	std::vector<GLsync> m_fences;
	unsigned int m_iFenceIndex;

	unsigned int m_uiIndexBuffer;

	void FillVertexBuffer();

	void CreateVertexBuffer();
	void CreateIndexBuffer();

	// specifies how far apart each character is in a sentence
	glm::vec2 GetTextOffset(const glm::vec2& scale, unsigned int i, unsigned int j) const;

};

#endif // _FONTENGINE_
