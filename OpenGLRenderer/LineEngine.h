#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "Camera.h"
#include "ResourceManager.h"

struct LineVertex
{
	glm::vec3 pos;
	glm::vec4 color;
};

struct LineSubset
{
	unsigned int uiLength;
	float fWidth;
};

class LineEngine
{
public:

	LineEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~LineEngine();

	void GetLineWidthRange(glm::vec2& out) const;

	void DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T);

	void Render();

	void OnReset();

private:

	unsigned int m_uiVertexBuffer;
	unsigned int m_iMaxLength;
	unsigned int m_iCurrentLength;
	Camera* m_pCamera;
	ResourceManager* m_pRM;

	float m_fMaxWidth[2];

	std::vector<LineSubset> m_LineSubsets;

	void CreateVertexBuffer();

};

#endif // _LINEENGINE_
