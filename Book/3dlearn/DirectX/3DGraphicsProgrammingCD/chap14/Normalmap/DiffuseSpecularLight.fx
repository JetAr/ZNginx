float4x4 WorldViewProj : WorldViewProjection < string UIWidget="None"; >;
float4x4 WorldXf : World < string UIWidget="None"; >;
float4x4 ViewIXf : ViewInverse < string UIWidget="None"; >;
float4x4 WorldITXf : WorldInverseTranspose < string UIWidget="None"; >;

float3 Light0Pos : Position <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Position";
    string Space = "World";
> = {-0.5f,2.0f,1.25f};

float3 Light0Diffuse : Diffuse <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Diffuse";
    string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

float3 Light0Specular : Specular <
    string Object = "PointLight0";
    string UIName =  "PointLight0 Specular";
    string UIWidget = "Color";
> = {1.0f, 1.0f, 1.0f};

float3 AmbientLight : Ambient <
    string Object = "AmbientLight0";
    string UIName =  "Ambient Light";
    string UIWidget = "Color";
> = {0.07f, 0.07f, 0.07f};

float SpecExpon : SpecularPower <
    string UIName =  "Specular Power";
    string UIWidget = "slider";
    float UIMin = 1.0;
    float UIMax = 128.0;
    float UIStep = 1.0;
> = 55.0;

float ConstantAttenuation : ConstantAttenuation <
    string Object = "PointLight0";
    string UIWidget = "Color";
    string UIName =  "Light0 Constant Attenuation";
> = 1.0f;

float LinearAttenuation : LinearAttenuation <
    string Object = "PointLight0";
    string UIWidget = "Color";
    string UIName =  "Light0 Linear Attenuation";
> = 1.0f;

float QuadraticAttenuation : QuadraticAttenuation <
    string Object = "PointLight0";
    string UIWidget = "Color";
    string UIName =  "Light0 Quadratic Attenuation";
> = 1.0f;

texture DiffuseTexture  <
    string ResourceName = "default_color.dds";
    string UIName =  "Diffuse Texture";
    string ResourceType = "2D";
>;

sampler2D DiffuseSampler = sampler_state 
{
    Texture = <DiffuseTexture>;
    FILTER = MIN_MAG_MIP_LINEAR;
    //MINFilter = Linear;
	//MAGFilter = Linear;
	//MIPFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

texture NormalTexture  <
    string ResourceName = "default_bump_normal.dds";
    string UIName =  "Normal-Map Texture";
    string ResourceType = "2D";
>;

sampler2D NormalSampler = sampler_state 
{
    Texture = <NormalTexture>;
    FILTER = MIN_MAG_MIP_LINEAR;
    //Minfilter = Linear;
	//MAGFilter = Linear;
	//MIPFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
}; 

struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: NORMAL;
    float3 Tangent	: TANGENT;
    float3 Binormal	: BINORMAL;
	float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
  	float3 WorldPosition : TEXCOORD1;
  	float3 WorldNormal : TEXCOORD2;
	float3 WorldTangent : TEXCOORD3;
	float3 WorldBinormal : TEXCOORD4;
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
    float3 vWorldPosition = In.WorldPosition;
	// reconstruct tangentspace matrix   
	float3x3 TangentSpace;
	TangentSpace[0] = normalize(In.WorldTangent);
	TangentSpace[1] = normalize(In.WorldBinormal);
	TangentSpace[2] = normalize(In.WorldNormal);
	// decode normap
	float3 vNormalmap = tex2D(NormalSampler, In.Texcoord).rgb*2.0f - 1.0f;
	// get normal in world space
	float3 vWorldNormal = mul(vNormalmap, TangentSpace);
	// light distance & direction	
    float3 vDiff = Light0Pos - vWorldPosition;
    float  fDistance = length(vDiff);
    float3 vDir = vDiff / fDistance;
	// diffuse lighting
	float4 vDiffuse = tex2D(DiffuseSampler, In.Texcoord);
    vDiffuse.rgb *= Light0Diffuse * saturate(dot(vDir, vWorldNormal));
	// specular lighting
    float3 vCameraPosition = ViewIXf[3].xyz;
    float3 vCameraDir = normalize(vCameraPosition-vWorldPosition);
    float3 vHalfDir = normalize(vDir + vCameraDir);
    float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
    float  fSpecular = pow(fSpecularCosine, SpecExpon);
    float3 vSpecular = Light0Specular * fSpecular;
	// attenuation
	float3 Attenuation = float3(ConstantAttenuation, LinearAttenuation, QuadraticAttenuation);
    float fLightAttenuation = dot(float3(1.0f, fDistance, fDistance * fDistance), Attenuation);
	// surface color
    float4 Color;
    Color.rgb = AmbientLight + (vDiffuse + vSpecular) / fLightAttenuation;
    Color.a = vDiffuse.a;
    
    return Color;
}

#if DIRECT3D_VERSION >= 0xa00

///// TECHNIQUES /////////////////////////////
RasterizerState DisableCulling
{
    CullMode = NONE;
};

DepthStencilState DepthEnabling
{
	DepthEnable = TRUE;
};

BlendState DisableBlend
{
	BlendEnable[0] = FALSE;
};

technique10 Main 
{
    pass p0
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, mainPS() ) );
                
        SetRasterizerState(DisableCulling);       
		SetDepthStencilState(DepthEnabling, 0);
		SetBlendState(DisableBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
    }
}


#else

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