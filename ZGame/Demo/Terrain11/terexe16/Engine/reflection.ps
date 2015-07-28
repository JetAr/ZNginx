////////////////////////////////////////////////////////////////////////////////
// Filename: reflection.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);


//////////////
// SAMPLERS //
//////////////
SamplerState SampleType;


//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer
{
	float4 lightDiffuseColor;
	float3 lightDirection;
	float colorTextureBrightness;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
   	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 color : COLOR;
   	float clip : SV_ClipDistance0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ReflectionPixelShader(PixelInputType input) : SV_TARGET
{
	float3 lightDir;
	float4 textureColor;
	float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 color;


    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Sample the color texture.
    textureColor = colorTexture.Sample(SampleType, input.tex);

	// Combine the color map value into the texture color.
	textureColor = saturate(input.color * textureColor * colorTextureBrightness);

	// Calculate the bump map using the normal map.
	bumpMap = normalTexture.Sample(SampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    
	// Calculate the first bump mapped pixel color.
	color = saturate(lightDiffuseColor * lightIntensity);
    color = color * textureColor;

	return color;
}