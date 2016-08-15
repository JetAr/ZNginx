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
	float2 Texcoord	: TEXCOORD0;
};

Texture2D InputTexture;
SamplerState InputTextureSampler;

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
uniform row_major float4x4 wv_matrix;
bool bEyeSpace;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	if ( bEyeSpace )
	{
		// 使用鏡頭座標系上的位置
		Out.Texcoord = mul( float4(In.Position, 1.0), wv_matrix);
	}
	else
	{
		// 直接使用物件的頂點座標
		Out.Texcoord = In.Position.xy;
	}
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 color = InputTexture.Sample(InputTextureSampler, In.Texcoord);
	return color;
}
