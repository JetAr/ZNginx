// 改變亮度

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

sampler2D RGBASampler : register(s0);

// VS 轉換矩陣
uniform row_major float4x4 wvp_matrix : register(c0);
uniform float4 texcoord_offset : register(c4);

// PS 調整亮度參數
uniform float4 IntensityOffset : register(c0);
uniform float4 IntensityScale  : register(c1);

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord + texcoord_offset.xy;

	return Out;
}

//
// Pixel Shader
//
float4 PS(VS_OUTPUT In) : COLOR
{
	float4 rgba = tex2D(RGBASampler, In.Texcoord);
	float3 table = float3(0.3f, 0.59f, 0.11f);
	// 先轉成灰階來察看它的亮度
	float4 old_intensity = dot(rgba.rgb, table);
	// 把亮度做個改變
	float4 new_intensity = (old_intensity + IntensityOffset) * IntensityScale;
	// 把原始的顏色乘上新的亮度
	float4 color = rgba * new_intensity;
	
	return color;
}
