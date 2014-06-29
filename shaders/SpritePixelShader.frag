#version 330

// Interpolated values from the vertex shaders
in vec2 UV;

out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;
uniform vec4 uniformColor;

void main()
{
	// Output color = color of the texture at the specified UV
	outColor = texture( textureSampler, UV ) * uniformColor;
	
	if(outColor.a <= 0.0)
		discard;
}
