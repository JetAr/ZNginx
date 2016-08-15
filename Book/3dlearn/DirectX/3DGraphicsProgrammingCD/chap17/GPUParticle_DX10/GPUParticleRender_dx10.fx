// 頂點的資料輸入格式
struct sParticle
{
	float4 Position : POSITION;
	float4 Velocity : VELOCITY;
};

Texture2D DiffuseTex;

SamplerState DiffuseSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
	AddressU  = Clamp;
	AddressV  = Clamp;
};

float4x4 wvp_matrix;
float4x4 camera_matrix;

//
// Vertex Shader
//
sParticle VS(sParticle In)
{
	return In;
}

struct GS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
};

float2 texcoord_table[4] =
{
	float2(0, 1),
	float2(1, 1),
	float2(1, 0),
	float2(0, 0)
};

//
// Geometry Shader
//
[maxvertexcount(6)]
void GS_billboard(point sParticle input[1], inout TriangleStream<GS_OUTPUT> stream )
{
	float4 position = input[0].Position;
	float fSize = input[0].Position.w * 0.5f;
	float fTime = input[0].Velocity.w;
	float4 color = saturate(fTime * 2.0f);
	
	GS_OUTPUT Out[4];
	float4 v[4];
	
	// 利用鏡頭矩陣來建立 billboard
	v[0] = position - fSize * camera_matrix[0] - fSize * camera_matrix[1];
	v[1] = position + fSize * camera_matrix[0] - fSize * camera_matrix[1];
	v[2] = position + fSize * camera_matrix[0] + fSize * camera_matrix[1];
	v[3] = position - fSize * camera_matrix[0] + fSize * camera_matrix[1];
	
	for ( int i=0; i<4; i++ )
	{
		Out[i].Position = mul(float4(v[i].xyz, 1.0f), wvp_matrix);
		Out[i].Color = color;
		Out[i].Texcoord = texcoord_table[i];
	}
	
	stream.Append(Out[1]);
	stream.Append(Out[2]);
	stream.Append(Out[0]);
	stream.Append(Out[3]);
	stream.RestartStrip();
}

float4 PS(GS_OUTPUT In) : SV_Target
{
	return DiffuseTex.Sample(DiffuseSampler, In.Texcoord) * In.Color;
}

RasterizerState CullNone
{
    CullMode = NONE;
};

DepthStencilState DisableZWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = 0;
    DepthFunc = LESS_EQUAL;
};

BlendState AddBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = ONE;
    DestBlend = ONE;
};

technique10 RenderParticle
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS_billboard() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
		SetRasterizerState(CullNone);
		SetDepthStencilState(DisableZWrite, 0);
		SetBlendState(AddBlending, float4(0,0,0,0), 0xffffffff);
	}
}
