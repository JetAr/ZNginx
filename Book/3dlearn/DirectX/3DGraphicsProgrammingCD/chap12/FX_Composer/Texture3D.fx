float4x4 WorldViewProj : WorldViewProjection;

texture ColorTexture
<
    string UIName =  "3D Texture";
    string ResourceType = "3D";
>;

sampler3D ColorSampler = sampler_state {
    Texture = <ColorTexture>;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};  

float Timer : TIME < string UIWidget = "None"; >;

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 Texcoord	: TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
    
    Out.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
    Out.Texcoord = float3(In.Texcoord, Timer);

    return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    float4 color = tex3D(ColorSampler, In.Texcoord);
    return color;
}

technique technique0 
{
    pass p0 
    {
        VertexShader = compile vs_3_0 mainVS();
        PixelShader = compile ps_3_0 mainPS();
		CULLMODE = NONE;
    }
}
