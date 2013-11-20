#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "IRenderer.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"

struct LineVertex
{
	glm::vec3 pos;
	glm::vec4 color;
};

struct LineSubset
{
	LineSubset(unsigned int length, float width) : uiLength(length), fWidth(width) {}

	unsigned int uiLength;
	float fWidth;
};

class LineEngine
{
public:

	LineEngine(ResourceManager* pRm, VertexBuffer* pVertexStructure, RenderSpace space, Camera* pCam = nullptr);

	void GetLineWidthRange(float& min, float& max) const;

	void DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T);

	void Render();

	void SetCamera(Camera* pCam);

private:

	ResourceManager* m_pRM;
	VertexBuffer* m_pVertexBuffer;
	Camera* m_pCamera;

	unsigned int m_iCurrentLength;

	float m_lineSizeMin;
	float m_lineSizeMax;

	RenderSpace m_renderSpace;

	std::vector<LineSubset> m_LineSubsets;

	void CreateVertexBuffer();

	// Get the range of widths supported by the hardware
	void GetLineWidthRange();

};

#endif // _LINEENGINE_
