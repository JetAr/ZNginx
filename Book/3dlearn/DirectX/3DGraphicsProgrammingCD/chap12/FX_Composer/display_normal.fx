/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/

float4x4 WorldViewProj : WorldViewProjection;

struct VS_OUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

VS_OUT mainVS(float3 pos : POSITION, float3 normal : NORMAL )
{
    VS_OUT output;
    
    output.Position = mul(float4(pos.xyz, 1.0), WorldViewProj);
    output.Color = float4(normal.xyz, 1.0f);
    
    return output;
}

float4 mainPS(float4 color : COLOR) : COLOR 
{
    return color;
}

technique technique0 {
	pass p0 {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS();
	}
}
