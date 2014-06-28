#version 330
#extension GL_ARB_explicit_uniform_location: enable

// Interpolated values from the vertex shaders
in vec2 UV;

out vec4 outColor;

// Values that stay constant for the whole mesh.
layout(location = 5) uniform sampler2D textureSampler;
layout(location = 6) uniform vec4 uniformColor;

void main()
{
	// Output color = color of the texture at the specified UV
	float texColor = texture(textureSampler,UV).r;
		
	outColor = vec4(texColor) * uniformColor;
}
