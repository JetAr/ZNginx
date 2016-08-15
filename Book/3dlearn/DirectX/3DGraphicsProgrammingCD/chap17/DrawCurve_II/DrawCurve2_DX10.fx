struct VS_INPUT
{
	float4 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

struct GS_OUTPUT
{
	float4 Position : POSITION;
};

// 沒有做任何事情的vertex shader
VS_OUTPUT VS(void)
{
	VS_OUTPUT Out;

	Out.Position = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return Out;
}

VS_OUTPUT VS_passthrough(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.Position = float4(In.Position.xy, 0, 1);

	return Out;
}

float4 PS(void) : SV_Target
{
	return float4(1,1,1,1);
}

float func(float x)
{
	return sin(x);
}

float2 MapToScreenSpace(float2 f, float2 min, float2 ext)
{
	float2 diff = f-min;
	float2 t = 2*diff/ext - 1.0f;
	return t;	
}

// 畫出sin函式的geometry shader
[maxvertexcount(256)]
void GS_sin(inout PointStream<GS_OUTPUT> stream )
{
	VS_INPUT v;
	
	const  float PI = 3.14159f;
	float2 vMin = float2(0.0f, -PI);
	float2 vMax = float2(PI*2.0f, PI);
	float2 vExt = vMax - vMin;
	float  fStep = vExt.x / 256.0f;
	float  fX = vMin.x;
	float  fY = func(fX);
	float2 t = MapToScreenSpace(float2(fX, fY), vMin, vExt);
	
	v.Position = float4(t, 0, 1);
	
	stream.Append(v);
	
	for ( fX=vMin.x+fStep; fX<=vMax.x; fX+=fStep )
	{
		fY = func(fX);
		t = MapToScreenSpace(float2(fX, fY), vMin, vExt);
		v.Position = float4(t, 0, 1);
		stream.Append(v);
	}
}

GeometryShader SinStreamOut = ConstructGSWithSO( CompileShader( gs_4_0, GS_sin() ), "POSITION.xyzw" );

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

technique10 SinCurve
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( SinStreamOut );
        SetPixelShader( NULL );
        SetDepthStencilState( DisableDepth, 0 );
	}
}

float4x4 proj_matrix;
float2   NearFarPlane;

float ScreenSpaceZ(float z)
{
	float4 pos = float4(0.0f, 0.0f, -z, 1.0f);
	float4 screen_pos = mul(pos, proj_matrix);
	return screen_pos.z / screen_pos.w;
}

// 畫出ZBuffer分布的geometry shader
[maxvertexcount(256)]
void GS_z(inout PointStream<GS_OUTPUT> stream )
{
	GS_OUTPUT v;
	
	const float PI = 3.14159f;
	float2 vMin = float2(NearFarPlane.x, 0.0f);
	float2 vMax = float2(NearFarPlane.y, 1.0f);
	float2 vExt = vMax - vMin;
	float  fStep = vExt.x / 256.0f;
	float  fX = vMin.x;
	float  fY = ScreenSpaceZ(fX);
	float2 t = MapToScreenSpace(float2(fX, fY), vMin, vExt);
	
	v.Position = float4(t, 0.0f, 1.0f);
	stream.Append(v);
	
	for ( fX=vMin.x+fStep; fX<=vMax.x; fX+=fStep )
	{
		fY = ScreenSpaceZ(fX);
		t = MapToScreenSpace(float2(fX, fY), vMin, vExt);
		v.Position = float4(t, 0.0f, 1.0f);
		stream.Append(v);
	}
}

GeometryShader ZCurveStreamOut = ConstructGSWithSO( CompileShader( gs_4_0, GS_z() ), "POSITION.xyzw" );

technique10 ZCurve
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( ZCurveStreamOut );
        SetPixelShader( NULL );
	}
}

technique10 DrawCurve
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_passthrough() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}