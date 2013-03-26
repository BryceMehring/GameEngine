#include "LineEngine.h"
#include <GL\glew.h>
#include <glm\gtx\transform.hpp>


LineEngine::LineEngine(ResourceManager* pRM, unsigned int maxLength, Camera* pCam)
	: m_iMaxLength(maxLength), m_pCamera(pCam), m_pRM(pRM), m_iCurrentLength(0)
{
	CreateVertexBuffer();
	SetLineWidth(5.0f);
}
LineEngine::~LineEngine()
{
	glDeleteBuffers(1,&m_uiVertexBuffer);
}

void LineEngine::SetLineWidth(float width)
{
	glLineWidth(width);
}

void LineEngine::CreateVertexBuffer()
{
	glGenBuffers(1,&m_uiVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_uiVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iMaxLength,0,GL_DYNAMIC_DRAW);
}

void LineEngine::DrawLine(const glm::vec3* pArray, unsigned int uiLength, const glm::vec4& color, const glm::mat4& T)
{
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	LineVertex* pLineVertex = (LineVertex*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iCurrentLength,sizeof(LineVertex) * uiLength,GL_MAP_WRITE_BIT);

	for(unsigned int i = 0; i < uiLength; ++i)
	{
		glm::vec4 pos = (T * glm::vec4(pArray[i],1.0f));
		pLineVertex[i].pos = glm::vec3(pos.x,pos.y,pos.z);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	m_LineSubsets.push_back(Line(uiLength,color));
	m_iCurrentLength += uiLength;
}

void LineEngine::Render()
{
	ResourceManager::Shader& theShader = static_cast<ResourceManager::Shader&>(m_pRM->GetResource("lineShader"));
	unsigned int colorId = theShader.uniforms["lineColor"];

	glUseProgram(theShader.id);

	// set shader parameters
	glUniformMatrix4fv(theShader.uniforms["MVP"],1,false,&m_pCamera->viewProj()[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(LineVertex),(void*)0);

	unsigned int uiStartingIndex = 0;
	for(unsigned int j = 0; j < m_LineSubsets.size(); ++j)
	{
		glUniform4f(colorId,m_LineSubsets[j].color.x,m_LineSubsets[j].color.y,m_LineSubsets[j].color.z,m_LineSubsets[j].color.w);
		glDrawArrays(GL_LINE_STRIP,uiStartingIndex,m_LineSubsets[j].length);

		uiStartingIndex += m_LineSubsets[j].length;
	}

	glDisableVertexAttribArray(0);

	m_LineSubsets.clear();
	m_iCurrentLength = 0;
}