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

// PS
#define KernelSize 9

texture Image;
texture Image2;

sampler LinearSampler = sampler_state
{
	Texture = <Image>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler LinearSampler2 = sampler_state
{
	Texture = <Image2>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler PointSampler = sampler_state
{
	Texture = <Image>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler PointSampler2 = sampler_state
{
	Texture = <Image2>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

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
uniform float4 vTexOffset[KernelSize];
//
// Blur Pixel Shader
//
float4 PS_Blur(VS_OUTPUT In) : COLOR
{
	float4 color = 0;

	for ( int i=0; i<KernelSize; i++ )
	{
		color += tex2D(LinearSampler, In.Texcoord + vTexOffset[i].xy) * vTexOffset[i].w;
	}
	
	return color;
}

// 調整亮度參數
uniform float4 IntensityOffset;
uniform float4 IntensityScale;

//
// Brightness Pixel Shader
//
float4 PS_Brightness(VS_OUTPUT In) : COLOR
{
	float4 rgba = tex2D(LinearSampler, In.Texcoord);
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
float4 PS_tex2D(VS_OUTPUT In ) : COLOR
{
	return tex2D(LinearSampler, In.Texcoord);
}

float4 PS_tex2DNormalmap(VS_OUTPUT In ) : COLOR
{
	return tex2D(LinearSampler, In.Texcoord)*0.5f+0.5f;
}

float4 PS_tex_red(VS_OUTPUT In ) : COLOR
{
	return tex2D(LinearSampler, In.Texcoord).rrrr;
}

float4 PS_addtextures(VS_OUTPUT In ) : COLOR
{
	float4 image1 = tex2D(LinearSampler, In.Texcoord);
	float4 image2 = tex2D(LinearSampler2, In.Texcoord);
	float4 color = image1 + image2;
	
	return color;
}

texture FramebufferTexture;
sampler2D FramebufferSampler = sampler_state
{
	texture = <FramebufferTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture BlurbufferTexture;
sampler2D BlurbufferSampler = sampler_state
{
	texture = <BlurbufferTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture ZBufferTexture;
sampler2D ZBufferSampler = sampler_state
{
	texture = <ZBufferTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

// 控制景深變化參數
uniform float4 vDepthOfField;
// 把非線性的Z值轉換回到鏡頭的線性距離
uniform float4 vZInv;

// Pixel Shader
float4 PS_DoF(VS_OUTPUT In) : COLOR
{
	float4 framebuffer = tex2D(FramebufferSampler, In.Texcoord);
	float4 blurbuffer = tex2D(BlurbufferSampler, In.Texcoord);
	float4 zbuffer = tex2D(ZBufferSampler, In.Texcoord);
	float fLinearZ = zbuffer.r;

	float fNearBlur = saturate(vDepthOfField.x-fLinearZ)/vDepthOfField.y;
	float fFarBlur = saturate(fLinearZ - vDepthOfField.z)/vDepthOfField.w;
	float fBlur = max(fNearBlur, fFarBlur);
	
	float4 color = lerp(framebuffer, blurbuffer, fBlur);
		
	return color;
}

technique DrawImage
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_tex2D();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique DrawNormal
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_tex2DNormalmap();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique DrawImageRedChannel
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_tex_red();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique CombineImage
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_addtextures();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique AddImage
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_tex2D();

		AlphaBlendEnable = TRUE;
		AlphaTestEnable = FALSE;
		SrcBlend = ONE;
		DestBlend = ONE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique BlurImage
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Blur();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique Brightness
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Brightness();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique DoF
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_DoF();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}