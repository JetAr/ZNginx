// 設定頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
// 顏色
uniform float4 color;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Position = mul( float4(In.Position.xyz, 1), wvp_matrix);
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	return color;
}

BlendState ShadowVolumeBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = ZERO;
	DestBlend = SRC_COLOR;
};

RasterizerState DisableCulling
{
    CullMode = NONE;
};

RasterizerState Culling
{
	FrontCounterClockwise = TRUE;
    CullMode = NONE;
};

DepthStencilState ShadowDS
{
	StencilEnable = TRUE;
	DepthEnable = FALSE;
	FrontFaceStencilFunc = Not_Equal;
	BackFaceStencilFunc = Not_Equal;
};

technique10 OutputColor
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
		SetRasterizerState(DisableCulling);
	}
}

technique10 Shadow
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
     
		SetRasterizerState(Culling);
		SetBlendState(ShadowVolumeBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		SetDepthStencilState(ShadowDS, 0);
	}
}
