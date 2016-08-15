// 頂點的資料輸入格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

// Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD0;
};

// VS
cbuffer MatrixConstant : register(b0)
{
	uniform row_major float4x4 wvp_matrix;
}

// PS
#define KernelSize 9

Texture2D Image;
SamplerState ImageSampler;

cbuffer TexOffset : register(b1)
{
	uniform float4 vTexOffset[KernelSize];
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
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 color = 0;

	for ( int i=0; i<KernelSize; i++ )
	{
		color += Image.Sample(ImageSampler, In.Texcoord + vTexOffset[i].xy) * vTexOffset[i].w;
	}
	
	return color;
}
