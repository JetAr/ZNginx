////////////////////////////////////////////////////////////////////////////////
// Filename: glowmap.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t0);
Texture2D glowMapTexture : register(t1);
SamplerState SampleType;


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 GlowMapPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 glowMap;
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Sample the glow map.
	glowMap = glowMapTexture.Sample(SampleType, input.tex);

	// If the glow map is black then return just black.  Otherwise if the glow map has color then return the color from the texture.
	if ((glowMap.r == 0.0f) && (glowMap.g == 0.0f) && (glowMap.b == 0.0f))
	{
		color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		color = textureColor;
	}

	return color;
}