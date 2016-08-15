// 計算陰影效果

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 WorldPos : COLOR0;
	float3 WorldNormal : COLOR1;
	float2 Texcoord : TEXCOORD;
};

float4x4 wvp_matrix;
float4x4 world_matrix;

texture diffuseTexture;
sampler2D diffuseSampler = sampler_state
{
	texture = <diffuseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	float2 texcoord = float2(In.Texcoord.x, 1-In.Texcoord.y);
	
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	Out.WorldPos = mul(float4(In.Position, 1.0f), world_matrix).xyz;
	Out.WorldNormal = mul(In.Normal, (float3x3) world_matrix).xyz;
	Out.Texcoord = In.Texcoord;
	
	return Out;
}

struct PS_OUTPUT
{
	float4 WorldPos : COLOR0;
	float4 WorldNormal : COLOR1;
	float4 Diffuse : COLOR2;
};

PS_OUTPUT PS(VS_OUTPUT In)
{
	PS_OUTPUT Out;
	
	Out.WorldPos = float4(In.WorldPos, 1.0f);
	Out.WorldNormal = float4(In.WorldNormal, 1.0f);
	Out.Diffuse = tex2D(diffuseSampler, In.Texcoord);
		
	return Out;
}

technique DeferredLighting
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}


