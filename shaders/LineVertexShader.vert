#version 277 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 Position;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(Position,1.0f);
}
