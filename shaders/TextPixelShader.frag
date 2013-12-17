#version 330

// Interpolated values from the vertex shaders
in vec2 interpUV;
in vec4 interpColor;

out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{

	// Output color = color of the texture at the specified UV
	float texColor = texture2D(myTextureSampler,interpUV).r;
	outColor = vec4(texColor) * interpColor;
}
