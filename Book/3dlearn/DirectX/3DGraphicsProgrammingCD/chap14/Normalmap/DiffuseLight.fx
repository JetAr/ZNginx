float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 WorldXf : World < string UIWidget="None"; >;
float4x4 WorldITXf : WorldInverseTranspose < string UIWidget="None"; >;

float3 Light0Pos : Position <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Position";
    string Space = "World";
> = {-0.5f,2.0f,1.25f};

float3 Light0Color : Diffuse <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Color";
    string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

texture NormalTexture  <
    string ResourceName = "default_bump_normal.dds";
    string UIName =  "Normal-Map Texture";
    string ResourceType = "2D";
>;

sampler2D NormalSampler = sampler_state 
{
    Texture = <NormalTexture>;
    FILTER = MIN_MAG_MIP_LINEAR;
	//DX9 old style
    //MINFilter = Linear;
	//MAGFilter = Linear;
	//MIPFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
	float3 Normal	: NORMAL;
    float3 Tangent	: TANGENT0;
    float3 Binormal	: BINORMAL0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 WorldPosition: TEXCOORD0;
	float3 WorldNormal	: TEXCOORD1;
	float3 WorldTangent : TEXCOORD2;
	float3 WorldBinormal: TEXCOORD3;
	float2 Texcoord		: TEXCOORD4;
};

VS_OUTPUT mainVS(VS_INPUT In)
{
    VS_OUTPUT Out;
	
    Out.Position = mul(float4(In.Position.xyz, 1.0f), WorldViewProj);
	
    Out.WorldPosition = mul( In.Position, WorldXf).xyz;
    Out.WorldNormal = mul(In.Normal, (float3x3)WorldITXf).xyz;
    Out.WorldTangent = mul(In.Tangent, (float3x3)WorldITXf).xyz;
    Out.WorldBinormal = mul(In.Binormal, (float3x3)WorldITXf).xyz;
	
	Out.Texcoord = In.Texcoord;
	
    return Out;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
	// construct tangent matrix
	float3x3 TangentSpace;
	TangentSpace[0] = normalize(In.WorldTangent);
	TangentSpace[1] = normalize(In.WorldBinormal);
	TangentSpace[2] = normalize(In.WorldNormal);
	// access normalmap, transform normal
	float3 vNormalmap = (tex2D(NormalSampler, In.Texcoord)-0.5f)*2.0f;
	float3 vSurfaceNormal = mul(vNormalmap, TangentSpace);
	// diffuse reflection
    float3 vDir = normalize(Light0Pos - In.WorldPosition);
	float3 vDiffuse = Light0Color * saturate(dot(vDir, vSurfaceNormal));
	// surface color
    float4 color;
    color.rgb = vDiffuse;
	color.a = 1.0f;
      
    return color;
}

#if DIRECT3D_VERSION >= 0xa00

RasterizerState DisableCulling
{
    CullMode = NONE;
};

// Direct3D10
technique10 Main
{
    pass p0
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, mainPS() ) );
                
        SetRasterizerState(DisableCulling);       
    }
}

#else

// Direct3D9
technique Main
{
    pass p0 
    {
		CullMode = None;
        VertexShader = compile vs_2_0 mainVS();
        PixelShader = compile ps_2_0 mainPS();
    }
}

#endif