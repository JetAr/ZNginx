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

// VS
uniform row_major float4x4 wvp_matrix : register(c0);
uniform float4 texcoord_offset : register(c4);

// PS
sampler2D ImageSampler : register(s0);

#define KernelSize 9

uniform float4 vTexOffset[KernelSize] : register(c0); // c0-c8

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
	float4 color = 0;

	for ( int i=0; i<KernelSize; i++ )
	{
		color += tex2D(ImageSampler, In.Texcoord + vTexOffset[i].xy) * vTexOffset[i].w;
	}
	
	return color;
}
