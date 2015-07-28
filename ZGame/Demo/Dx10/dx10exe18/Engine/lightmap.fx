////////////////////////////////////////////////////////////////////////////////
// Filename: lightmap.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTextures[2];


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
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
PixelInputType LightMapVertexShader(VertexInputType input)
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
float4 LightMapPixelShader(PixelInputType input) : SV_Target
{
	float4 color;
    float4 lightColor;
    float4 finalColor;


    // Get the pixel color from the color texture.
    color = shaderTextures[0].Sample(SampleType, input.tex);

    // Get the pixel color from the light map.
    lightColor = shaderTextures[1].Sample(SampleType, input.tex);

    // Blend the two pixels together.
    finalColor = color * lightColor;

    return finalColor;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 LightMapTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, LightMapVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, LightMapPixelShader()));
        SetGeometryShader(NULL);
    }
}