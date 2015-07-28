////////////////////////////////////////////////////////////////////////////////
// Filename: glowmap.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
Texture2D glowMapTexture;


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType GlowMapVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 GlowMapPixelShader(PixelInputType input) : SV_Target
{
	float4 textureColor;
	float4 glowMap;
	float4 color;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Sample the glow map.
	glowMap = glowMapTexture.Sample(SampleType, input.tex);

	// If the glow map is black then return just black.  Otherwise if the glow map has color then return the color from the texture.
	if((glowMap.r == 0.0f) && (glowMap.g == 0.0f) && (glowMap.b == 0.0f))
	{
		color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		color = textureColor;
	}

    return color;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 GlowMapTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, GlowMapVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, GlowMapPixelShader()));
        SetGeometryShader(NULL);
    }
}