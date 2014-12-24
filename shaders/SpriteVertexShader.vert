#version 330

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 transformation;
uniform vec2 tileSize;
uniform vec2 tiling;
uniform int tileIndex;

void main()
{
	// Output position of the vertex, in clip space
	gl_Position = MVP * transformation * vec4(vertexPosition_modelspace,1);

	vec2 uvOffset = vec2(mod(tileIndex,tileSize.x), floor(tileIndex / tileSize.x));

	UV = (uvOffset + vertexUV) / tileSize * tiling;
}
