#version 120

varying vec2 pos;

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform float scalarValue;
uniform vec2 mousePos;

void main(){

	// Output color = color of the texture at the specified UV
	//float value = 1.0f*cos(2200.5f*distance(pos,vec2(cos(scalarValue),sin(scalarValue))) - 20.5f*scalarValue) + 0.5f;
	gl_FragColor = (texture2D( myTextureSampler, UV ));
	//gl_FragColor.x = 0.5f*sin(5.5f*distance(pos,mousePos) - 10.5f*scalarValue);
	//gl_FragColor.y = 0.5f*sin(5.5f*distance(pos,mousePos) - 10.5f*scalarValue);
	//gl_FragColor.z = 
	
	//gl_FragColor.z = sqrt(sin(gl_FragColor.z + sqrt(scalarValue) / scalarValue));
	
}
