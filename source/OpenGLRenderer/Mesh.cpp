#include "Mesh.h"

Mesh::Mesh() : m_mode(GL_TRIANGLES), m_count(6), m_type(GL_UNSIGNED_SHORT)
{
	unsigned short indexBuffer[6] = { 0, 2, 1,	2, 3, 1 };
	VertexPT verticies[] =
	{
		{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0)},
		{glm::vec3(-0.5f, -0.5f, 0.0), glm::vec2(0,1)},
		{glm::vec3(0.5f, 0.5f, 0.0), glm::vec2(1,0)},
		{glm::vec3(0.5f, -0.5f, 0.0), glm::vec2(1)}
	};

	m_buffer.reset(new VertexBuffer(verticies, sizeof(VertexPT), 4, GL_STATIC_DRAW, indexBuffer, sizeof(unsigned short), 6));
}

void Mesh::Bind()
{
	m_buffer->BindVAO();
}

void Mesh::Draw() const
{
	glDrawElements(m_mode, m_count, m_type, 0);
}
