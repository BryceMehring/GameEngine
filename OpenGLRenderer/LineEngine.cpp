#define GLM_SWIZZLE
#include "LineEngine.h"
#include "VertexStructures.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

LineEngine::LineEngine(ResourceManager* pRM, VertexBuffer* pVertexBuffer, Camera* pCam)
: m_pRM(pRM), m_pVertexBuffer(pVertexBuffer), m_pCamera(pCam), m_iCurrentLength(0)
{
	assert(pVertexBuffer->GetVertexSize() == sizeof(VertexPC));
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

	VertexPC* pLineVertex = static_cast<VertexPC*>(glMapBufferRange(GL_ARRAY_BUFFER,sizeof(VertexPC) * m_iCurrentLength,sizeof(VertexPC) * uiNumVertices,GL_MAP_WRITE_BIT));

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
			pLineVertex[0].color = color;

			pLineVertex[1].pos = (T * glm::vec4(pArray[i - 1].x + px, pArray[i - 1].y - py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[1].color = color;

			pLineVertex[2].pos = (T * glm::vec4(pArray[i].x - px, pArray[i].y + py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[2].color = color;

			pLineVertex[3].pos = (T * glm::vec4(pArray[i].x + px, pArray[i].y - py, pArray[i].z, 1.0f)).xyz();
			pLineVertex[3].color = color;

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

	Shader* pShader = static_cast<Shader*>(m_pRM->GetResource("lineShader", ResourceType::Shader));
	assert(pShader != nullptr);

	glUseProgram(pShader->GetID());

	glUniformMatrix4fv(pShader->GetMVP(),1,false,&m_pCamera->ViewProj()[0][0]);

	m_pVertexBuffer->BindVAO();

	glDrawElements(GL_TRIANGLES, m_iCurrentLength / 4 * 6, GL_UNSIGNED_SHORT, 0);

	m_iCurrentLength = 0;
}
