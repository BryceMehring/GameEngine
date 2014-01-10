#ifndef _VERTEXBUFFER_
#define _VERTEXBUFFER_

#include <GL/glew.h>
#include "VertexStructures.h"

// Defines a a vertex buffer which manages the creation buffers(vao and vbo) which are
// needed to render objects
class VertexBuffer
{
public:

	// Construct a VertexBuffer,
	// vertexBufferSize: the size of the vertex structure in bytes
	// bufferLength: the number of vertices to be part of the vertex buffer
	// bPCT: should be true if the vertex structure uses a texture, else false
	VertexBuffer(GLuint vertexBufferSize, GLuint bufferLength, bool bPCT = true);
	~VertexBuffer();

	// This class cannot be copied
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator = (const VertexBuffer&) = delete;

	// Binds the vertex buffer object
	void BindVBO() const;

	// Binds the vertex array object
	void BindVAO() const;

	// Returns the number of vertices part of the vertex buffer
	GLuint GetLength() const;

	// Returns the size of the vertex buffer in bytes
	GLuint GetSize() const;

	// Returns the size of the vertex structure in bytes
	GLuint GetVertexSize() const;

private:

	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	GLuint m_arrayObject;
	GLuint m_length;
	GLuint m_size;
	
};


#endif // _VERTEXBUFFER_
