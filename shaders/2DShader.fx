
// Animation
uniform extern float2 gOffset;

// Transformations
uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldViewProj;
uniform extern float4x4 gWorldInverseTranspose;
uniform extern float4x4 gViewProj; 

// Textures
uniform extern texture gTex;
uniform extern int gSpriteWidth;
uniform extern int gSpriteHeight;
uniform extern int gCurrentFrame;

struct VSIn
{
	float3 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct VSOut
{
	float4 posH : POSITION0;
	float2 tex : TEXCOORD0;
	float3 interpPos : TEXCOORD1;
};


sampler TexS = sampler_state
{
	texture = <gTex>;
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;

	AddressU = mirror;
	AddressV = mirror;

	//MaxAnisotropy = 8;
};


VSOut SpriteVS(VSIn IN)
{
	VSOut OUT = (VSOut)0;

	OUT.posH = mul(float4(IN.pos,1.0f),gWorldViewProj);
	OUT.tex = IN.tex;

	return OUT;
}


VSOut SpriteAnimationVS(VSIn IN)
{
	VSOut OUT = (VSOut)0;

	OUT.posH = mul(float4(IN.pos,1.0f),gWorldViewProj);

	// the global UV coordinate of the current frame's upper-left corner on the texture map
    float cellU = (gCurrentFrame % gSpriteHeight)/ (float)gSpriteHeight;
   	float cellV = floor(gCurrentFrame / gSpriteWidth) / (float)gSpriteWidth;
    
   	// the local UV offset inside the current frame
    float cellDU = IN.tex.x / (float)gSpriteHeight;
    float cellDV = IN.tex.y / (float)gSpriteWidth;
   	 
    OUT.tex.x = cellU + cellDU;
  	OUT.tex.y = cellV + cellDV;

	return OUT;
}


float4 SpritePS(float2 tex : TEXCOORD0) : COLOR
{
	return tex2D(TexS,tex);
}


technique Sprite
{
	pass p1
	{
		VertexShader = compile vs_3_0 SpriteVS();
		PixelShader = compile ps_3_0 SpritePS();
	}
}

technique AnimatedSprite
{
	pass p1
	{
		VertexShader = compile vs_3_0 SpriteAnimationVS();
		PixelShader = compile ps_3_0 SpritePS();
	}
}

/*float4 SpritePS(float2 tex : TEXCOORD0, float3 interpPos : TEXCOORD1) : COLOR
{
	float4 texColor = tex2D(TexS,tex);
	texColor.r *= sin(texColor.r+interpPos.x*0.05f);
	texColor.g *= cos(texColor.g+interpPos.y*0.05f);
	texColor.b *= tan(texColor.b+interpPos.z*0.05f);
	texColor.r = 1.0f - texColor.r;
	texColor.g = 1.0f - texColor.g;
	texColor.b = 1.0f - texColor.b;
	
	return texColor;
}*/