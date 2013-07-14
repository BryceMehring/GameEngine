#include "LineEngine.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>


LineEngine::LineEngine(ResourceManager* pRM, unsigned int maxLength, Camera* pCam)
    : m_iMaxLength(maxLength), m_iCurrentLength(0), m_pCamera(pCam), m_pRM(pRM)
{
	CreateVertexBuffer();

	// Get the range of widths supported by the hardware
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE,m_fMaxWidth);
}
LineEngine::~LineEngine()
{
	glDeleteBuffers(1,&m_uiVertexBuffer);
}

void LineEngine::CreateVertexBuffer()
{
	glGenBuffers(1,&m_uiVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_uiVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iMaxLength,0,GL_DYNAMIC_DRAW);
}

void LineEngine::GetLineWidthRange(glm::vec2& out) const
{
	out.x = m_fMaxWidth[0];
	out.y = m_fMaxWidth[1];
}

void LineEngine::DrawLine(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T)
{
	unsigned int uiNewLength = m_iCurrentLength + uiLength;
	if(uiNewLength >= this->m_iMaxLength)
		return;

	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	LineVertex* pLineVertex = (LineVertex*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iCurrentLength,sizeof(LineVertex) * uiLength,GL_MAP_WRITE_BIT);

	for(unsigned int i = 0; i < uiLength && (i + uiLength < m_iMaxLength) ; ++i)
	{
		glm::vec4 pos = (T * glm::vec4(pArray[i],1.0f));
		pLineVertex[i].pos = glm::vec3(pos.x,pos.y,pos.z);
		pLineVertex[i].color = color;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	LineSubset subset;
	subset.fWidth = glm::clamp(fWidth,m_fMaxWidth[0],m_fMaxWidth[1]);
	subset.uiLength = uiLength;

	m_LineSubsets.push_back(subset);
	m_iCurrentLength = uiNewLength;
}

void LineEngine::OnReset()
{
	glDeleteBuffers(1,&m_uiVertexBuffer);
	CreateVertexBuffer();
}

void LineEngine::Render()
{
	// if there is nothing to draw, do nothing
	if(m_LineSubsets.empty())
		return;

	Shader& theShader = static_cast<Shader&>(m_pRM->GetResource("lineShader"));
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(theShader.id, "vertexPosition_modelspace");
	GLuint vertexColorID = glGetAttribLocation(theShader.id, "vertexColor");

	glUseProgram(theShader.id);

	// set shader parameters
	glUniformMatrix4fv(theShader.uniforms["MVP"],1,false,&m_pCamera->viewProj()[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);
	glVertexAttribPointer(vertexPosition_modelspaceID,3,GL_FLOAT,GL_FALSE,sizeof(LineVertex),(void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(vertexColorID,4,GL_FLOAT,GL_FALSE,sizeof(LineVertex),(void*)sizeof(glm::vec3));

	unsigned int uiStartingIndex = 0;
	for(unsigned int j = 0; j < m_LineSubsets.size(); ++j)
	{
		glLineWidth(m_LineSubsets[j].fWidth);
		glDrawArrays( GL_LINE_STRIP,uiStartingIndex,m_LineSubsets[j].uiLength);

		uiStartingIndex += m_LineSubsets[j].uiLength;
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	m_LineSubsets.clear();
	m_iCurrentLength = 0;
}
