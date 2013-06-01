#version 120

varying vec4 fragmentColor;

void main()
{
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	gl_FragColor = fragmentColor;
}
