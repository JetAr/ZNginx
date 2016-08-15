// ��sShadowmap��Shader

cbuffer cb0
{
	float4x4 wvp_matrix;
	float4x4 vp_matrix;
	float4x4 world_matrix;
	float4x4 light_matrix;
	float4	 light_pos;
	float	 light_range;
};

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 ScreenPos : SV_POSITION;
	float4 WorldPos : TEXCOORD0;
	float4 LightPos : TEXCOORD1;
};

struct GS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.ScreenPos = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.WorldPos =  mul( float4(In.Position, 1.0f), world_matrix);
	Out.LightPos =  mul( float4(In.Position, 1.0f), light_matrix);
	
	return Out;
}

float3 ComputeNormal(float3 a, float3 b, float3 c)
{
	float3 vDiff0 = normalize(b - a);
	float3 vDiff1 = normalize(c - a);
	float3 vNormal = cross(vDiff0, vDiff1);

	return vNormal;
}

[maxvertexcount(21)]
void GS( triangleadj VS_OUTPUT input[6], inout TriangleStream<GS_OUTPUT> stream )
{
	int i;
	float4 vertex[6];
	
	// `���]�b������m��@�����Y�Ӱ�ø��, ��X���I�b�o�����Y�W���ù��y�Ь۹��m.`
	for ( i=0; i<6; i++ )
	{
		vertex[i] = input[i].LightPos / input[i].LightPos.w;
	}

	float3 vFaceNormal = ComputeNormal(vertex[0], vertex[2], vertex[4]);
	float4 color = float4(1,1,1,1);
		
	// `�u�B�z����������T����.`
	if ( vFaceNormal.z > 0 )
	{
		// `�p��۾F�T���Ϊ� normal ���V.`
		float3 vFace0Normal = ComputeNormal(vertex[0], vertex[1], vertex[2]);
		float3 vFace1Normal = ComputeNormal(vertex[2], vertex[3], vertex[4]);
		float3 vFace2Normal = ComputeNormal(vertex[0], vertex[4], vertex[5]);

		// `�p����u��m��F�T����3�ӳ��I����V.`
		// `������i�H�����a�J���u��V, ���ݭn�p��.`
		float3 vDir0 = normalize(input[0].WorldPos - light_pos);
		float3 vDir2 = normalize(input[2].WorldPos - light_pos);
		float3 vDir4 = normalize(input[4].WorldPos - light_pos);

		// `�p��T����3�ӳ��I�u�ۥ��u��V�����X�h�����I.`
		float3 ExtendedWorldPos0 = input[0].WorldPos + vDir0 * light_range;
		float3 ExtendedWorldPos2 = input[2].WorldPos + vDir2 * light_range;
		float3 ExtendedWorldPos4 = input[4].WorldPos + vDir4 * light_range;

		// `�p�⩵�ӥX�h�����I�b�ù��y�Шt�W����m.`
		float4 ExtendedScreenPos0 = mul(float4(ExtendedWorldPos0, 1), vp_matrix);
		float4 ExtendedScreenPos2 = mul(float4(ExtendedWorldPos2, 1), vp_matrix);
		float4 ExtendedScreenPos4 = mul(float4(ExtendedWorldPos4, 1), vp_matrix);
		
		GS_OUTPUT output;

		output.Color = float4(1,0,0,1);

		// `�p�G�Y�Ӭ۾F�����O�I�����, �N����F�@����u, �����X����T����.`
		
		if ( vFace0Normal.z <= 0 )
		{
			output.Position = input[0].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos0;
			stream.Append(output);
			output.Position = input[2].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos2;
			stream.Append(output);
			
			stream.RestartStrip();
		}
		
		if ( vFace1Normal.z <=0 )
		{
			output.Position = input[2].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos2;
			stream.Append(output);
			output.Position = input[4].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos4;
			stream.Append(output);

			stream.RestartStrip();
		}
		
		if ( vFace2Normal.z <=0 )
		{
			output.Position = input[4].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos4;
			stream.Append(output);
			output.Position = input[0].ScreenPos;
			stream.Append(output);
			output.Position = ExtendedScreenPos0;
			stream.Append(output);

			stream.RestartStrip();
		}

		bool bZFail = true;

		// `�ϥ� ZFail ��k�ӼХܳ��v�~�ݭn�e�᪺�\�l.`
		if ( bZFail )
		{
			output.Color = float4(1,1,1,1);

			output.Position = input[0].ScreenPos;
			stream.Append(output);
			output.Position = input[2].ScreenPos;
			stream.Append(output);
			output.Position = input[4].ScreenPos;
			stream.Append(output);
			stream.RestartStrip();

			output.Position = ExtendedScreenPos0;
			stream.Append(output);
			output.Position = ExtendedScreenPos4;
			stream.Append(output);
			output.Position = ExtendedScreenPos2;
			stream.Append(output);
			stream.RestartStrip();
		}
	}
}

float4 PS(GS_OUTPUT input) : SV_Target
{
	return input.Color;
}

RasterizerState StencilPassRS
{
	FrontCounterClockWise = TRUE;
    CullMode = NONE;
};

BlendState StencilPassBS
{
	RenderTargetWriteMask[0] = 0x0;
};

DepthStencilState StencilPassDS
{
	DepthWriteMask = 0;
	StencilEnable = TRUE;
	DepthFunc = Less;
	
	FrontFaceStencilFunc = ALWAYS;
    FrontFaceStencilDepthFail = Decr;
	
	BackFaceStencilFunc = ALWAYS;
    BackFaceStencilDepthFail = Incr;
};

technique10 ShadowVolume
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

		SetRasterizerState(StencilPassRS);
		SetBlendState(StencilPassBS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);
		SetDepthStencilState(StencilPassDS, 0);
	}
}
