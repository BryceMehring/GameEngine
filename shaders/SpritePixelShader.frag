#version 330

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 color;

out vec4 outColor;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{
	// Output color = color of the texture at the specified UV
        outColor = texture2D( myTextureSampler, UV ) * vec4(color,1.0f);
	
        if(outColor.x <= 0.0)
		discard;
}
