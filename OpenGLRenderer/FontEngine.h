#ifndef _FONTENGINE_
#define _FONTENGINE_

#include "Camera.h"
#include "IRenderer.h"
#include "ResourceManager.h"
#include <map>

struct DrawTextInfo
{
	DrawTextInfo(const std::string& t, const glm::vec3 p, const glm::vec2& s, const glm::vec3& c, FontAlignment o) : text(t), pos(p), scale(s), color(c), options(o) {}

	std::string text;
	glm::vec3 pos;
	glm::vec2 scale;
	glm::vec3 color;
	FontAlignment options;
};

// todo: move these to another header file
struct FontVertex
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 color;
};

// This class manages the rendering of fonts
class FontEngine
{
public:

	FontEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~FontEngine();

	void GetStringRec(const char* str, const glm::vec2& scale, Math::FRECT& out) const;

	void DrawString(const char* str, const char* font, const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& color, FontAlignment options);

	void Render();

	void OnReset();

	void SetCamera(Camera* pCam) { m_pCamera = pCam; }

private:

	ResourceManager* m_pRm;
	Camera* m_pCamera;
	unsigned int m_iMaxLength;

	std::map<std::string,std::vector<DrawTextInfo> > m_textSubsets; // the key is the texture for the vector of DrawTextInfo
	unsigned int m_uiVertexBuffer;
	unsigned int m_uiIndexBuffer;

	// Fills the vertex buffer with all of the text subsets
	// output is the number of characters to draw for each font texture, in the correct sorted order
	void FillVertexBuffer(std::vector<unsigned int>&);

	void CreateVertexBuffer();
	void CreateIndexBuffer();

	void DeleteBuffers();

    void GetStringRec(const Charset* font,const char* str, const glm::vec2& scale, Math::FRECT& out) const;
};

#endif // _FONTENGINE_
