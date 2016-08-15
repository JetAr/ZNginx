/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

*/

float4x4 WorldViewProj : WorldViewProjection;

float4 mainVS(float3 pos : POSITION) : POSITION{
	return mul(float4(pos.xyz, 1.0), WorldViewProj);
}

float4 mainPS() : COLOR {
	return float4(1.0, 1.0, 1.0, 1.0);
}

technique technique0 {
	pass p0 {
		CullMode = None;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
