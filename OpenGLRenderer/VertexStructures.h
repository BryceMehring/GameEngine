#ifndef __VERTEXSTRUCTURES__
#define __VERTEXSTRUCTURES__

#include <glm/glm.hpp>

struct VertexPTC
{
	glm::vec3 pos;
	glm::vec2 tex;
	glm::vec3 color;
};

struct VertexPC
{
	glm::vec3 pos;
	glm::vec4 color;
};

#endif // __VERTEXSTRUCTURES__
