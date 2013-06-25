#version 120

// Interpolated values from the vertex shaders
varying vec2 interpUV;
varying vec3 interpColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{

	// Output color = color of the texture at the specified UV
	gl_FragColor = texture2D(myTextureSampler,vec2(interpUV.x,interpUV.y)) * vec4(interpColor,1.0f) ; // gl_FragColor
	
	if(gl_FragColor.a <= 0.0)
		discard;
}
