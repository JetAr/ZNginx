//----------------------------------------------------------------------------------
// File:        DeinterleavedTexturing\media/Scene3D.hlsl
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#pragma pack_matrix( row_major )

cbuffer GlobalConstantBuffer : register(b0)
{
  float4x4 g_WorldView;
  float4x4 g_WorldViewInverse;
  float4x4 g_WorldViewProjection;
  float  g_IsWhite;
};

Texture2D<float3> tColor    : register(t0);
SamplerState PointSampler   : register(s0);

//-----------------------------------------------------------------------------
// Geometry-rendering shaders
//-----------------------------------------------------------------------------

struct VS_INPUT
{
    float3 WorldPosition            : POSITION;
    float3 WorldNormal              : NORMAL;
};

struct VS_OUTPUT
{
    float4 HPosition                : SV_POSITION;
    centroid float3 ViewPosition    : TEXCOORD0;
    centroid float3 WorldNormal     : TEXCOORD1;
};

struct PS_OUTPUT
{
    float4 Color                    : SV_Target0;
};

VS_OUTPUT GeometryVS( VS_INPUT input )
{
    VS_OUTPUT output;
    output.HPosition = mul( float4(input.WorldPosition,1), g_WorldViewProjection );
    output.ViewPosition = mul( float4(input.WorldPosition,1), g_WorldView ).xyz;
    output.WorldNormal = input.WorldNormal.xyz;
    return output;
}

PS_OUTPUT GeometryPS( VS_OUTPUT input )
{
    PS_OUTPUT output;
    output.Color = g_IsWhite ? float4(1,1,1,1) : float4(.457,.722, 0.0, 1);
    return output;
}

//-----------------------------------------------------------------------------
// Post-processing shaders
//-----------------------------------------------------------------------------

struct PostProc_VSOut
{
    float4 pos : SV_Position;
    float2 uv  : TexCoord;
};

// Vertex shader that generates a full screen quad with texcoords
// To use draw 3 vertices with primitive type triangle list
PostProc_VSOut FullScreenTriangleVS( uint id : SV_VertexID )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;
    output.uv = float2( (id << 1) & 2, id & 2 );
    output.pos = float4( output.uv * float2( 2.0f, -2.0f ) + float2( -1.0f, 1.0f), 0.0f, 1.0f );
    return output;
}

float4 CopyColorPS( PostProc_VSOut IN ) : SV_TARGET
{
    float3 color = tColor.Sample(PointSampler, IN.uv);
    return float4(color,1);
}
