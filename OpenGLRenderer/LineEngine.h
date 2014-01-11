#ifndef _LINEENGINE_
#define _LINEENGINE_

#include "IRenderer.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "VertexBuffer.h"

class LineEngine
{
public:

	LineEngine(ResourceManager* pRm, VertexBuffer* pVertexStructure, Camera* pCam = nullptr);

	void DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T);

	void Render();

	void SetCamera(Camera* pCam);

private:

	ResourceManager* m_pRM;
	VertexBuffer* m_pVertexBuffer;
	Camera* m_pCamera;

	std::vector<glm::vec4> m_lineColorSubset;

	unsigned int m_iCurrentLength;
};

#endif // _LINEENGINE_
