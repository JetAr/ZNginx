////////////////////////////////////////////////////////////////////////////////
// Filename: transparentdepth.fx
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;


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
    float4 depthPosition : TEXCOORD0;
    float2 tex : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TransparentDepthVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;
	
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TransparentDepthPixelShader(PixelInputType input) : SV_Target
{
	float depthValue;
	float4 color;
	float4 textureColor;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Test based on the alpha value of the texture.
	if(textureColor.a > 0.8f)
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


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 TransparentDepthTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TransparentDepthVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, TransparentDepthPixelShader()));
        SetGeometryShader(NULL);
    }
}