#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "ResourceManager.h"
#include "Camera.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <VecMath.h>

struct DrawTextInfo
{
	DrawTextInfo() {}
	DrawTextInfo(const std::string& str, const glm::vec2& p, const glm::vec2& s, const glm::vec3& c) :
		text(str), pos(p), scale(s), color(c), length(0) {}

	std::string text;
	glm::vec2 pos;
	glm::vec2 scale;
	glm::vec3 color;
	unsigned int length;
};

// todo: move these to another header file
struct FontVertex
{
	glm::vec3 pos;
	glm::vec2 tex;
};

// This class manages the rendering of fonts
class FontEngine
{
public:

	FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~FontEngine();

	void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const;

	void DrawString(const char* str, const char* font, const glm::vec2& pos, const glm::vec2& scale, const glm::vec3& color);

	void Render();

	void OnReset();

private:

	ResourceManager* m_pRm;
	Camera* m_pCamera;
	unsigned int m_iMaxLength;

	std::map<std::string,std::vector<DrawTextInfo> > m_textSubsets; // the key is the texture for the vector of DrawTextInfo
	unsigned int m_uiVertexBuffer;
	unsigned int m_uiIndexBuffer;

	void FillVertexBuffer();

	void CreateVertexBuffer();
	void CreateIndexBuffer();
};

#endif // _FONTENGINE_
