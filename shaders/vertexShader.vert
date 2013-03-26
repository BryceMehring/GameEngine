#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 texCoords;
out vec2 UV;

uniform mat4 MVP;

void main()
{
	gl_Position =   MVP * vec4(Position,1.0f);
	UV = texCoords;
}