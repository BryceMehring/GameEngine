#include "VertexStructures.h"
#include "GenerateBuffers.h"

VertexStructure::VertexStructure(GLuint vertexBufferSize, GLuint bufferLength) : m_length(bufferLength), m_size(vertexBufferSize * bufferLength)
{
	m_indexBuffer = CreateQuadIndexBuffer(bufferLength);

	glGenBuffers(1,&m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,vertexBufferSize * bufferLength,0,GL_DYNAMIC_DRAW);
}

VertexStructure::~VertexStructure()
{
	glDeleteBuffers(1,&m_indexBuffer);
	glDeleteBuffers(1,&m_vertexBuffer);
}

GLuint VertexStructure::GetIndexBuffer() const
{
	return m_indexBuffer;
}

GLuint VertexStructure::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

GLuint VertexStructure::GetLength() const
{
	return m_length;
}

GLuint VertexStructure::GetSize() const
{
	return m_size;
}




