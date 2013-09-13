#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;
varying vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{
	// Output color = color of the texture at the specified UV
	gl_FragColor = texture2D( myTextureSampler, UV ) * vec4(color,1.0f);
	
	if(gl_FragColor.a <= 0.0)
		discard;
}
