#version 430

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

struct CharDescriptor
{
	vec2 pos;
	vec2 size;
};

// Values that stay constant for the whole mesh.
layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform mat4 transformation;
layout(location = 2) uniform CharDescriptor charInfo;
layout(location = 4) uniform vec2 fontSize;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position = MVP * transformation * vec4(vertexPosition_modelspace,1);

	vec2 uvOffset = charInfo.pos;
	uvOffset += vertexUV * charInfo.size;
	uvOffset /= fontSize;

	UV = uvOffset;
}

