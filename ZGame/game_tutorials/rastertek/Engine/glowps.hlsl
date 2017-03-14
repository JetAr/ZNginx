////////////////////////////////////////////////////////////////////////////////
// Filename: glow.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D colorTexture : register(t0);
Texture2D glowTexture : register(t1);
SamplerState SampleType;

cbuffer GlowBuffer
{
	float glowStrength;
	float3 padding;
};

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
float4 GlowPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 glowColor;
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = colorTexture.Sample(SampleType, input.tex);

	// Sample the glow texture.
	glowColor = glowTexture.Sample(SampleType, input.tex);

	// Add the texture color to the glow color multiplied by the glow stength.
	color = saturate(textureColor + (glowColor * glowStrength));

	return color;
}