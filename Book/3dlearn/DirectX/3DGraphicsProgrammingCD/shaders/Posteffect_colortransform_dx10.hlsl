// 頂點的資料輸入格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

// Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

sampler2D RGBSampler : register(s0);

// 轉換矩陣
cbuffer VertexTransform
{
	uniform row_major float4x4 wvp_matrix;
	uniform row_major float4x4 color_matrix;
}

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
