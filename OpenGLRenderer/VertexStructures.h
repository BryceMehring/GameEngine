#ifndef _VERTEXSTRUCTURES_
#define _VERTEXSTRUCTURES_

#include <GL/glew.h>

class VertexStructure
{
public:

	VertexStructure(GLuint vertexBufferSize, GLuint bufferLength);
	~VertexStructure();

	GLuint GetIndexBuffer() const;
	GLuint GetVertexBuffer() const;
	GLuint GetLength() const;
	GLuint GetSize() const;

private:
	GLuint m_indexBuffer;
	GLuint m_vertexBuffer;
	GLuint m_length;
	GLuint m_size;
};


#endif // _VERTEXSTRUCTURES_
