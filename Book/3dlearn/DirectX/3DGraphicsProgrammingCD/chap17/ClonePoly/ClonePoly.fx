RasterizerState DisableCulling
{
	FillMode = WIREFRAME;
    CullMode = NONE;
};

float Offset
<
    string UIName =  "Offset";
    string UIWidget = "slider";
    float UIMin = 0.0;
    float UIMax = 10.0;
    float UIStep = 0.1;
> = 1.0;

float4x4 WorldViewProj : WorldViewProjection;

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal 	: NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;	
	float4 Color	: COLOR;
	float3 Normal	: TEXCOORD0;
	float3 ObjPos   : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position, 1.0), WorldViewProj);
	Out.Color = 1;
	Out.ObjPos = In.Position.xyz;
	Out.Normal = In.Normal;
	
	return Out;
}

[maxvertexcount(6)]
void GS( triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> stream )
{
	int i;
	// original
	for ( i=0; i<3; i++ )
	{
		stream.Append(input[i]);
	}
	stream.RestartStrip();

	// cloned
	for ( i=0; i<3; i++ )
	{
		VS_OUTPUT Out = (VS_OUTPUT)0;
		float3 position = input[i].ObjPos + input[i].Normal * Offset;
		
		Out.Position = mul(float4(position, 1.0), WorldViewProj);
		Out.Color = float4(1,0,0,1);
		stream.Append(Out);
	}	
	stream.RestartStrip();
}

float4 PS(VS_OUTPUT input) : COLOR 
{
	return input.Color;
}

technique10 CopyFace
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		
		SetRasterizerState(DisableCulling);
	}
}

