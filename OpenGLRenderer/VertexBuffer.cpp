#include "VertexBuffer.h"
#include "GenerateBuffers.h"

VertexBuffer::VertexBuffer(GLuint vertexBufferSize, GLuint bufferLength) : m_length(bufferLength), m_size(vertexBufferSize * bufferLength)
{
	glGenBuffers(1,&m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,vertexBufferSize * bufferLength,0,GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1,&m_vertexBuffer);
}

void VertexBuffer::Bind() const
{
	glBindBuffer( GL_ARRAY_BUFFER , m_vertexBuffer);
}

GLuint VertexBuffer::GetLength() const
{
	return m_length;
}

GLuint VertexBuffer::GetSize() const
{
	return m_size;
}

IndexedVertexBuffer::IndexedVertexBuffer(GLuint vertexBufferSize, GLuint bufferLength) : VertexBuffer(vertexBufferSize,bufferLength)
{
	m_indexBuffer = CreateQuadIndexBuffer(bufferLength);
}
IndexedVertexBuffer::~IndexedVertexBuffer()
{
	glDeleteBuffers(1,&m_indexBuffer);
}

void IndexedVertexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBuffer);
	VertexBuffer::Bind();
}




