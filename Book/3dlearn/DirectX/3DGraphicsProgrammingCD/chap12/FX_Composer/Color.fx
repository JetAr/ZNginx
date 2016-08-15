float4x4 WorldViewProj : WorldViewProjection;

uniform float4 Color
<
    string UIName =  "Color";
    string UIWidget = "Color";
> = float4(1.0f, 1.0f, 1.0f, 1.0f);

float4 mainVS(float3 pos : POSITION) : POSITION
{
    return mul(float4(pos.xyz, 1.0), WorldViewProj);
}

float4 mainPS() : COLOR 
{
    return Color;
}

technique technique0 
{
    pass p0 
    {
        VertexShader = compile vs_2_0 mainVS();
        PixelShader = compile ps_2_0 mainPS();
		
		CULLMODE = NONE;
    }
}
