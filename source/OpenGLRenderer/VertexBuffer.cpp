#include "VertexBuffer.h"

#include <vector>

VertexBuffer::VertexBuffer(void* pVertexBuffer, GLuint vertexStructureSize, GLuint vertexBufferLength, GLenum usage, void* pIndexBuffer, GLuint indexSize, GLuint indexBufferLength, bool bTexture) :
	m_length(vertexBufferLength), m_size(vertexStructureSize * vertexBufferLength)
{
	assert(pIndexBuffer != nullptr);

	glGenVertexArrays(1,&m_arrayObject);
	glBindVertexArray(m_arrayObject);

	glGenBuffers(1,&m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);

	glBufferData(GL_ARRAY_BUFFER, m_size, pVertexBuffer, usage);

	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferLength * indexSize, pIndexBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStructureSize, 0);

	if(bTexture)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexStructureSize, reinterpret_cast<void*>(3 * sizeof(float)));
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1,&m_vertexBuffer);
	glDeleteBuffers(1,&m_indexBuffer);
	glDeleteVertexArrays(1,&m_arrayObject);
}

void VertexBuffer::BindVBO() const
{
	glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
}

void VertexBuffer::BindVAO() const
{
	glBindVertexArray(m_arrayObject);
}

GLuint VertexBuffer::GetLength() const
{
	return m_length;
}

GLuint VertexBuffer::GetSize() const
{
	return m_size;
}

GLuint VertexBuffer::GetVertexSize() const
{
	return m_size / m_length;
}







