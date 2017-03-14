////////////////////////////////////////////////////////////////////////////////
// Filename: softshadow.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D shaderTexture : register(t0);
Texture2D shadowTexture : register(t1);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 SoftShadowPixelShader(PixelInputType input) : SV_TARGET
{
	float4 color;
	float lightIntensity;
	float4 textureColor;
	float2 projectTexCoord;
	float shadowValue;


	// Set the default output color to the ambient light value for all pixels.
	color = ambientColor;

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, input.lightPos));
	if (lightIntensity > 0.0f)
	{
		// Determine the light color based on the diffuse color and the amount of light intensity.
		color += (diffuseColor * lightIntensity);

		// Saturate the light color.
		color = saturate(color);
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;

	// Calculate the projected texture coordinates to be used with the shadow texture.
	projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

	// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
	shadowValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;

	// Combine the shadows with the final color.
	color = color * shadowValue;

	return color;
}