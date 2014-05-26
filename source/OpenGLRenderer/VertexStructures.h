#ifndef __VERTEXSTRUCTURES__
#define __VERTEXSTRUCTURES__

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct VertexP
{
	glm::vec3 pos;
};

struct VertexPT
{
	glm::vec3 pos;
	glm::vec2 tex;
};

#endif // __VERTEXSTRUCTURES__
