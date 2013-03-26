#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
 
// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec3 textColor;

void main()
{
	float alpha = texture(myTextureSampler,UV).a;

	color = vec4(textColor,alpha);

	if(alpha < 0.3)
		discard;

	
}