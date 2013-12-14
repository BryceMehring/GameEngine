#version 330

// Interpolated values from the vertex shaders
in vec2 interpUV;
in vec3 interpColor;

out vec3 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{

	// Output color = color of the texture at the specified UV
	float texColor = texture2D(myTextureSampler,interpUV).r;
	
	if(texColor <= 0.2)
		discard;
	
        outColor = vec3(texColor) * interpColor;
	
}
