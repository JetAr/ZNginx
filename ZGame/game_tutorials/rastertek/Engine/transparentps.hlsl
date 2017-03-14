////////////////////////////////////////////////////////////////////////////////
// Filename: transparent.ps
////////////////////////////////////////////////////////////////////////////////

//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType : register(s0);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
	float2 tex : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TransparentPixelShader(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color;
	float4 textureColor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Test based on the alpha value of the texture.
	if (textureColor.a > 0.8f)
	{
		// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
		depthValue = input.depthPosition.z / input.depthPosition.w;
	}
	else 
	{
		// Otherwise discard this pixel entirely.
		discard;
	}

	color = float4(depthValue, depthValue, depthValue, 1.0f);

	return color;
}