#version 330

in vec3 fragmentColor;
out vec3 outColor;

void main()
{
	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
        outColor = fragmentColor;
}
