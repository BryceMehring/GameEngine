#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
attribute vec2 vertexUV;
attribute vec2 vertexTiling;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;
varying vec2 pos;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main()
{

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV * vertexTiling;
	pos = vec2(gl_Position.x,gl_Position.y);
}

