//=============================================================================
// rain.fx by Frank Luna (C) 2004 All Rights Reserved.
//
// Falling rain.
//=============================================================================

uniform extern float4x4 gWVP;
uniform extern texture  gTex;
uniform extern float3   gEyePosL;
uniform extern float3   gAccel;
uniform extern float    gTime;
uniform extern int      gViewportHeight;

sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU  = CLAMP;
    AddressV  = CLAMP;
};
 
struct OutputVS
{
    float4 posH  : POSITION0;
    float2 tex0  : TEXCOORD0; // D3D fills in for point sprites.
    float size   : PSIZE; // In pixels.
};

OutputVS RainVS(float3 posL    : POSITION0, 
                float3 vel     : TEXCOORD0,
                float size     : TEXCOORD1,
                float time     : TEXCOORD2,
                float lifeTime : TEXCOORD3,
                float mass     : TEXCOORD4,
                float4 color   : COLOR0)
{
    // Zero out our output.
	OutputVS outVS = (OutputVS)0;
	
	// Get age of particle from creation time.
	float t = gTime - time;
	
	// Constant acceleration.
	posL = posL + vel*t + 0.5f * gAccel * t * t;
	
	// Transform to homogeneous clip space.
	outVS.posH = mul(float4(posL, 1.0f), gWVP);
		
	// Do not scale rain drops as a function of distance.
	// Compute size as a function of the viewport height.  
	// The constant was found by experimenting.
	outVS.size = 0.0035f*gViewportHeight*size;

	// Done--return the output.
    return outVS;
}

float4 RainPS(float2 tex0 : TEXCOORD0) : COLOR
{
	return tex2D(TexS, tex0);
}

technique RainTech
{
    pass P0
    {
        vertexShader = compile vs_2_0 RainVS();
        pixelShader  = compile ps_2_0 RainPS();
        
        PointSpriteEnable = true;
        AlphaTestEnable   = true;
		AlphaRef          = 100;
		AlphaFunc         = GreaterEqual;
    }
}
