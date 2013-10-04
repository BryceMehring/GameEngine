#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "IRenderer.h"
#include "Camera.h"
#include "ResourceManager.h"

struct LineVertex
{
	glm::vec3 pos;
	glm::vec4 color;
};

struct LineSubset
{
	LineSubset(unsigned int length, float width, RenderSpace rs) :
		uiLength(length), fWidth(width), space(rs) {}

	unsigned int uiLength;
	float fWidth;
	RenderSpace space;
};

class LineEngine
{
public:

	LineEngine(ResourceManager* pRm, unsigned int maxLength, Camera* pCam, Camera* pOrthoCam);
	~LineEngine();

	void GetLineWidthRange(float& min, float& max) const;

	void DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T, RenderSpace space);

	void Render();

	void OnReset();

	void SetCamera(Camera* pCam) { m_pCamera[0] = pCam; }

private:

	unsigned int m_uiVertexBuffer;
	unsigned int m_iMaxLength;
	unsigned int m_iCurrentLength;
	Camera* m_pCamera[2];
	ResourceManager* m_pRM;

	float m_lineSizeMin;
	float m_lineSizeMax;

	std::vector<LineSubset> m_LineSubsets;

	void CreateVertexBuffer();

	// Get the range of widths supported by the hardware
	void GetLineWidthRange();

};

#endif // _LINEENGINE_
