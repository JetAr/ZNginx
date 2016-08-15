// 計算陰影效果

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 LightPos : TEXCOORD0;
	float3 Normal   : TEXCOORD1;
};

#define num_samples 49

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
uniform row_major float4x4 light_wvp_matrix;
uniform row_major float4x4 light_wv_matrix;
uniform float4 vTexOffset[num_samples];

texture ShadowmapTexture;
sampler2D ShadowmapSampler = sampler_state
{
	texture = <ShadowmapTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler2D ShadowmapPointSampler = sampler_state
{
	texture = <ShadowmapTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.LightPos = mul( float4(In.Position, 1.0f), light_wvp_matrix);
	Out.Normal =  mul( In.Normal.xyz, (float3x3) light_wv_matrix);
	
	return Out;
}

//
// Pixel Shader
// 使用ZBuffer動態貼圖來計算陰影
float4 PS_D24S8(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float3 normal = normalize(In.Normal);
	// 使用DepthBuffer為輸入貼圖時, 會自動做PS_R32F函式里相同的距離比較.
	float4 Lit = tex2Dproj(ShadowmapSampler, In.LightPos);
	float4 color = fSpotLightEffect * normal.z * Lit;
	
	return color;
}

//
// Pixel Shader
// 使用ZBuffer動態貼圖來計算陰影
float4 PS_D24S8_PCF(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float3 normal = normalize(In.Normal);
	float  fLit = 0.0f;
	
	for ( int i=0; i<num_samples; i++ )
	{
		// 使用DepthBuffer為輸入貼圖時, 會自動做PS_R32F函式里相同的距離比較.
		float4 texcoord = projected_pos;
		texcoord.xy += vTexOffset[i].xy;
		fLit += tex2Dproj(ShadowmapSampler, texcoord);
	}

	fLit /= (float)num_samples;
	
	float4 color = fSpotLightEffect * normal.z * fLit;
	
	return color;
}

//
// Pixel Shader
// 使用Float32動態貼圖來計算陰影
float4 PS_R32F(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.45f);
	float3 normal = normalize(In.Normal);
	float4 shadowmap = tex2D(ShadowmapSampler, projected_pos.xy);
	float  fLit = projected_pos.z < shadowmap.r;
	float4 color = normal.z * fLit;

	return color;
}

//
// Pixel Shader
// 使用Float32動態貼圖來計算陰影
float4 PS_R32F_PCF(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float3 normal = normalize(In.Normal);
	float  fLighting = fSpotLightEffect * normal.z;
	float  fLit = 0.0f;
	float  fIncrement = 1.0f/(float)num_samples;
	
	for ( int i=0; i<num_samples; i++ )
	{
		float4 shadowmap = tex2D(ShadowmapSampler, projected_pos.xy + vTexOffset[i].xy);
		fLit += (shadowmap.r < projected_pos.z) ? 0.0f : 1.0f;
	}

	float4 color = fLighting * fLit * fIncrement;
	
	return color;
}

uniform float4 vTextureSize;

float LightCoverage_4samples(sampler2D shadowmap, float4 projected_pos)
{
	float  fLit[4];
	float2 texoffset[4];
	int index = 0;

	for ( int y=0; y<=1; y++ )
	{
		for ( int x=0; x<=1; x++ )
		{
			texoffset[index] = float2(x,y) * vTextureSize.zw;
			index++;
		}
	}

	for ( int i=0; i<4; i++ )
	{
		float4 distance = tex2D(shadowmap, projected_pos.xy + texoffset[i].xy);
		fLit[i] = distance.r < projected_pos.z ? 0.0f : 1.0f;
	}

	float2 texel_frac = frac(projected_pos.xy * vTextureSize.xy);

	float fLitX0 = lerp(fLit[0], fLit[1], texel_frac.x);
	float fLitX1 = lerp(fLit[2], fLit[3], texel_frac.x);
	float fLitFinal = lerp(fLitX0, fLitX1, texel_frac.y);

	return fLitFinal;
}

float LightCoverage_16samples(sampler2D shadowmap, float4 projected_pos)
{
	float  fLit[16];
	float2 texoffset[16];
	
	int index = 0;
	for ( int y=-1; y<=2; y++ )
	{
		for ( int x=-1; x<=2; x++ )
		{
			texoffset[index] = float2(x,y) * vTextureSize.zw;
			index++;
		}
	}
	
	for ( int i=0; i<16; i++ )
	{
		float4 distance = tex2D(shadowmap, projected_pos.xy + texoffset[i]);
		fLit[i] = distance.r < projected_pos.z ? 0.0f : 1.0f;
	}

	float fDividor = 1.0f/9.0f;
	
	float p0 = 
		(fLit[0] + fLit[1] + fLit[ 2] +
		 fLit[4] + fLit[5] + fLit[ 6] +
		 fLit[8] + fLit[9] + fLit[10]) * fDividor;
	
	float p1 = 
		(fLit[1] + fLit[ 2] + fLit[ 3] +
		 fLit[5] + fLit[ 6] + fLit[ 7] +
		 fLit[9] + fLit[10] + fLit[11]) * fDividor;

	float p2 = 
		(fLit[ 4] + fLit[ 5] + fLit[ 6] +
		 fLit[ 8] + fLit[ 9] + fLit[10] +
		 fLit[12] + fLit[13] + fLit[14]) * fDividor;

	float p3 = 
		(fLit[ 5] + fLit[ 6] + fLit[ 7] +
		 fLit[ 9] + fLit[10] + fLit[11] +
		 fLit[13] + fLit[14] + fLit[15]) * fDividor;

	float2 texel = projected_pos.xy * vTextureSize.xy;
	float2 texel_frac = frac(texel);

	float fLitX0 = lerp(p0, p1, texel_frac.x);
	float fLitX1 = lerp(p2, p3, texel_frac.x);
	float fLitFinal = lerp(fLitX0, fLitX1, texel_frac.y);
	
	return fLitFinal;
}

//
// Pixel Shader
// 使用Float32動態貼圖來計算陰影
float4 PS_R32F_PCF_Lerp(VS_OUTPUT In) : COLOR
{
	float4 projected_pos = In.LightPos / In.LightPos.w;
	float  fToCenter = distance(projected_pos, float2(0.5f,0.5f));
	float  fSpotLightEffect = 1.0f - saturate(fToCenter/0.4f);
	float3 normal = normalize(In.Normal);
	float  fLighting = fSpotLightEffect * normal.z;

	float  fLightCoverage = LightCoverage_16samples(ShadowmapPointSampler, projected_pos);
	float4 color = fLightCoverage * fLighting;
	
	return color;
}

technique ShadowmapD24
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_D24S8();
	}
}

technique ShadowmapD24_PCF
{
	pass p0 
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_D24S8_PCF();
	}
}

technique ShadowmapD24_PCF_Lerp
{
	pass p0 
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_D24S8_PCF();
	}
}

technique ShadowmapR32F
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS_R32F();
	}
}

technique ShadowmapR32_PCF
{
	pass p0 
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_R32F_PCF();
	}
}

technique ShadowmapR32_PCF_Lerp
{
	pass p0 
	{
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS_R32F_PCF_Lerp();
	}
}

