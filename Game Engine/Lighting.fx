
/*

% Description of my shader.
% Second line of description for my shader.

date: YYMMDD

*/

#define LIGHT_MAX 8

struct Material
{

	float4   diffuse;        /* Diffuse color RGBA */
    float4   ambient;        /* Ambient color RGB */
    float4   spec;          /* Specular 'shininess' */
    float4   emissive;      /* Emissive color RGB */
    float    power;         /* Sharpness if specular highlight */
};

struct PhongOutputVS
{
	float4 posH : POSITION0;
	float3 normalW : TEXCOORD0;
	float3 posW : TEXCOORD1;
};
struct ToonOutputVS
{
	float4 posH : POSITION0;
	float d : TEXCOORD0;
	float a : TEXCOORD1;
};

// Transformations
uniform extern float4x4 gWorld;
uniform extern float4x4 gWorldViewProj;
uniform extern float4x4 gWorldInverseTranspose;

// Lighting
uniform extern Material gObjectMtrl;
uniform extern Material gDirLightMtrl;
uniform extern Material gPointLightMtrl[LIGHT_MAX];
uniform extern float3   gPointLightPos[LIGHT_MAX];
uniform extern float3   gLightDir; // Directional light
uniform extern float3   gCamPos; // needed for spec calculation
uniform extern float	gSpecPower;
uniform extern int		gLightNum; // Number of point lights
uniform extern int      gLightMode;

static const float gToonColor[6] = {0.0, 0.2, .4f, 0.6,.8,1.0};

PhongOutputVS PhongVS(float3 pos : POSITION0, float3 normalI : NORMAL0)
{
	PhongOutputVS output = (PhongOutputVS)0;

	// Transform normal to world space
	output.normalW = mul(float4(normalI,0.0f),gWorldInverseTranspose).xyz;

	// Transform vertex to World Space
	output.posW = mul(float4(pos,1.0f),gWorld).xyz;

	// Transform vertex to Homogeneous Clip space
	output.posH = mul(float4(pos,1.0f),gWorldViewProj);
	
	return output;
}

float4 PhongPS(float3 normalW : TEXCOORD0, float3 posW : TEXCOORD1) : COLOR
{
	float3 ToEye = normalize(gCamPos - posW);
	float4 color = float4(0.0f,0.0f,0.0f,0.0f);

	normalW = normalize(normalW);

	//******Point Lights***************

	// Loop through all the point lights
	for(int i = 0; i < gLightNum; ++i)
	{
		// Point Light Vector
		float3 LightVec = normalize(gPointLightPos[i] - posW);

		///// Create the Color //////

		// Spec lighting, if any
		float3 r = normalize(reflect(-LightVec,normalW));
		float s = pow(max(dot(r,ToEye),0.0f),gSpecPower);

		// Diffuse
		float d = max(dot(LightVec,normalW),0.0f);

		// compute the color
		float3 diffuse = d*(gPointLightMtrl[i].diffuse*gObjectMtrl.diffuse).rgb;
		float3 spec    = s*(gPointLightMtrl[i].spec*gObjectMtrl.spec).rgb;
		float3 ambient = (gPointLightMtrl[i].ambient*gObjectMtrl.ambient).rgb;

		//Attenuation
		float Dist = distance(gPointLightPos[i],posW);
		float a = Dist*Dist;

		// Sum up all of the lights
		if(gLightMode == 0)
		{
			color += float4(ambient,gObjectMtrl.diffuse.a);
		}
		else if(gLightMode == 1)
		{
			color += float4(((diffuse) / ( 0.1*Dist + 0.1*a)),gObjectMtrl.diffuse.a);
		}
		// Sum up all of the lights
		else if(gLightMode == 2)
		{
			color += float4(((spec) / ( 0.1*Dist + 0.1*a)),gObjectMtrl.diffuse.a);
		}
		else if(gLightMode == 3)
		{
			color += float4(ambient + ((diffuse + spec) / ( 0.1*Dist + 0.1*a)),gObjectMtrl.diffuse.a);
		}
	}

	return color;
}

ToonOutputVS ToonVS(uniform int i, float3 pos : POSITION0, float3 normalI : NORMAL0)
{
	ToonOutputVS output = (ToonOutputVS)0;
	
	// Transform normal to world space
	float3 normalW = mul(float4(normalI,0.0f),gWorldInverseTranspose).xyz;
	normalW = normalize(normalW);

	// Transform vertex to World Space
	float3 posW = mul(float4(pos,1.0f),gWorld).xyz;

	// Point Light Vector
	float3 LightVec = normalize(gPointLightPos[i] - posW);

	// Compute toon lighting

	// diffuse intensity 
	output.d = max(dot(normalize(LightVec),normalW),0.0f);

	// attenuation
	float Dist = distance(gPointLightPos[i],posW);
	float a = Dist*Dist;

	output.a = 0.1*Dist + 0.1*a;

	// Transform vertex to Homogeneous Clip space
	output.posH = mul(float4(pos,1.0f),gWorldViewProj);
	
	return output;
}

float4 ToonPS(uniform int i, float d : TEXCOORD0, float a : TEXCOORD1) : COLOR
{
	int iIndex = 0;

	if( d >= 0.0f && d <= 0.1666667f)
	{
		iIndex = 0;
	}
	else if( d >= 0.1666667f && d <= 0.33333f)
	{
		iIndex = 1;
	}
	else if( d >= 0.333333f && d <= 0.5f)
	{
		iIndex = 2;
	}
	else if( d >= 0.5f && d <= 0.6666666667f)
	{
		iIndex = 3;
	}
	else if( d >= 0.6666666667f && d <= .8333333f)
	{
		iIndex = 4;
	}
	else if( d >= .8333333f && d <= 1.0f)
	{
		iIndex = 5;
	}

	float3 diffuse = gToonColor[iIndex]*(gObjectMtrl.diffuse*gPointLightMtrl[i].diffuse);
	float3 ambient = gObjectMtrl.ambient*gPointLightMtrl[i].ambient;

	return float4(diffuse / a + ambient,gObjectMtrl.diffuse.a);
}

technique PhongLighting
{
	pass p0
	{
		VertexShader = compile vs_3_0 PhongVS();
		PixelShader = compile ps_3_0 PhongPS();
	}
}

technique ToonLighting
{
	pass p0
	{
		VertexShader = compile vs_2_0 ToonVS(0);
		PixelShader = compile ps_2_0 ToonPS(0);
	}
	pass p1
	{
		VertexShader = compile vs_2_0 ToonVS(1);
		PixelShader = compile ps_2_0 ToonPS(1);

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;
		BlendOp = add;
	}
}


