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
	float texColor = texture2D(textureSampler,UV).r;
	
	if(texColor <= 0.2)
		discard;
		
	outColor = vec4(texColor) * uniformColor;
}
