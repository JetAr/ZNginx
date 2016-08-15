/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/

float4x4 WorldViewProj : WorldViewProjection;

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct PS_IN
{
    float4 Color : COLOR;
};

VS_OUT mainVS(VS_IN input)
{
    VS_OUT output;
    
    output.Position = mul(float4(input.pos.xyz, 1.0), WorldViewProj);
    output.Color = float4(input.normal.xyz, 1.0f);
    
    return output;
}

float4 mainPS(PS_IN input) : COLOR 
{
    return input.Color;
}

technique technique0 {
	pass p0 {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS();
		CULLMODE = NONE;
	}
}
