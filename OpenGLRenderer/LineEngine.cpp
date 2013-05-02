#include "LineEngine.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>


LineEngine::LineEngine(ResourceManager* pRM, unsigned int maxLength, Camera* pCam)
	: m_iMaxLength(maxLength), m_pCamera(pCam), m_pRM(pRM), m_iCurrentLength(0)
{
	CreateVertexBuffer();
    SetLineWidth(2.0f);
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
    unsigned int uiNewLength = m_iCurrentLength + uiLength;
    if(uiNewLength >= this->m_iMaxLength)
        return;

	glBindBuffer( GL_ARRAY_BUFFER , m_uiVertexBuffer);

	LineVertex* pLineVertex = (LineVertex*)glMapBufferRange(GL_ARRAY_BUFFER,sizeof(LineVertex) * m_iCurrentLength,sizeof(LineVertex) * uiLength,GL_MAP_WRITE_BIT);

	for(unsigned int i = 0; i < uiLength; ++i)
	{
		glm::vec4 pos = (T * glm::vec4(pArray[i],1.0f));
        pLineVertex[i].pos = glm::vec3(pos.x,pos.y,pos.z);
        pLineVertex[i].color = color;
    }

	glUnmapBuffer(GL_ARRAY_BUFFER);

    m_LineSubsets.push_back(uiLength);
    m_iCurrentLength = uiNewLength;
}

void LineEngine::Render()
{
    Shader& theShader = static_cast<Shader&>(m_pRM->GetResource("lineShader"));
    GLuint vertexPosition_modelspaceID = glGetAttribLocation(theShader.id, "vertexPosition_modelspace");
    GLuint vertexColorID = glGetAttribLocation(theShader.id, "vertexColor");
    //unsigned int colorId = theShader.uniforms["lineColor"];

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
        glDrawArrays( GL_LINE_STRIP,uiStartingIndex,m_LineSubsets[j]);

        uiStartingIndex += m_LineSubsets[j];
    }

	glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

	m_LineSubsets.clear();
	m_iCurrentLength = 0;
}
