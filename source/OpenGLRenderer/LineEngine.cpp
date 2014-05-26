#define GLM_SWIZZLE
#include "LineEngine.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

LineEngine::LineEngine(ResourceManager* pRM, VertexBuffer* pVertexBuffer, Camera* pCam)
: m_pRM(pRM), m_pVertexBuffer(pVertexBuffer), m_pCamera(pCam), m_iCurrentLength(0)
{
	assert(pVertexBuffer->GetVertexSize() == sizeof(VertexP));
}

void LineEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void LineEngine::DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	unsigned int uiNumVertices = (uiLength - 1) * 4;
	unsigned int uiNewLength = m_iCurrentLength + uiNumVertices;
	if(uiNewLength >= m_pVertexBuffer->GetLength())
		return;

	m_pVertexBuffer->BindVBO();

	VertexP* pLineVertex = static_cast<VertexP*>(glMapBufferRange(GL_ARRAY_BUFFER,sizeof(VertexP) * m_iCurrentLength,sizeof(VertexP) * uiNumVertices,GL_MAP_WRITE_BIT));
	
	for(unsigned int i = 1; i < uiLength; ++i)
	{
		float dx = pArray[i].x - pArray[i - 1].x;
		float dy = pArray[i].y - pArray[i - 1].y;

		float linelength = sqrt(dx * dx + dy * dy);

		if (linelength != 0.0f)
		{
			dx /= linelength;
			dy /= linelength;

			const float px = 0.5f * fWidth * dy;
			const float py = 0.5f * fWidth * dx;

			pLineVertex[0].pos = (T * glm::vec4(pArray[i - 1].x - px, pArray[i - 1].y + py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[1].pos = (T * glm::vec4(pArray[i - 1].x + px, pArray[i - 1].y - py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[2].pos = (T * glm::vec4(pArray[i].x - px, pArray[i].y + py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[3].pos = (T * glm::vec4(pArray[i].x + px, pArray[i].y - py, pArray[i].z, 1.0f)).xyz();

			m_lineColorSubset.push_back(color);

			pLineVertex += 4;
			m_iCurrentLength += 4;
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void LineEngine::Render()
{
	if(m_iCurrentLength == 0)
		return;

	ApplyShader currentShader = static_cast<Shader*>(m_pRM->GetResource("lineShader", ResourceType::Shader));

	currentShader->SetMVP(m_pCamera->ViewProj());

	m_pVertexBuffer->BindVAO();

	for (unsigned int i = 0; i < m_lineColorSubset.size(); ++i)
	{
		currentShader->SetColor(m_lineColorSubset[i]);
		glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0, 4 * i);
	}

	m_iCurrentLength = 0;
	m_lineColorSubset.clear();
}
