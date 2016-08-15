// 頂點的資料輸入格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
};

// Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Normal	: TEXCOORD0;
	float4 ScreenPos : TEXCOORD1;
};

uniform float4x4 wvp_matrix;
uniform float4x4 wv_matrix;
uniform float4   object_color;

SamplerState LinearSampler 
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D BackgroundImage;

DepthStencilState ZEnable
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;    
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

float4 PS(VS_OUTPUT In) : SV_Target
{
	float2 uv = (In.ScreenPos.xy / In.ScreenPos.w) * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	uv += In.Normal.xy * float2(-0.05f, 0.05f);
	return BackgroundImage.Sample(LinearSampler, uv) * object_color;
}

technique10 Refraction
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

        SetDepthStencilState( ZEnable, 0 );
	}
}
