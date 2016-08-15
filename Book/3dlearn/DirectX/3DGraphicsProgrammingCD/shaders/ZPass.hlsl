// 更新Shadowmap用的Shader

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 ScreenPos : TEXCOORD0;
};

// 轉換矩陣
uniform row_major float4x4 wvp_matrix : register(c0);

// Vertex Shader

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.ScreenPos = Out.Position;
	
	return Out;
}

// Pixel Shader

float4 PS(VS_OUTPUT In) : COLOR
{
	// 這個值跟ZBuffer相同
	float4 color = In.ScreenPos.z/In.ScreenPos.w;
	return color;
}
