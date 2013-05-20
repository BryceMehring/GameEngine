#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform vec3 textColor;

void main(){



	// Output color = color of the texture at the specified UV
	vec4 color = texture2D( myTextureSampler, UV );

	gl_FragColor = color * vec4(textColor,1.0f) ; // gl_FragColor
	
	/*if(gl_FragColor.a <= 0)
		discard;*/

}
