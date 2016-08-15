// 設定頂點的資料格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Texcoord : TEXCOORD0;
};

TextureCube InputTexture;
SamplerState InputTextureSampler;

// 轉換矩陣
uniform row_major float4x4 viewproj_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	// 座標轉換
	Out.Position = mul( float4(In.Position.xyz, 1), viewproj_matrix);
	// 把Normal直接當成貼圖座標
	Out.Texcoord = In.Normal;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 color = InputTexture.Sample(InputTextureSampler, In.Texcoord);
	return color;
}
