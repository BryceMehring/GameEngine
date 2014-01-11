#version 330

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 color;

out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;

void main()
{
	// Output color = color of the texture at the specified UV
    outColor = texture2D( textureSampler, UV ) * color;
}
