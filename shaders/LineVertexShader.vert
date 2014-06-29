#version 330

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 transformation;

void main()
{	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * transformation * vec4(vertexPosition_modelspace,1.0f);
}

