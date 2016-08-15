struct VS_INPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

float4x4 WorldViewProj : WorldViewProjection;

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position.xyz, 1.0), WorldViewProj);
	Out.Texcoord = In.Texcoord * float2(512,512);
	
	return Out;
}

float rand0(float fx)
{
	uint x = floor(fx);
	uint a = 1664525;
	uint c = 1013904223;
	uint mask = 0xffffffff;
	uint nx = (a*x + c) & 0xffffffff;
	float r = (float)nx / (float)mask;
	
	return r;
}

float rand1(float fx)
{
	uint x = floor(fx);
	uint a = 22695477;
	uint c = 5;
	uint mask = 0xffffffff;
	uint nx = (a*x + c) & 0xffffffff;
	float r = (float)nx / (float)mask;
	
	return rand0(r*512);
}

float4 PS(VS_OUTPUT In) : COLOR 
{
	//return In.Texcoord.xyxy;
	float rx = rand1(In.Texcoord.x + In.Texcoord.y);
	return rx;
}

technique10 Noise
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}
