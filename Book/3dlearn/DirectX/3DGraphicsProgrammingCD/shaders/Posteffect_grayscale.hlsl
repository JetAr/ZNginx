// 彩色轉黑白

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
uniform row_major float4x4 wvp_matrix : register(c0);

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
	float3 rgb = tex2D(RGBSampler, In.Texcoord);
	float3 table = float3(0.3f, 0.59f, 0.11f);
	float4 color = dot(rgb, table);
	
	return color;
}
