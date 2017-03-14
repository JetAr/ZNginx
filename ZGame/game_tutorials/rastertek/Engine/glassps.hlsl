////////////////////////////////////////////////////////////////////////////////
// Filename: glass.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
SamplerState SampleType;

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D refractionTexture : register(t2);

cbuffer GlassBuffer
{
	float refractionScale;
	float3 padding;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 refractionPosition : TEXCOORD1;
};

float4 GlassPixelShader(PixelInputType input) : SV_TARGET
{
	float2 refractTexCoord;
	float4 normalMap;
	float3 normal;
	float4 refractionColor;
	float4 textureColor;
	float4 color;

	// Calculate the projected refraction texture coordinates.
	refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
	refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	// Sample the normal from the normal map texture.
	normalMap = normalTexture.Sample(SampleType, input.tex);

	// Expand the range of the normal from (0,1) to (-1,+1).
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	// Re-position the texture coordinate sampling position by the normal map value to simulate light distortion through glass.
	refractTexCoord = refractTexCoord + (normal.xy * refractionScale);

	// Sample the texture pixel from the refraction texture using the perturbed texture coordinates.
	refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// Sample the texture pixel from the glass color texture.
	textureColor = colorTexture.Sample(SampleType, input.tex);

	// Evenly combine the glass color and refraction value for the final color.
	color = lerp(refractionColor, textureColor, 0.5f);

	return color;
}