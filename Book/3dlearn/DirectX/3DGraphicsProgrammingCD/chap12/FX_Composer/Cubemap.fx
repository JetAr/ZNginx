float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 World : World < string UIWidget="None"; >;
float4x4 ViewInv : ViewInverse < string UIWidget="None"; >;

texture CubemapTexture
<
    //string ResourceName = "default_reflection.dds";
    string UIName =  "Cubemap";
    string ResourceType = "CUBE";
>;

samplerCUBE CubemapSampler = sampler_state 
{
    Texture = <CubemapTexture>;
    MinFilter = Linear;
    MipFilter = Linear;
    MagFilter = Linear;
};  

struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float3 Reflection : TEXCOORD0;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
    
    Out.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
    
    float4 WorldPosition = mul(float4(In.Position.xyz, 1.0), World);
    float3 WorldNormal = mul(In.Normal, (float3x3) World);
    float3 CameraPosition = ViewInv[3].xyz;
    float3 Ray = normalize(WorldPosition - CameraPosition);

    Out.Reflection = reflect(Ray, WorldNormal);
    
    return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
    float4 color = texCUBE(CubemapSampler, In.Reflection);    
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
