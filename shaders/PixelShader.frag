#version 120

varying vec2 pos;

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform float scalarValue;
uniform vec2 mousePos;

void main()
{
	// Output color = color of the texture at the specified UV
	gl_FragColor = (texture2D( myTextureSampler, UV ));
}
