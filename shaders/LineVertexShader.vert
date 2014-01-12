#version 330

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

out vec4 color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec4 uniformColor;

void main()
{	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1.0f);
	color = uniformColor;
}

