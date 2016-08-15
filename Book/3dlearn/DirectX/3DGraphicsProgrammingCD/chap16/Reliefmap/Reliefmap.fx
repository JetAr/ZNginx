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
   FILTER = MIN_MAG_MIP_LINEAR;

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
   FILTER = MIN_MAG_MIP_LINEAR;
   MinFilter = Linear;
   MagFilter = Linear;
   MipFilter = Linear;   
   AddressU  = Wrap;
   AddressV  = Wrap;
};

float fHeightScale
<
    string UIName =  "Height";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 0.5;
    float UIStep = 0.01;
> = 0.1f;

int Iterations
<
    string UIName =  "Iterations";
    string UIWidget = "slider";
    int UIMin = 1;
    int UIMax = 50;
    int UIStep = 1;
> = 5;

int Iterations2
<
    string UIName =  "Iterations2";
    string UIWidget = "slider";
    int UIMin = 1;
    int UIMax = 50;
    int UIStep = 1;
> = 5;

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

float4 mainPS_reliefmap(VS_OUTPUT In) : COLOR 
{
	float3 camera_dir = normalize(In.CameraDir);
	float z = -camera_dir.z; z = max(0.5f, z);
	float  ray_length  = fHeightScale / z;
	float4 step_length = ray_length/Iterations;
	float4 prob = float4(In.Texcoord, 1.0f, 0.0f);
	float3 prob_step = camera_dir.xyz * step_length;
	prob_step.z /= fHeightScale;

	// linear search
	for ( int i=0; i<Iterations; i++ )
	{
		float height = tex2D(HeightmapSampler, prob.xy);
		float error = (prob.z - height);
		prob.xyz += error > 0 ? prob_step : 0.0f;
	}

	float4 color = tex2D(DiffuseSampler, prob.xy);
	return color;
}

float4 mainPS_reliefmap_improved(VS_OUTPUT In) : COLOR 
{
	float3 camera_dir = normalize(In.CameraDir);
	float  z = -camera_dir.z; z = max(0.5f, z);
	float  ray_length  = fHeightScale / z;
	float  step_length = ray_length/Iterations;
	float3 prob = float3(In.Texcoord.xy, 1.0);
	float3 prob_step = camera_dir * step_length;

	prob_step.z /= fHeightScale;
	
	// linear search
	for ( int i=0; i<Iterations; i++ )
	{
		float height = tex2D(HeightmapSampler, prob.xy);
		float error = (prob.z - height);
		prob.xyz += error > 0 ? prob_step : 0.0f;
	}

	// binary search
	prob_step *= 0.5f;
	prob -= prob_step;
	for (i=0; i<Iterations2; i++)
	{
		float height = tex2D(HeightmapSampler, prob.xy);
		float error = prob.z - height;
		prob_step *= 0.5f;
		prob.xyz += (error < 0) ? -prob_step : prob_step;
	}
	
	/*
	// binary search non-optimized version
	float3 p0 = prob - prob_step;
	float3 p1 = prob;

	for (i=0; i<Iterations2; i++)
	{
		prob.xyz = (p0+p1) * 0.5f;
		float height = tex2D(HeightmapSampler, prob.xy);
		float error = prob.z - height;
		if ( error < 0 )
		{
			p1 = prob; 
		}
		else
		{
			p0 = prob; 
		}
	}
	*/
	
	float4 color = tex2D(DiffuseSampler, prob.xy);
	
	return color;
}

float4 mainPS_test(VS_OUTPUT In) : COLOR 
{
	int Iter = 1000;
	float3 camera_dir = normalize(In.CameraDir);
	float z = -camera_dir.z; z = max(0.5f, z);
	float  ray_length  = fHeightScale / z;
	float4 step_length = ray_length/Iter;
	float4 prob = float4(In.Texcoord, 1.0f, 0.0f);
	float3 prob_step = camera_dir.xyz * step_length;
	prob_step.z /= fHeightScale;

	// linear search
	[unroll]for ( int i=0; i<Iter; i++ )
	{
		float height = tex2D(HeightmapSampler, prob.xy);
		float error = (prob.z - height);
		prob.xyz += error > 0 ? prob_step : 0.0f;
	}

	float4 color = tex2D(DiffuseSampler, prob.xy);
	return color;
}

#if DIRECT3D_VERSION >= 0xa00

RasterizerState DisableCulling
{
    CullMode = NONE;
};

technique10 Test
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetPixelShader( CompileShader( ps_4_0, mainPS_test() ) );
        SetRasterizerState(DisableCulling);       
	}
}

technique10 Reliefmap
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetPixelShader( CompileShader( ps_4_0, mainPS_reliefmap() ) );
        SetRasterizerState(DisableCulling);       
	}
}

technique10 ReliefmapImproved 
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetPixelShader( CompileShader( ps_4_0, mainPS_reliefmap_improved() ) );
        SetRasterizerState(DisableCulling);       
	}
}

#else

technique Reliefmap
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_reliefmap();
		CULLMODE = NONE;
	}
}

technique ReliefmapImproved
{
	pass p0 
	{
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS_reliefmap_improved();
		CULLMODE = NONE;
	}
}

#endif