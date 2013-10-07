#version 120

// Interpolated values from the vertex shaders
varying vec2 interpUV;
varying vec3 interpColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{

	// Output color = color of the texture at the specified UV
	float texColor = texture2D(myTextureSampler,interpUV).r;
	
	if(texColor <= 0.2)
		discard;
	
	gl_FragColor = vec4(vec3(texColor) * interpColor,1.0f);
	
}
