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

texture Image;
sampler2D LinearSampler = sampler_state
{
	Texture = <Image>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler2D PointSampler = sampler_state
{
	Texture = <Image>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture Image2;
sampler2D LinearSampler2 = sampler_state
{
	Texture = <Image2>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler2D PointSampler2 = sampler_state
{
	Texture = <Image2>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture Image3;
sampler2D PointSampler3 = sampler_state
{
	Texture = <Image3>;
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

float4 vLuminanceTable;

float4 PS_Luminance(VS_OUTPUT In) : COLOR
{
	float4 color = tex2D(LinearSampler, In.Texcoord);
	float  luminance = dot(color.rgb, vLuminanceTable.rgb);
	return luminance;
}

float4 PS_LogLuminance(VS_OUTPUT In) : COLOR
{
	float4 color = tex2D(LinearSampler, In.Texcoord);
	float fluminance = dot(color.rgb, vLuminanceTable.rgb);
	float fLogLuminance = log(fluminance + 0.0001f);
	return fLogLuminance;
}

uniform float4 vTexOffset[16];

float4 PS_Average16Samples(VS_OUTPUT In) : COLOR
{
	float4 color = 0;
	
	for ( int i=0; i<16; i++ )
	{
		float2 texcoord = In.Texcoord.xy + vTexOffset[i].xy;
		color += tex2D(PointSampler, texcoord);
	}
	
	color *= 1.0f/16.0f;
	
	return color;
}

float4 PS_ExpLuminance(VS_OUTPUT In) : COLOR
{
	float fLogLuminance = tex2D(PointSampler, In.Texcoord).r;
	float fLuminance = exp(fLogLuminance);
	return fLuminance;
}

uniform float4 vAdaptiveSpeed;
float4 PS_AdaptiveLuminance(VS_OUTPUT In) : COLOR
{
	float fPrevLuminance = tex2D(PointSampler, In.Texcoord).r;
	float fCurrentLuminance = tex2D(PointSampler2, In.Texcoord).r;
	float fNewLuminance = lerp(fPrevLuminance, fCurrentLuminance, vAdaptiveSpeed.x);
	
	return fNewLuminance;
}

uniform float4 vMiddleGray;
uniform float4 vMultiplierClamp;

float4 PS_AutoExposure(VS_OUTPUT In) : COLOR
{
	float4 image = tex2D(PointSampler, In.Texcoord);
	float fAverageLuminance = tex2D(PointSampler2, float2(0.5f, 0.5f)).r;
	float4 color = image;

	float multiplier = pow(vMiddleGray.r / fAverageLuminance, 0.75);
	color.rgb *= multiplier;
	
	return color;
}

float4 PS_AutoExposureExp(VS_OUTPUT In) : COLOR
{
	float4 HDRImage = tex2D(PointSampler, In.Texcoord);
	float fAverageLuminance = tex2D(PointSampler2, float2(0.5f, 0.5f)).r;

	float multiplier = vMiddleGray.r / fAverageLuminance;
	float p = multiplier > 1 ? 0.5f : 0.85f;
	multiplier = pow(multiplier, p);
	
	HDRImage.rgb *= multiplier;
	
	float3 table = float3(0.3f, 0.3f, 0.3f);
	float fLuminance = dot(HDRImage.rgb, table.rgb);
	
	float fExposure = 2.0f;
	float fCoeff0 = 2.0f;
	
	float final = 1.0f - exp(-fLuminance * fExposure) * (fAverageLuminance * fCoeff0);
	HDRImage *= final;
	
	return HDRImage;
}

technique Luminance
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Luminance();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique LogLuminance
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_LogLuminance();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique Average16Samples
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_Average16Samples();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique ExpLuminance
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_ExpLuminance();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique AdaptiveLuminance
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_AdaptiveLuminance();

		AlphaBlendEnable = FALSE;
		AlphaTestEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}

technique AutoExposure
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS_Passthrough();
		PixelShader = compile ps_2_0 PS_AutoExposure();

		AlphaBlendEnable = FALSE;
		ZEnable = FALSE;
		CULLMODE = NONE;
	}
}
