#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "camera.h"
#include "ResourceManager.h"

struct LineVertex
{
	glm::vec3 pos;
};

struct Line
{
	Line(unsigned int l, const glm::vec4& c) : length(l), color(c) {}

	unsigned int length;
	glm::vec4 color;
};

class LineEngine
{
public:

	LineEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam);
	~LineEngine();

	void SetLineWidth(float width);

	void DrawLine(const glm::vec3* pArray, unsigned int uiLength, const glm::vec4& color, const glm::mat4& T);

	void Render();

private:

	unsigned int m_uiVertexBuffer;
	unsigned int m_iMaxLength;
	unsigned int m_iCurrentLength;
	Camera* m_pCamera;
	ResourceManager* m_pRM;

	std::vector<Line> m_LineSubsets;

	void CreateVertexBuffer();

};

#endif // _LINEENGINE_