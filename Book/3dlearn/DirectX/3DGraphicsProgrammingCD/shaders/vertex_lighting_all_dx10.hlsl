// `設定頂點的資料格式`
struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Normal	: Normal;
};

// `設定Vertex Shader輸出的資料格式`
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color	: COLOR;
};

struct Light_Info
{
	float4 m_vPosition;
	float4 m_vDirection;
	
	float4 m_vAmbientColor;
	float4 m_vDiffuseColor;
	float4 m_vSpecularColor;
	
	float4 m_vAttenuation;

	float  m_fSpotlightCutoffCosine;
	float  m_fSpotlightExponent;	
		
	int	   m_iLightType;
	bool   m_bEnabled;
};

// `轉換矩陣`
cbuffer Matrices : register(c0)
{
	uniform row_major float4x4 world_view_proj_matrix;
	uniform row_major float4x4 world_matrix;
	uniform float4 camera_position;
};

// `光源`
cbuffer Lights : register(c1)
{
	uniform float4 g_vAmbientLight;
	uniform float4 g_vMaterialShininess;
	Light_Info g_Lights[3];
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	//  `view_matrix最下面那一行 = 鏡頭位置`
	float3 vCameraPosition = camera_position;
	
	// 轉換到螢幕座標
	Out.Position = mul( In.Position, world_view_proj_matrix);
	
	// `計算Normal, Position在世界座標系上的方向跟位置`
	float3 vWorldNormal = mul( In.Normal, (float3x3) world_matrix);
	float3 vWorldPosition = mul( In.Position, world_matrix);
	
	// `初值`
	float4 vLighting = g_vAmbientLight;
	
	for ( int i=0; i<3; i++ )
	{
		if ( g_Lights[i].m_bEnabled )
		{
			float4 vAmbient = g_Lights[i].m_vAmbientColor;
			float4 vDiffuse, vSpecular;
			float3 vLightDir;
			float  fLightAttenuation = 1.0f;
			float  fSpotLightEffect = 1.0f;
						
			if ( g_Lights[i].m_iLightType==1 )
			// `方向光`
			{
				vLightDir = g_Lights[i].m_vDirection;
			}
			else
			// `點光源跟聚光燈`
			{
				// `計算光的方向跟距離`
				float3 vDiff = g_Lights[i].m_vPosition.xyz - vWorldPosition;
				float fLightDistance = length(vDiff);
				vLightDir = vDiff / fLightDistance;

				fLightAttenuation = dot(float3(1.0f, fLightDistance, fLightDistance * fLightDistance), g_Lights[i].m_vAttenuation);
				
				if( g_Lights[i].m_iLightType==3 )
				// `聚光燈`
				{
					// `計算聚光燈的效果`
					float fSpotLightCosine = dot(-vLightDir, g_Lights[i].m_vDirection);
					fSpotLightCosine = fSpotLightCosine > g_Lights[i].m_fSpotlightCutoffCosine ? fSpotLightCosine : 0.0f;
					fSpotLightEffect = pow(fSpotLightCosine, g_Lights[i].m_fSpotlightExponent);
				}
			}

			// `光源的Diffuse部份`
			vDiffuse = g_Lights[i].m_vDiffuseColor * saturate(dot(vLightDir, vWorldNormal));
			
			// `方源的Spcular部份`
			float3 vCameraDir = normalize(vCameraPosition - vWorldPosition);
			float3 vHalfDir = normalize(vLightDir + vCameraDir);
			float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
			float  fSpecular = pow(fSpecularCosine, g_vMaterialShininess.x);
			vSpecular = g_Lights[i].m_vSpecularColor * fSpecular;
			
			vLighting += vAmbient + (vDiffuse + vSpecular)/fLightAttenuation * fSpotLightEffect;
		}
	}	
	
	Out.Color = vLighting;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	// `使用頂點間內插出來的顏色`
	return In.Color;
}
