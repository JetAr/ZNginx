// 更新Shadowmap用的Shader

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 ViewPos : TEXCOORD0;
};

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
uniform row_major float4x4 wv_matrix;

// Vertex Shader

VS_OUTPUT VS_LinearZ(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.ViewPos = mul( float4(In.Position, 1.0f), wv_matrix);
	
	return Out;
}

// Pixel Shader

float4 PS_LinearZ(VS_OUTPUT In) : COLOR
{
	float4 color = abs(In.ViewPos.zzzz);
	return color;
}

technique LinearZ
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_LinearZ();
		PixelShader = compile ps_2_0 PS_LinearZ();
	}
}
