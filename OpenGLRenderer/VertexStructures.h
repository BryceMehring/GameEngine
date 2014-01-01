#ifndef __VERTEXSTRUCTURES__
#define __VERTEXSTRUCTURES__

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct VertexPCT
{
	glm::vec3 pos;
	glm::vec4 color;
	glm::vec2 tex;
};

struct VertexPC
{
	glm::vec3 pos;
	glm::vec4 color;
};



#endif // __VERTEXSTRUCTURES__
