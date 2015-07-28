////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D grassTexture : register(t0);
Texture2D slopeTexture : register(t1);
Texture2D rockTexture  : register(t2);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType;


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
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
	float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
	float4 grassColor;
	float4 slopeColor;
	float4 rockColor;
	float slope;
	float blendAmount;
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;


	// Sample the grass color from the texture using the sampler at this texture coordinate location.
    grassColor = grassTexture.Sample(SampleType, input.tex);

	// Sample the slope color from the texture using the sampler at this texture coordinate location.
    slopeColor = slopeTexture.Sample(SampleType, input.tex);

	// Sample the rock color from the texture using the sampler at this texture coordinate location.
    rockColor = rockTexture.Sample(SampleType, input.tex);

	// Calculate the slope of this point.
	slope = 1.0f - input.normal.y;

	// Determine which texture to use based on height.
	if(slope < 0.2)
	{
		blendAmount = slope / 0.2f;
		textureColor = lerp(grassColor, slopeColor, blendAmount);
	}
	
	if((slope < 0.7) && (slope >= 0.2f))
	{
		blendAmount = (slope - 0.2f) * (1.0f / (0.7f - 0.2f));
		textureColor = lerp(slopeColor, rockColor, blendAmount);
	}

	if(slope >= 0.7) 
	{
		textureColor = rockColor;
	}

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));
	
	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
    color = diffuseColor * lightIntensity;

    // Saturate the final light color.
    color = saturate(color);

	// Multiply the texture color and the final light color to get the result.
    color = color * textureColor;

	return color;
}