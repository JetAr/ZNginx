// 設定頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Normal	: Normal;
};

// 設定Vertex Shader輸出的資料格式
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

	float  m_fSpotlightInnerCone;
	float  m_fSpotlightCutoffCosine;
	float  m_fSpotlightExponent;	
	
	int	   m_iLightType;
	bool   m_bEnabled;
};

// 轉換矩陣
cbuffer Matrices : register(b0)
{
	uniform row_major float4x4 viewproj_matrix;
	uniform row_major float4x4 world_matrix;
};

// 光源
cbuffer Lights : register(b1)
{
	uniform float4 g_vGlobal_AmbientLight;
	Light_Info g_Lights[3];
};

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( In.Position, viewproj_matrix);
	float3 world_position = mul( In.Position, world_matrix);
	float3x3 world_matrix_3x3 = (float3x3) world_matrix;
	float3 world_normal = mul( In.Normal, world_matrix_3x3);
	
	float4 lighting = g_vGlobal_AmbientLight;
	
	for ( int i=0; i<3; i++ )
	{
		if ( g_Lights[i].m_bEnabled )
		{
			float4 vIntensity = g_Lights[i].m_vAmbientColor;
		
			switch(g_Lights[i].m_iLightType)
			{
			case 1: // directional light
				{
					vIntensity += saturate(dot(g_Lights[i].m_vDirection, world_normal));
				}
				break;
			case 2: // point light
				{
					float3 light_direction = (g_Lights[i].m_vPosition - world_position);
					float  light_distance = length(light_direction);
					light_direction /= light_distance;
					float  fCosine = saturate(dot(light_direction, world_normal));
					float3 vAttenuation = float3(1.0f, light_distance, light_distance * light_distance);
					float  attenuation = dot(vAttenuation, g_Lights[i].m_vAttenuation);
					vIntensity += fCosine / attenuation;
				}
				break;
			case 3: // spot light
				{
					float3 light_direction = (g_Lights[i].m_vPosition - world_position);
					float  light_distance = length(light_direction);
					light_direction /= light_distance;
					float  fCosine = saturate(dot(-g_Lights[i].m_vDirection, light_direction));
					if ( fCosine >= g_Lights[i].m_fSpotlightCutoffCosine )
					{
						float  SpotlightEffect = pow(fCosine, g_Lights[i].m_fSpotlightExponent);
						float  fCosine2 = saturate(dot(light_direction, world_normal));
						float3 vAttenuation = float3(1.0f, light_distance, light_distance * light_distance);
						float  attenuation = dot(vAttenuation, g_Lights[i].m_vAttenuation);
						vIntensity += SpotlightEffect * fCosine2 / attenuation;
					}
				}	
				break;
			}
			
			lighting += vIntensity * g_Lights[i].m_vDiffuseColor;
		}
	}	
	
	Out.Color = lighting;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	// 使用頂點間內插出來的顏色
	return In.Color;
}
