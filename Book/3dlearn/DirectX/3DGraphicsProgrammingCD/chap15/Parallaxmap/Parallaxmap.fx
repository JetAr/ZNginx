/*
	Parallax map
	
	Peter Pon 
*/

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
	float3 Normal 	: NORMAL;
	float3 Tangent 	: TANGENT;
	float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
	float3 CameraDir : TEXCOORD1;
};

texture DiffuseTexture : DIFFUSE
<
    string ResourceName = "default_color.dds";
    string UIName =  "Diffuse Texture";
    string ResourceType = "2D";
>;

sampler DiffuseSampler = sampler_state
{
   Texture = <DiffuseTexture>;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
   AddressU  = Wrap;
   AddressV  = Wrap;
};

texture HeightmapTexture
<
    string UIName =  "Heightmap";
    string ResourceType = "2D";
>;

sampler HeightmapSampler = sampler_state
{
   Texture = <HeightmapTexture>;
   MinFilter = Point;
   MagFilter = Point;
   MipFilter = None;   
   AddressU  = Wrap;
   AddressV  = Wrap;
};

float fHeightScale
<
    string UIName =  "Height";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 1.0;
    float UIStep = 0.01;
> = 0.1f;

int Iterations
<
    string UIName =  "Iterations";
    string UIWidget = "slider";
    int UIMin = 1.0;
    int UIMax = 20.0;
    int UIStep = 1;
> = 1;

float4x4 WorldViewProj : WorldViewProjection;
float4x4 World : World;
float4x4 ViewInv : ViewInv;

VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT output;
	
	float3x3 TangentSpace;
	
	TangentSpace[0] = mul(In.Tangent, World);
	TangentSpace[1] = mul(In.Binormal, World);
	TangentSpace[2] = mul(In.Normal, World);
	
	float4 world_pos = mul(float4(In.Position.xyz, 1.0), World);
	float4 camera_dir = normalize(world_pos - ViewInv[3]);
	
	output.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
	output.Texcoord = In.Texcoord;
	output.CameraDir = mul(TangentSpace, camera_dir.xyz);
	
	return output;
}

float4 mainPS_parallaxmap(VS_OUTPUT In) : COLOR 
{
	float3 camera_dir = normalize(In.CameraDir);
	float  height = (1.0f - tex2D(HeightmapSampler, In.Texcoord)) * fHeightScale;
	float2 tex_corrected = In.Texcoord + camera_dir.xy * height;
	float4 color = tex2D(DiffuseSampler, tex_corrected);
	
	return color;
}

float4 mainPS_parallaxmap_improved(VS_OUTPUT In) : COLOR 
{
	float3 camera_dir = normalize(In.CameraDir);
	float2 texcoord = In.Texcoord;
	float4 height = 1;
	float4 step = fHeightScale;
	
	for ( int i=0; i<Iterations; i++ )
	{
		float height_nextstep = tex2D(HeightmapSampler, texcoord);
		float height_diff = height - height_nextstep;
		texcoord += camera_dir.xy * height_diff * step;
		height = height_nextstep;
		step *= 0.5;
	}

	float4 color = tex2D(DiffuseSampler, texcoord);
	
	return color;
}

technique parallaxmap
{
	pass p0 
	{
		VertexShader = compile vs_2_0 mainVS();
		PixelShader = compile ps_2_0 mainPS_parallaxmap();
		CULLMODE = NONE;
	}
}

technique parallaxmap_improved
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_parallaxmap_improved();
		CULLMODE = NONE;
	}
}
