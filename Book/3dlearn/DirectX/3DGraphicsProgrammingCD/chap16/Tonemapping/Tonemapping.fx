/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

float4x4 WorldViewProj : WorldViewProjection;

texture DiffuseTexture : DIFFUSE
<
    string ResourceName = "default_color.dds";
    string UIName =  "Diffuse Texture";
    string ResourceType = "2D";
>;

float fScale
<
    string UIName =  "Scale";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 1.0;
    float UIStep = 0.01;
> = 1.0f;

float fCoeff0
<
    string UIName =  "Coeff0";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 5.0;
    float UIStep = 0.01;
> = 1.5f;

float fCoeff1
<
    string UIName =  "Coeff1";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 5.0;
    float UIStep = 0.01;
> = 2.0f;

float fMultiplier
<
    string UIName =  "Multiplier";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 2.0;
    float UIStep = 0.01;
> = 1.0f;

float fBurnout
<
    string UIName =  "Burnout";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 2.0;
    float UIStep = 0.01;
> = 1.2f;

float fMiddleGray
<
    string UIName =  "MiddleGray";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 2.0;
    float UIStep = 0.01;
> = 0.5f;

sampler2D DiffuseSampler = sampler_state
{
   Texture = <DiffuseTexture>;
   FILTER = MIN_MAG_MIP_LINEAR;
   AddressU  = Wrap;
   AddressV  = Wrap;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT output;
	
	output.Position.xy = (In.Texcoord.xy - 0.5f) * float2(2.0f, -2.0f);
	output.Position.z = 0.0f;
	output.Position.w = 1.0f;

	output.Texcoord = In.Texcoord;
	
	return output;
}

float4 mainPS_0(VS_OUTPUT In) : COLOR 
{
	float4 HDRImage = tex2D(DiffuseSampler, In.Texcoord.xy);
	float4 tonemapped = HDRImage * fScale;
	return tonemapped;
}

float4 mainPS_1(VS_OUTPUT In) : COLOR 
{
	float4 HDRImage = tex2D(DiffuseSampler, In.Texcoord.xy);
	float4 tonemapped = 1 - fCoeff0 * exp(-HDRImage * fCoeff1);
	
	return tonemapped;
}

float4 mainPS_2(VS_OUTPUT In) : COLOR 
{
	float4 HDRImage = tex2D(DiffuseSampler, In.Texcoord.xy);
	float3 table = float3(0.33f, 0.33f, 0.33f);
	float fLuminance = dot(HDRImage.rgb, table.rgb);
	
	float4 vColor = HDRImage;
	
	vColor *= fMultiplier;
    vColor *= (1.0f + vColor/fBurnout);
    vColor /= (1.0f + vColor);
	
	return vColor;
}

technique tonemap_0
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_0();
		CULLMODE = NONE;
	}
}

technique tonemap_1
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_1();
		CULLMODE = NONE;
	}
}

technique tonemap_2
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_2();
		CULLMODE = NONE;
	}
}
