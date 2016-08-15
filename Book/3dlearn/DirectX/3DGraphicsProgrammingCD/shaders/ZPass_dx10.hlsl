// 更新Shadowmap的Shader

#include "DefaultConstantBuffers.h"

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 ScreenPos : TEXCOORD0;
};

//
// Vertex Shader
//

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.ScreenPos = Out.Position;
	
	return Out;
}

//
// Pixel Shader
//

float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 color = In.ScreenPos.z/In.ScreenPos.w;
	return color;
}
