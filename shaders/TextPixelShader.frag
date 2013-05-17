#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec4 textColor;

void main(){



	// Output color = color of the texture at the specified UV
	vec4 color = texture2D( myTextureSampler, UV );

	if(color.r <= 0)
		discard;
	gl_FragColor = color ; // gl_FragColor

}
