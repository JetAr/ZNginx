////////////////////////////////////////////////////////////////////////////////
// Filename: glow.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D colorTexture;
Texture2D glowTexture;
float glowStrength;


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
PixelInputType GlowVertexShader(VertexInputType input)
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
float4 GlowPixelShader(PixelInputType input) : SV_Target
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


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 GlowTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, GlowVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, GlowPixelShader()));
        SetGeometryShader(NULL);
    }
}