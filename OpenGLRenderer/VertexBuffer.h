#ifndef _VERTEXBUFFER_
#define _VERTEXBUFFER_

#include <GL/glew.h>
#include "VertexStructures.h"

class VertexBuffer
{
public:

	VertexBuffer(GLuint vertexBufferSize, GLuint bufferLength, bool bPCT = true);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator = (const VertexBuffer&) = delete;

	void Bind() const;
	void BindVAO() const;

	GLuint GetLength() const;

private:

	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	GLuint m_arrayObject;
	GLuint m_length;
};


#endif // _VERTEXBUFFER_
