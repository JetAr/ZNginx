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

SamplerState PointSamplerRepeat
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

Texture2D Image0;
Texture2D Image1;
Texture2D Image2;

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

float4 PS_Luminance(VS_OUTPUT In) : SV_Target
{
	float4 color = Image0.Sample(LinearSampler, In.Texcoord);
	float  luminance = dot(color.rgb, vLuminanceTable.rgb);
	return luminance;
}

float4 PS_LogLuminance(VS_OUTPUT In) : SV_Target
{
	float4 color = Image0.Sample(LinearSampler, In.Texcoord);
	float fluminance = dot(color.rgb, vLuminanceTable.rgb);
	float fLogLuminance = log(fluminance + 0.0001f);
	return fLogLuminance;
}

uniform float4 vTexOffset[16];

float4 PS_Average16Samples(VS_OUTPUT In) : SV_Target
{
	float4 color = 0;
	
	for ( int i=0; i<16; i++ )
	{
		float2 texcoord = In.Texcoord.xy + vTexOffset[i].xy;
		float4 hdr = Image0.Sample(PointSamplerRepeat, texcoord);
		color += hdr;
	}
	
	color *= 1.0f/16.0f;
	
	return color;
}

float4 PS_ExpLuminance(VS_OUTPUT In) : SV_Target
{
	float fLogLuminance = Image0.Sample(PointSampler, In.Texcoord).r;
	float fLuminance = exp(fLogLuminance);
	return fLuminance;
}

uniform float4 vAdaptiveSpeed;

float4 PS_AdaptiveLuminance(VS_OUTPUT In) : SV_Target
{
	float fPrevLuminance = Image0.Sample(PointSampler, In.Texcoord).r;
	float fCurrentLuminance = Image1.Sample(PointSampler, In.Texcoord).r;
	float fNewLuminance = lerp(fPrevLuminance, fCurrentLuminance, vAdaptiveSpeed.x);
	
	return fNewLuminance;
}

uniform float4 vMiddleGray;
uniform float4 vMultiplierClamp;

float4 PS_AutoExposure(VS_OUTPUT In) : SV_Target
{
	float4 HDRImage = Image0.Sample(PointSampler, In.Texcoord);
	float fAverageLuminance = Image1.Sample(PointSampler, float2(0.5f, 0.5f)).r;

	float multiplier = pow(vMiddleGray.r / fAverageLuminance, 0.5);
	HDRImage.rgb *= multiplier;

	return HDRImage;
}

technique10 Luminance
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Luminance() ) );
	}
}

technique10 LogLuminance
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_LogLuminance() ) );
	}
}

technique10 Average16Samples
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_Average16Samples() ) );
	}
}

technique10 ExpLuminance
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_ExpLuminance() ) );
	}
}

technique10 AdaptiveLuminance
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_AdaptiveLuminance() ) );
	}
}

technique10 AutoExposure
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_Passthrough() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_AutoExposure() ) );
	}
}
