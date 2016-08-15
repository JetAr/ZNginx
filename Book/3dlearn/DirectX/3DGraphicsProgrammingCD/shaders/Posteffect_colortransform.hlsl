// 把顏色向量(r,g,b,a)跟矩陣相乘

struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

sampler2D RGBSampler : register(s0);

// 轉換矩陣
// VS
uniform row_major float4x4 wvp_matrix : register(c0);
// PS
uniform row_major float4x4 color_matrix : register(c0);

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord;

	return Out;
}

//
// Pixel Shader
//
float4 PS(VS_OUTPUT In) : COLOR
{
	float4 rgba = tex2D(RGBSampler, In.Texcoord);
	float4 color = mul(rgba, color_matrix);
	
	return color;
}
