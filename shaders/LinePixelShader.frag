#version 330

in vec4 color;
out vec4 outColor;

void main()
{
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	outColor = color;
}
