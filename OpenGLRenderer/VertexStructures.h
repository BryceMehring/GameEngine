#ifndef _VERTEXSTRUCTURES_
#define _VERTEXSTRUCTURES_

#include <GL/glew.h>

class VertexStructure
{
public:

	VertexStructure(GLuint vertexBufferSize, GLuint bufferLength);
	virtual ~VertexStructure();

	GLuint GetVertexBuffer() const;
	GLuint GetLength() const;
	GLuint GetSize() const;

private:
	GLuint m_vertexBuffer;
	GLuint m_length;
	GLuint m_size;
};

class IndexedVertexStructure : public VertexStructure
{
public:

	IndexedVertexStructure(GLuint vertexBufferSize, GLuint bufferLength);
	virtual ~IndexedVertexStructure();

	GLuint GetIndexBuffer() const;

private:

	GLuint m_indexBuffer;

};


#endif // _VERTEXSTRUCTURES_
