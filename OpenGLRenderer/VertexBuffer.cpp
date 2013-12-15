#include "VertexBuffer.h"
#include "GenerateBuffers.h"

VertexBuffer::VertexBuffer(GLuint vertexBufferSize, GLuint bufferLength, bool bPCT) : m_length(bufferLength)
{
	glGenVertexArrays(1,&m_arrayObject);
	glBindVertexArray(m_arrayObject);

	glGenBuffers(1,&m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,vertexBufferSize * bufferLength,0,GL_DYNAMIC_DRAW);

	m_indexBuffer = CreateQuadIndexBuffer(bufferLength);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexBufferSize, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexBufferSize, reinterpret_cast<void*>(3 * sizeof(float)));

	if(bPCT)
	{
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexBufferSize, reinterpret_cast<void*>(7 * sizeof(float)));
	}
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1,&m_vertexBuffer);
	glDeleteBuffers(1,&m_indexBuffer);
	glDeleteVertexArrays(1,&m_arrayObject);
}

void VertexBuffer::Bind() const
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



