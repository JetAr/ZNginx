float4x4 WorldViewProj : WorldViewProjection;

struct VS_INPUT
{
	float4 Position : POSITION;
	uint Index 		: SV_VertexID;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 Color 	: COLOR;
};

float4 color_table[4] = 
{
	float4(1,0,0,1),
	float4(0,1,0,1),
	float4(0,0,1,1),
	float4(1,1,0,1)
};

VS_OUTPUT mainVS(VS_INPUT In)
{
	VS_OUTPUT Output;
	
	Output.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
	Output.Color = color_table[In.Index & 0x3];
	
	return Output;
}

float4 mainPS(VS_OUTPUT In) : COLOR 
{
	return In.Color;
}

float4 mainPS_primitiveID(VS_OUTPUT In, uint index : SV_PrimitiveID) : COLOR 
{
	float4 color = color_table[index & 0x03];
	return color;
}

technique10 VertexID_Demo
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, mainPS() ) );
	}
}

technique10 PrimitiveID_Demo
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, mainVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, mainPS_primitiveID() ) );
	}
}
