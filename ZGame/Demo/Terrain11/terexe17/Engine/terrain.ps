////////////////////////////////////////////////////////////////////////////////
// Filename: terrain.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TEXTURES //
//////////////
Texture2D colorTexture1 : register(t0);
Texture2D colorTexture2 : register(t1);
Texture2D colorTexture3 : register(t2);
Texture2D colorTexture4 : register(t3);
Texture2D alphaTexture1 : register(t4);
Texture2D normalMap1 : register(t5);
Texture2D normalMap2 : register(t6);


//////////////
// SAMPLERS //
//////////////
SamplerState SampleType;


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
   	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
    float2 tex2 : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
	float3 lightDir;
	float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity1;
    float lightIntensity2;
	float4 textureColor1;
	float4 textureColor2;
	float4 textureColor3;
	float4 textureColor4;
	float4 alphaMap1;
	float4 baseColor;
	float4 color;


	// Invert the light direction for calculations.
    lightDir = -lightDirection;

	// Calculate the first bump map using the first normal map.
	bumpMap = normalMap1.Sample(SampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	bumpNormal = normalize(bumpNormal);
    lightIntensity1 = saturate(dot(bumpNormal, lightDir));

	// Calculate the second bump map using the second normal map.
	bumpMap = normalMap2.Sample(SampleType, input.tex);
	bumpMap = (bumpMap * 2.0f) - 1.0f;
	bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	bumpNormal = normalize(bumpNormal);
    lightIntensity2 = saturate(dot(bumpNormal, lightDir));

    // Sample the color textures.
    textureColor1 = colorTexture1.Sample(SampleType, input.tex);
    textureColor2 = colorTexture2.Sample(SampleType, input.tex);
    textureColor3 = colorTexture3.Sample(SampleType, input.tex);
    textureColor4 = colorTexture4.Sample(SampleType, input.tex);

	// Add the bump maps to their respective textures.
    textureColor1 = saturate(lightIntensity1 * textureColor1);
    textureColor2 = saturate(lightIntensity1 * textureColor2);
    textureColor3 = saturate(lightIntensity1 * textureColor3);
    textureColor4 = saturate(lightIntensity2 * textureColor4);

    // Sample the alpha map using second set of texture coordinates.
	alphaMap1 = alphaTexture1.Sample(SampleType, input.tex2);

	// Set the base color to the first color texture.
	baseColor = textureColor1;

	// Add the second layer using the red channel of the alpha map.
	color = lerp(baseColor, textureColor2, alphaMap1.r);

	// Add the third layer using the green channel of the alpha map.
	color = lerp(color, textureColor3, alphaMap1.g);

	// Add the fourth layer using the blue channel of the alpha map.
	color = lerp(color, textureColor4, alphaMap1.b);
	
	return color;
}