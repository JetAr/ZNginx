////////////////////////////////////////////////////////////////////////////////
// Filename: alphamap.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTextures[3];


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
PixelInputType AlphaMapVertexShader(VertexInputType input)
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
float4 AlphaMapPixelShader(PixelInputType input) : SV_Target
{
	float4 color1;
    float4 color2;
    float4 alphaValue;
    float4 blendColor;


    // Get the pixel color from the first texture.
    color1 = shaderTextures[0].Sample(SampleType, input.tex);

    // Get the pixel color from the second texture.
    color2 = shaderTextures[1].Sample(SampleType, input.tex);

	// Get the alpha value from the alpha map texture.
	alphaValue = shaderTextures[2].Sample(SampleType, input.tex);
	
    // Combine the two textures based on the alpha value.
    blendColor = (alphaValue * color1) + ((1.0 - alphaValue) * color2);

    // Saturate the final color value.
    blendColor = saturate(blendColor);

    return blendColor;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 AlphaMapTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, AlphaMapVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, AlphaMapPixelShader()));
        SetGeometryShader(NULL);
    }
}