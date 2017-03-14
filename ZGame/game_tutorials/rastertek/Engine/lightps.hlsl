////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypePoint : register(s0);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
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
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 colors;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	// Sample the colors from the color render texture using the point sampler at this texture coordinate location.
	colors = colorTexture.Sample(SampleTypePoint, input.tex);

	// Sample the normals from the normal render texture using the point sampler at this texture coordinate location.
	normals = normalTexture.Sample(SampleTypePoint, input.tex);

	// Invert the light direction for calculations.
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(normals.xyz, lightDir));

	// Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
	outputColor = saturate(colors * lightIntensity);

	return outputColor;

}