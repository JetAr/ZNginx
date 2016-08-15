struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD0;
};

SamplerState LinearSampler 
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

bool bHighPrecision = true;

Texture2D heightmap_prev;
Texture2D heightmap_current;

float4x4 wvp_matrix;

//
// Pass through vertex shader
//
VS_OUTPUT VS_Passthrough(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = float4(In.Position, 1.0f);
	Out.Texcoord = In.Texcoord;

	return Out;
}

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord;

	return Out;
}

VS_OUTPUT VS_Transform(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	return Out;
}

// 把高度值分解成(r,g,b,a) 4個數值
float4 EncodeHeightmap(float fHeight)
{
	float h = fHeight;
	float positive = fHeight > 0 ? fHeight : 0;
	float negative = fHeight < 0 ? -fHeight : 0;

	float4 color = 0;

	// 簡單的做法
	color.r = positive;
	color.g = negative;
	
	if ( bHighPrecision )
	{
		// `要增加精確度時, 把小數點中比較低的位數放在 (b,a) 中 .`
		color.ba = frac(color.rg*256);
		// `(r,g) 儲存的是位數比較高的部份`
		color.rg -= color.ba/256.0f;
	}
	
	return color;
}

// 把高度值從(r,g,b,a)4維向量還原成單一數值
float DecodeHeightmap(float4 heightmap)
{
	float4 table;

	if ( bHighPrecision )
		table = float4(1.0f, -1.0f, 1.0f/256.0f, -1.0f/256.0f);
	else
		table = float4(1.0f, -1.0f, 0.0f, 0.0f);
	
	return dot(heightmap, table);
}

float DecodeHeightmap(Texture2D Heightmap, float2 texcoord)
{
	float4 heightmap = Heightmap.Sample(PointSampler, texcoord);
	return DecodeHeightmap(heightmap);
}

float4 vTextureSize;
float  fDamping;

//
// Pixel Shader
//
float4 PS_Simulate(VS_OUTPUT In) : SV_Target
{
	float3 offset[4] =
	{
		float3(-1.0f, 0.0f, 0.25f),
		float3( 1.0f, 0.0f, 0.25f),
		float3( 0.0f,-1.0f, 0.25f),
		float3( 0.0f, 1.0f, 0.25f),
	};	

	// 前一個畫面的彈簧長度
	float fHeightPrev = DecodeHeightmap(heightmap_prev, In.Texcoord);
	
	// 目前的彈簧長度, 它取的是附近像素的平均值, 而不是單一圖素的值.
	float fNeighCurrent = 0;
	for ( int i=0; i<4; i++ )
	{
		float2 texcoord = In.Texcoord + offset[i].xy * vTextureSize.xy;
		fNeighCurrent += (DecodeHeightmap(heightmap_current, texcoord) * offset[i].z);
	}	
	
	// 套用公式來預測彈簧下一步的位置.
	float fHeight = fNeighCurrent * 2.0f - fHeightPrev;
	
	// 減弱彈簧的擺動, 讓它慢慢的停下來.
	fHeight *= fDamping;
	float4 color = EncodeHeightmap(fHeight);
			
	return color;
}

float fNormalScale;

// 經由水面高度來計算Normal
float4 PS_Normal(VS_OUTPUT In) : SV_Target
{
	float2 offset[4] =
	{
		float2(-1.0f, 0.0f),
		float2( 1.0f, 0.0f),
		float2( 0.0f,-1.0f),
		float2( 0.0f, 1.0f),
	};	

	float fHeightL = DecodeHeightmap(heightmap_current, In.Texcoord + offset[0]*vTextureSize.xy);
	float fHeightR = DecodeHeightmap(heightmap_current, In.Texcoord + offset[1]*vTextureSize.xy);
	float fHeightT = DecodeHeightmap(heightmap_current, In.Texcoord + offset[2]*vTextureSize.xy);
	float fHeightB = DecodeHeightmap(heightmap_current, In.Texcoord + offset[3]*vTextureSize.xy);
	
	float3 n = float3(fHeightB - fHeightT, fHeightR - fHeightL, fNormalScale);
	float3 normal = ((n) + 1.0f) * 0.5f;
	
	return float4(normal.rgb, 1.0f);
}

float4 PS_Heightmap(VS_OUTPUT In) : SV_Target
{
	float height = DecodeHeightmap(heightmap_current, In.Texcoord);
	return (height + 1.0f) * 0.5f;
}

float fForce;

float4 PS_Impulse(VS_OUTPUT In) : SV_Target
{
	float4 color = EncodeHeightmap(fForce);
	return color;
}

Texture2D NormalmapTex;
Texture2D WaterTex;

float fTexcoordScale;

float4 PS_Water(VS_OUTPUT In) : SV_Target
{
	float4 normalmap = NormalmapTex.Sample(PointSampler, In.Texcoord);
	float3 normal = (normalmap.rgb - 0.5f) * 2.0f;
	
	float2 texcoord = In.Texcoord + normal.xy * fTexcoordScale;
	float4 color = WaterTex.Sample(PointSampler, texcoord);
	
	return color;
}

BlendState NoBlend
{
	BlendEnable[0] = FALSE;
};

BlendState AddBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
};

DepthStencilState ZDisable
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

DepthStencilState ZEnable
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

technique10 AddImpulse
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Transform() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Impulse() ) );
        
        SetBlendState( AddBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

technique10 WaterSimulation
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Simulate() ) );
        
        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
	}
}

technique10 ConvertNormal
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Normal() ) );
	}
}

technique10 Water
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Water() ) );
	}
}

technique10 DrawObject
{
	pass p0 
	{
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetDepthStencilState( ZEnable, 0 );
	}
}