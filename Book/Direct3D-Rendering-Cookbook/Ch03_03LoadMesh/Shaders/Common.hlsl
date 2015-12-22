// Copyright (c) 2013 Justin Stenning
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//-------------------------------
// IMPORTANT: When creating a new shader file use "Save As...", "Save with encoding", 
// and then select "Western European (Windows) - Codepage 1252" as the 
// D3DCompiler cannot handle the default encoding of "UTF-8 with signature"
//-------------------------------

// Constant buffer to be updated by application per object
cbuffer PerObject : register(b0)
{
    // WorldViewProjection matrix
    float4x4 WorldViewProjection;
    
    // We need the world matrix so that we can
    // calculate the lighting in world space
    float4x4 World;
    
    // Inverse transpose of world, used for
    // bringing normals into world space, especially
    // necessary where non-uniform scaling has been applied
    float4x4 WorldInverseTranspose;
};

// A simple directional light (e.g. the sun)
struct DirectionalLight
{
    float4 Color;
    float3 Direction;
};

// Constant buffer - updated once per frame
// Note: HLSL data is packed in such a
// way that it does not cross a 16-byte boundary
cbuffer PerFrame: register (b1)
{
    DirectionalLight Light;
    float3 CameraPosition;
};

// Constant buffer to hold our material configuration
// Note: HLSL data is packed in such a
// way that it does not cross a 16-bytes boundary
cbuffer PerMaterial : register (b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float MaterialSpecularPower;
    bool HasTexture;
    float4 MaterialEmissive;
    float4x4 UVTransform;
};

// Vertex Shader input structure (from Application)
struct VertexShaderInput
{
    float4 Position : SV_Position;// Position - xyzw
    float3 Normal : NORMAL;    // Normal - for lighting and mapping operations
    float4 Color : COLOR0;     // Color - vertex color, used to generate a diffuse color
    float2 TextureUV: TEXCOORD0; // UV - texture coordinate
};

// Pixel Shader input structure (from Vertex Shader)
struct PixelShaderInput
{
    float4 Position : SV_Position;
    // Interpolation of combined vertex and material diffuse
    float4 Diffuse : COLOR;
    // Interpolation of vertex UV texture coordinate
    float2 TextureUV: TEXCOORD0;

    // We need the World Position and normal for light calculations
    float3 WorldNormal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

float3 Lambert(float4 pixelDiffuse, float3 normal, float3 toLight)
{
    // Calculate diffuse color (using Lambert's Cosine Law - dot product of 
    // light and normal) Saturate to clamp the value within 0 to 1.
    float3 diffuseAmount = saturate(dot(normal, toLight));
    return pixelDiffuse.rgb * diffuseAmount;
}

float3 SpecularPhong(float3 normal, float3 toLight, float3 toEye)
{
    // R = reflect(i,n) => R = i - 2 * n * dot(i,n)
    float3 reflection = reflect(-toLight, normal);

    // Calculate the specular amount (smaller specular power = larger specular highlight)
    // Cannot allow a power of 0 otherwise the model will appear black and white
    float specularAmount = pow(saturate(dot(reflection,toEye)), max(MaterialSpecularPower,0.00001f));
    return MaterialSpecular.rgb * specularAmount;
}

float3 SpecularBlinnPhong(float3 normal, float3 toLight, float3 toEye)
{
    // Calculate the half vector
    float3 halfway = normalize(toLight + toEye);

    // Saturate is used to prevent backface light reflection
    // Calculate specular (smaller specular power = larger specular highlight)
    float specularAmount = pow(saturate(dot(normal, halfway)), max(MaterialSpecularPower,0.00001f));
    return MaterialSpecular.rgb * specularAmount;
}