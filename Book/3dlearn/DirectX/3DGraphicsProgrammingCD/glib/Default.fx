//
// Passthrough Vertex Shader
//
float4 VS( float4 Pos : POSITION ) : SV_POSITION
{
    return Pos;
}


//
// Pixel Shader
//
float4 PS( float4 Pos : SV_POSITION ) : SV_Target
{
    return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}


technique10 Default
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
