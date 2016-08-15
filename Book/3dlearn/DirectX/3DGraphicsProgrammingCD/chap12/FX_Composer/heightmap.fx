float4x4 WorldViewProj : WorldViewProjection;

texture HeightmapTexture
<
    string ResourceName = "default_color.dds";
    string UIName =  "Heightmap";
    string ResourceType = "2D";
>;

sampler2D HeightmapSampler = sampler_state {
    Texture = <HeightmapTexture>;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};  

texture DiffuseTexture
<
    string ResourceName = "default_color.dds";
    string UIName =  "Diffuse Texture";
    string ResourceType = "2D";
>;

sampler2D DiffuseSampler = sampler_state {
    Texture = <DiffuseTexture>;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
};  

float Height <
    string UIName =  "Height";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 2.0;
    float UIStep = 0.01;
> = 1.0;

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
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
    
	float4 texcoord = float4(In.Texcoord, 0.0f, 0.0f);
	float4 heightmap = tex2Dlod(HeightmapSampler, texcoord);
	float3 position = In.Position + In.Normal * heightmap.r * Height;
	
    Out.Position = mul(float4(position, 1.0), WorldViewProj);
    Out.Texcoord = In.Texcoord;
    
    return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    float4 color = tex2D(DiffuseSampler, In.Texcoord);
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
