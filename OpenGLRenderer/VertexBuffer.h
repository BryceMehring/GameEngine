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
	// pVertexBuffer: pointer to the vertex buffer. May be null if the buffer is dynamic or streamed
	// vertexStructureSize: the size of the vertex structure in bytes
	// vertexBufferLength: the number of vertices to be part of the vertex buffer
	// usage: Specifies the usage of the vertex buffer. Can be GL_STATIC_DRAW, GL_STREAM_DRAW, or GL_DYNAMIC_DRAW
	// pIndexBuffer: pointer to the index buffer. This pointer cannot be null
	// indexBufferLength: length of the index buffer
	// bPT: should be true if the vertex structure uses a texture, else false
	VertexBuffer(void* pVertexBuffer, GLuint vertexStructureSize, GLuint vertexBufferLength, GLenum usage, const unsigned char* pIndexBuffer, GLuint indexBufferLength, bool bPT = true);
	~VertexBuffer();

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

	// This class cannot be copied
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator = (const VertexBuffer&) = delete;
	
};


#endif // _VERTEXBUFFER_
