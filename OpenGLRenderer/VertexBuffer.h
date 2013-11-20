#ifndef _VERTEXBUFFER_
#define _VERTEXBUFFER_

#include <GL/glew.h>

class VertexBuffer
{
public:

	VertexBuffer(GLuint vertexBufferSize, GLuint bufferLength);
	virtual ~VertexBuffer();

	GLuint GetBuffer() const;
	GLuint GetLength() const;
	GLuint GetSize() const;

private:
	GLuint m_vertexBuffer;
	GLuint m_length;
	GLuint m_size;
};

class IndexedVertexBuffer : public VertexBuffer
{
public:

	IndexedVertexBuffer(GLuint vertexBufferSize, GLuint bufferLength);
	virtual ~IndexedVertexBuffer();

	GLuint GetIndexBuffer() const;

private:

	GLuint m_indexBuffer;

};


#endif // _VERTEXBUFFER_
