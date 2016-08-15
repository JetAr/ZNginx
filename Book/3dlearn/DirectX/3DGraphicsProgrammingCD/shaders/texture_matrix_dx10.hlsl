// 設定頂點的資料格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord	: TEXCOORD;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Texcoord	: TEXCOORD0;
};

Texture2D InputTexture;
SamplerState InputTextureSampler;

// 轉換矩陣
uniform row_major float4x4 viewproj_matrix;
uniform row_major float4x4 texture_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( float4(In.Position, 1.0f), viewproj_matrix);
	// 讀取頂點的貼質座標
	Out.Texcoord = mul( float4(In.Texcoord, 0.0f, 1.0f), texture_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 texcoord = In.Texcoord / In.Texcoord.w;
	float4 color = InputTexture.Sample(InputTextureSampler, texcoord.xy);
	return color;
}
