#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform float scalarValue;

void main(){

	// Output color = color of the texture at the specified UV
	gl_FragColor = (texture2D( myTextureSampler, UV ));
	//gl_FragColor.x = gl_FragColor.x * scalarValue;
	gl_FragColor.y = gl_FragColor.y * scalarValue;
	gl_FragColor.z = (1.0f - gl_FragColor.z) * scalarValue;
	
}
