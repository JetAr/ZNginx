#define MAX_INSTANCES 10

float4x4 wvp_matrices[MAX_INSTANCES];

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD;
	uint Instance   : SV_InstanceID;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color	: COLOR;
	float2 Texcoord : TEXCOORD;
};

float4 color_table[8] =
{
	float4(1,0,0,1),
	float4(0,1,0,1),
	float4(1,1,0,1),
	float4(0,0,1,1),

	float4(1,0,1,1),
	float4(0,1,1,1),
	float4(1,1,1,1),
	float4(0.5,0.5,0.5,1),
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position.xyz, 1), wvp_matrices[In.Instance] );
	Out.Color = color_table[In.Instance & 0x07];
	Out.Texcoord = In.Texcoord;
	
	return Out;
}

float4 PS(VS_OUTPUT In) : SV_Target
{
	return In.Color;
}

technique10 RenderInstance
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}
