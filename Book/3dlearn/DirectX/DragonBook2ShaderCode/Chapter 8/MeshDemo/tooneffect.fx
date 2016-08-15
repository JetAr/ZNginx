////////////////////////////////////////////////////////////////////////////
// 
// File: tooneffect.txt
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Cartoon shader in an effect file.
//          
////////////////////////////////////////////////////////////////////////////

//
// Globals
//

uniform extern float4x4 WorldViewMatrix;
uniform extern float4x4 WorldViewProjMatrix;

uniform extern float4 Color;
uniform extern float4 LightDirection;

static float4 lightD;

//
// Structures
//

struct VS_OUTPUT
{
    float4 position : POSITION0;
    float2 uvCoords : TEXCOORD0;
    float4 diffuse  : COLOR0;
};
//
// Main
//

VS_OUTPUT Main(float4 position : POSITION0,float4 normal : NORMAL0)
{
    // zero out each member in output
    VS_OUTPUT output = (VS_OUTPUT)0;


    // transform vertex position to homogenous clip space
     output.position = mul(position, WorldViewProjMatrix);

    //
    // Transform lights and normals to view space.  Set w
    // components to zero since we're transforming vectors.
    // Assume there are no scalings in the world
    // matrix as well.
    //
    lightD.w = 0.0f;
    lightD.w   = 0.0f;
    lightD   = mul(LightDirection, WorldViewMatrix);
    normal     = mul(normal, WorldViewMatrix);

    //
    // Compute the 1D texture coordinate for toon rendering.
    //
    float u = dot(lightD, normal);

    //
    // Clamp to zero if u is negative because u
    // negative implies the angle between the light
    // and normal is greater than 90 degrees.  And
    // if that is true then the surface receives
    // no light.
    //
    if( u < 0.0f )
        u = 0.0f;

    // 
    // Set other tex coord to middle.
    //
    float v = 0.5f;


    output.uvCoords.x = u;
    output.uvCoords.y = v;

    // save color
    output.diffuse = Color;
    
    return output;
}


float4 ColorPS(float4 c : COLOR0) : COLOR
{
    return c;
}


// 
// Effect
//

technique Toon
{
    pass P0
    {
        vertexShader = compile vs_2_0 Main();
        pixelShader  = compile ps_2_0 ColorPS();
        
        FillMode = Wireframe;
    }
}
