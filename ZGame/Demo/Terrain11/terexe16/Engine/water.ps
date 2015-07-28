////////////////////////////////////////////////////////////////////////////////
// Filename: water.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
SamplerState SampleType;
Texture2D refractionTexture : register(t0);
Texture2D reflectionTexture : register(t1);
Texture2D normalTexture : register(t2);

cbuffer WaterBuffer
{
	float4 refractionTint;
	float3 lightDirection;
	float waterTranslation;
	float reflectRefractScale;
	float specularShininess;
	float2 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 reflectionPosition : TEXCOORD0;
    float4 refractionPosition : TEXCOORD1;
	float3 viewDirection : TEXCOORD2;
    float2 tex1 : TEXCOORD3;
    float2 tex2 : TEXCOORD4;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 WaterPixelShader(PixelInputType input) : SV_TARGET
{
	float4 normalMap1;
	float4 normalMap2;
	float3 normal1;
	float3 normal2;
	float3 normal;
	float2 refractTexCoord;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 refractionColor;
	float3 heightView;
	float r;
	float fresnelFactor;
	float4 color;
	float3 reflection;
	float specular;


	// Move the position the water normal is sampled from to simulate moving water.	
	input.tex1.y += waterTranslation;
	input.tex2.y += waterTranslation;

	// Sample the normal from the normal map texture using the two different tiled and translated coordinates.
	normalMap1 = normalTexture.Sample(SampleType, input.tex1);
	normalMap2 = normalTexture.Sample(SampleType, input.tex2);
	
	// Expand the range of the normal from (0,1) to (-1,+1).
	normal1 = (normalMap1.rgb * 2.0f) - 1.0f;
	normal2 = (normalMap2.rgb * 2.0f) - 1.0f;

	// Combine the normals to add the normal maps together.
	normal = normalize(normal1 + normal2);
		
	// Calculate the projected refraction texture coordinates.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;
	
	// Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
		
	// Re-position the texture coordinate sampling position by the scaled normal map value to simulate the rippling wave effect.
	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);
	
	// Sample the texture pixels from the textures using the updated texture coordinates.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// Combine the tint with the refraction color.
	refractionColor = saturate(refractionColor * refractionTint);

	// Get a modified viewing direction of the camera that only takes into account height.
	heightView.x = input.viewDirection.y;
	heightView.y = input.viewDirection.y;
	heightView.z = input.viewDirection.y;

	// Now calculate the fresnel term based solely on height.
	r = (1.2f - 1.0f) / (1.2f + 1.0f);
	fresnelFactor = max(0.0f, min(1.0f, r + (1.0f - r) * pow(1.0f - dot(normal, heightView), 2)));

	// Combine the reflection and refraction results for the final color using the fresnel factor.
	color = lerp(reflectionColor, refractionColor, fresnelFactor);

	// Calculate the reflection vector using the normal and the direction of the light.
	reflection = -reflect(normalize(lightDirection), normal);
	
	// Calculate the specular light based on the reflection and the camera position.
	specular = dot(normalize(reflection), normalize(input.viewDirection));

	// Check to make sure the specular was positive so we aren't adding black spots to the water.
	if(specular > 0.0f)
	{
		// Increase the specular light by the shininess value.
		specular = pow(specular, specularShininess);

		// Add the specular to the final color.
		color = saturate(color + specular);
	}

	return color;
}