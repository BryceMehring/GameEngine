#include "LineEngine.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

LineEngine::LineEngine(ResourceManager* pRM, VertexBuffer* pVertexStructure, RenderSpace space, Camera* pCam)
	: m_pRM(pRM), m_pVertexBuffer(pVertexStructure), m_pCamera(pCam), m_iCurrentLength(0), m_renderSpace(space)
{
	GetLineWidthRange();
}

void LineEngine::SetCamera(Camera* pCam)
{
	m_pCamera = pCam;
}

void LineEngine::GetLineWidthRange(float& min, float& max) const
{
	min = m_lineSizeMin;
	max = m_lineSizeMax;
}

void LineEngine::GetLineWidthRange()
{
	float lineWidth[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,lineWidth);

	m_lineSizeMin = lineWidth[0];
	m_lineSizeMax = lineWidth[1];
}

void LineEngine::DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	unsigned int uiNewLength = m_iCurrentLength + uiLength;
	if(uiNewLength >= m_pVertexBuffer->GetLength())
		return;

	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());

	LineVertex* pLineVertex = static_cast<LineVertex*>(glMapBufferRange(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iCurrentLength,sizeof(LineVertex) * uiLength,GL_MAP_WRITE_BIT));

	for(unsigned int i = 0; i < uiLength && (i + uiLength < m_pVertexBuffer->GetLength()) ; ++i)
	{
		glm::vec4 pos = (T * glm::vec4(pArray[i],1.0f));
		pLineVertex[i].pos = glm::vec3(pos.x,pos.y,pos.z);
		pLineVertex[i].color = color;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_LineSubsets.push_back(LineSubset(uiLength,glm::clamp(fWidth,m_lineSizeMin,m_lineSizeMax)));
	m_iCurrentLength = uiNewLength;
}

void LineEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_LineSubsets.empty())
		return;

	Shader* pShader = static_cast<Shader*>(m_pRM->GetResource("lineShader"));
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(pShader->GetID(), "vertexPosition_modelspace");
	GLuint vertexColorID = glGetAttribLocation(pShader->GetID(), "vertexColor");

	glUseProgram(pShader->GetID());

	glEnableVertexAttribArray(0);
	glBindBuffer( GL_ARRAY_BUFFER , m_pVertexBuffer->GetBuffer());
	glVertexAttribPointer(vertexPosition_modelspaceID,3,GL_FLOAT,GL_FALSE,sizeof(LineVertex),0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(vertexColorID,4,GL_FLOAT,GL_FALSE,sizeof(LineVertex),reinterpret_cast<void*>(sizeof(glm::vec3)));

	glUniformMatrix4fv(pShader->GetMVP(),1,false,&m_pCamera->viewProj()[0][0]);

	unsigned int uiStartingIndex = 0;

	for(unsigned int j = 0; j < m_LineSubsets.size(); ++j)
	{
		glLineWidth(m_LineSubsets[j].fWidth);
		glDrawArrays(GL_LINE_STRIP,uiStartingIndex,m_LineSubsets[j].uiLength);

		uiStartingIndex += m_LineSubsets[j].uiLength;
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	m_LineSubsets.clear();
	m_iCurrentLength = 0;
}
