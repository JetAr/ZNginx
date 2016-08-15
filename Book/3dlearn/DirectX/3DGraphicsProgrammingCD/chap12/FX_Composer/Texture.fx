float4x4 WorldViewProj : WorldViewProjection;

texture ColorTexture
<
    string ResourceName = "default_color.dds";
    string UIName =  "Diffuse Texture";
    string ResourceType = "2D";
>;

sampler2D ColorSampler = sampler_state {
    Texture = <ColorTexture>;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};  

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord	: TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
    
    Out.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
    Out.Texcoord = In.Texcoord;
    
    return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    float4 color = tex2D(ColorSampler, In.Texcoord);
    return color;
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
