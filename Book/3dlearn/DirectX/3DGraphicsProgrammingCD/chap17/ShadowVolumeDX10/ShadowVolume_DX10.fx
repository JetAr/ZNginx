// 更新Shadowmap的Shader

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
	
	// `假設在光源位置放一個鏡頭來做繪圖, 算出頂點在這個鏡頭上的螢幕座標相對位置.`
	for ( i=0; i<6; i++ )
	{
		vertex[i] = input[i].LightPos / input[i].LightPos.w;
	}

	float3 vFaceNormal = ComputeNormal(vertex[0], vertex[2], vertex[4]);
	float4 color = float4(1,1,1,1);
		
	// `只處理面對光源的三角形.`
	if ( vFaceNormal.z > 0 )
	{
		// `計算相鄰三角形的 normal 面向.`
		float3 vFace0Normal = ComputeNormal(vertex[0], vertex[1], vertex[2]);
		float3 vFace1Normal = ComputeNormal(vertex[2], vertex[3], vertex[4]);
		float3 vFace2Normal = ComputeNormal(vertex[0], vertex[4], vertex[5]);

		// `計算光線位置到達三角形3個頂點的方向.`
		// `平行光可以直接帶入光線方向, 不需要計算.`
		float3 vDir0 = normalize(input[0].WorldPos - light_pos);
		float3 vDir2 = normalize(input[2].WorldPos - light_pos);
		float3 vDir4 = normalize(input[4].WorldPos - light_pos);

		// `計算三角形3個頂點沿著光線方向延伸出去的頂點.`
		float3 ExtendedWorldPos0 = input[0].WorldPos + vDir0 * light_range;
		float3 ExtendedWorldPos2 = input[2].WorldPos + vDir2 * light_range;
		float3 ExtendedWorldPos4 = input[4].WorldPos + vDir4 * light_range;

		// `計算延申出去的頂點在螢幕座標系上的位置.`
		float4 ExtendedScreenPos0 = mul(float4(ExtendedWorldPos0, 1), vp_matrix);
		float4 ExtendedScreenPos2 = mul(float4(ExtendedWorldPos2, 1), vp_matrix);
		float4 ExtendedScreenPos4 = mul(float4(ExtendedWorldPos4, 1), vp_matrix);
		
		GS_OUTPUT output;

		output.Color = float4(1,0,0,1);

		// `如果某個相鄰的面是背對光源, 代表找到了一條邊線, 延伸出兩片三角形.`
		
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

		// `使用 ZFail 方法來標示陰影才需要前後的蓋子.`
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
