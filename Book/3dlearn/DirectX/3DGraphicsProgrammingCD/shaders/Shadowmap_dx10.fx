#include "DefaultConstantBuffers.h"

// 設定頂點的資料格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 LightPos : TEXCOORD0;
	float3 Normal   : TEXCOORD1;
};

Texture2D Shadowmap;
SamplerComparisonState ShadowmapCompareSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	ComparisonFunc = LESS;
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState ShadowmapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

#define PCF_Samples 25

uniform float4 vTexOffset[PCF_Samples];

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.LightPos = mul( float4(In.Position, 1.0f), light_wvp_matrix);
	Out.Normal =  mul( In.Normal.xyz, (float3x3) light_wv_matrix);
	
	return Out;
}

//
// Pixel Shader
// 使用ZBuffer來計錄
float4 PS_SamplerCompare(VS_OUTPUT In) : SV_Target
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);

	float4 fLit = Shadowmap.SampleCmpLevelZero(ShadowmapCompareSampler, projected_pos.xy, projected_pos.z);
	float4 color = fSpotLightEffect * In.Normal.z * fLit;
	
	return fLit;

	return color;
}

//
// Pixel Shader
// 使用ZBuffer來計錄
float4 PS_SamplerCompare_PCF(VS_OUTPUT In) : SV_Target
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float4 fLit = 0;
	
	for ( int i=0; i<PCF_Samples; i++ )
	{
		fLit += Shadowmap.SampleCmpLevelZero(ShadowmapCompareSampler, projected_pos.xy, projected_pos.z);
	}
	
	fLit /= (float) PCF_Samples;
		
	float4 color = fSpotLightEffect * In.Normal.z * fLit;
	
	return fLit;

	return color;
}

//
// Pixel Shader
//
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);

	float4 shadowmap = Shadowmap.Sample(ShadowmapSampler, projected_pos.xy);
	float  fLit = projected_pos.z < shadowmap.r;
	float4 color = fSpotLightEffect * In.Normal.z * fLit;

	return color;
}

//
// Pixel Shader
//
float4 PS_PCF(VS_OUTPUT In) : SV_Target
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float  fIncrement = 1.0f/(float)PCF_Samples;
	float  fLit = 0;
	
	for ( int i=0; i<PCF_Samples; i++ )
	{
		float2 texcoord = projected_pos.xy + vTexOffset[i].xy;
		float4 shadowmap = Shadowmap.Sample(ShadowmapSampler, texcoord);
		fLit += projected_pos.z < shadowmap.r ? fIncrement : 0.0f;
	}
	
	float4 color = fSpotLightEffect * In.Normal.z * fLit;

	return color;
}

technique10 ShadowmapD32
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_SamplerCompare() ) );
	}
}

technique10 ShadowmapD32_PCF
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_SamplerCompare_PCF() ) );
	}
}

technique10 ShadowmapR32_PCF
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_PCF() ) );
	}
}

