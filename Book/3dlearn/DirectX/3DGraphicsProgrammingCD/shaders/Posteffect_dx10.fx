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

DepthStencilState ZEnable
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;    
};

DepthStencilState ZDisable
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

RasterizerState CullNone
{
	CullMode = NONE;
};

// PS
#define KernelSize 9

Texture2D Image;

float4 vertex_table[4];
float4 texcoord_table[4];

VS_OUTPUT VS_Icon(VS_INPUT In, uint index : SV_VertexID)
{
	VS_OUTPUT Out;

	Out.Position = vertex_table[index%4];
	Out.Texcoord = texcoord_table[index%4];
	
	return Out;
}

//
// Vertex Shader
//
VS_OUTPUT VS_Passthrough(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = float4(In.Position, 1.0f);
	Out.Texcoord = In.Texcoord;

	return Out;
}

// 貼圖座標位移參數
cbuffer blur_coeff
{
	uniform float4 vTexOffset[KernelSize];
}
//
// Blur Pixel Shader
//
float4 PS_Blur(VS_OUTPUT In) : SV_Target
{
	float4 color = 0;

	for ( int i=0; i<KernelSize; i++ )
	{
		color += Image.Sample(LinearSampler, In.Texcoord + vTexOffset[i].xy) * vTexOffset[i].w;
	}
	
	return color;
}

// 調整亮度參數
cbuffer Brightness_Coeff
{
	uniform float4 IntensityOffset;
	uniform float4 IntensityScale;
}
//
// Brightness Pixel Shader
//
float4 PS_Brightness(VS_OUTPUT In) : SV_Target
{
	float4 rgba = Image.Sample(LinearSampler, In.Texcoord);
	return rgba * 2;
	float3 table = float3(0.3f, 0.59f, 0.11f);
	// 先轉成灰階來察看它的亮度
	float4 old_intensity = dot(rgba.rgb, table);
	// 把亮度做個改變
	float4 new_intensity = (old_intensity + IntensityOffset) * IntensityScale;
	// 把原始的顏色乘上新的亮度
	float4 color = rgba * new_intensity;
	
	return color;
}

//
// Texture
//
float4 PS_tex2D(VS_OUTPUT In ) : SV_Target
{
	return Image.Sample(LinearSampler, In.Texcoord);
}

Texture2D FramebufferTexture;
Texture2D BlurredTexture;
Texture2D ZBufferTexture;

// 控制景深變化參數
uniform float4 vDepthOfField;
// 把非線性的Z值轉換回到鏡頭的線性距離
uniform float4 vZInv;

// Pixel Shader
float4 PS_DoF(VS_OUTPUT In) : SV_Target
{
	float4 framebuffer = FramebufferTexture.Sample(PointSampler, In.Texcoord);
	float4 blurbuffer = BlurredTexture.Sample(LinearSampler, In.Texcoord);
	float4 zbuffer = ZBufferTexture.Sample(PointSampler, In.Texcoord);
	float4 z = float4(0.0f, 0.0f, zbuffer.r, 1.0f);
	float fLinearZ = abs(1.0f/dot(z, vZInv));

	float fNearBlur = saturate(vDepthOfField.x-fLinearZ)/vDepthOfField.y;
	float fFarBlur = saturate(fLinearZ - vDepthOfField.z)/vDepthOfField.w;
	float fBlur = max(fNearBlur, fFarBlur);
	
	float4 color = lerp(framebuffer, blurbuffer, fBlur);
		
	return color;
}

technique10 DrawIcon
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Icon() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_tex2D() ) );

        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}

technique10 DrawImage
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_tex2D() ) );

        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}

technique10 AddImage
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_tex2D() ) );
        
        SetBlendState( AddBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}

technique10 BlurImage
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Blur() ) );

        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}

technique10 Brightness
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Brightness() ) );
        
        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}

technique10 DoF
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_DoF() ) );

        SetBlendState( NoBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ZDisable, 0 );
        SetRasterizerState( CullNone );
	}
}