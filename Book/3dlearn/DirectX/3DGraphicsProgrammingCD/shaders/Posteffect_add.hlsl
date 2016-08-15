// 貼圖相加

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

sampler2D RGBASampler0 : register(s0);
sampler2D RGBASampler1 : register(s1);

// VS 轉換矩陣
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
	float4 rgba0 = tex2D(RGBASampler0, In.Texcoord);
	float4 rgba1 = tex2D(RGBASampler1, In.Texcoord);
	float4 color = rgba0 + rgba1;
	
	return color;
}
