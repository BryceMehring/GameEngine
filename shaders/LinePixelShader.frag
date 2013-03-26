#version 330 core

// Ouput data
out vec4 color;

uniform vec4 lineColor;

void main()
{
	color = lineColor;
}