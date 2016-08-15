// 頂點的資料輸入格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
};

// Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float3 Normal	: TEXCOORD0;
	float4 ScreenPos : TEXCOORD1;
};

uniform float4x4 wvp_matrix;
uniform float4x4 wv_matrix;
uniform float4   object_color;

texture BackgroundImage;

sampler2D BackgroundSampler = sampler_state
{
	Texture = <BackgroundImage>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	Out.ScreenPos = Out.Position;
	Out.Normal = normalize(mul(In.Normal, (float3x3)wv_matrix));

	return Out;
}

float4 PS(VS_OUTPUT In) : COLOR
{
	float2 uv = (In.ScreenPos.xy / In.ScreenPos.w) * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	uv += In.Normal.xy * float2(-0.05f, 0.05f);
	return tex2D(BackgroundSampler, uv) * object_color;
}

technique Refraction
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
		CULLMODE = CW;
		ZEnable = TRUE;
	}
}
