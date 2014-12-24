#pragma once
#include "VertexBuffer.h"
#include <memory>

// TODO: allow the user of the class to change the type of the mesh instead of always rendering a quad. Maybe even load the information from a data file.
// also add some more comments
class Mesh
{
public:

	Mesh();

	void Bind();
	void Draw() const;

private:
	std::unique_ptr<VertexBuffer> m_buffer;
	GLenum m_mode;
	GLint m_count;
	GLenum m_type;
};
