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
SamplerComparisonState ShadowmapCompareSampler : register(s0);
SamplerState ShadowmapSampler : register(s0);

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
