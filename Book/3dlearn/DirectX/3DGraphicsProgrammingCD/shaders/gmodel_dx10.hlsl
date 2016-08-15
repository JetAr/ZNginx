/*
	模擬fixed pipeline的Shader
	
			彭國倫 Peter Pon 
*/

#ifndef _VERTEXCOLOR
#define _VERTEXCOLOR 0
#endif

#ifndef _DIFFUSETEX0
#define _DIFFUSETEX0 0
#endif

#ifndef _DIFFUSETEX1
#define _DIFFUSETEX1 0
#endif

#ifndef _CUBEMAP
#define _CUBEMAP 0
#endif

#ifndef _LIGHTING
#define _LIGHTING 0 
#endif

#include "DefaultConstantBuffers.h"

// 設定頂點的資料格式
struct VS_INPUT
{
	float3 Position : POSITION;

#if (_VERTEXCOLOR)
	float4 Color : COLOR;
#endif

#if (_CUBEMAP || _LIGHTING)	
	float3 Normal	: NORMAL;
#endif

#if (_DIFFUSETEX0)
	float2 UV0		: TEXCOORD0;
#endif

#if (_DIFFUSETEX1)
	float2 UV1		: TEXCOORD1;
#endif
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
		
#if (_DIFFUSETEX0 || _DIFFUSETEX1)
	float4 UV		: TEXCOORD0;
#endif

#if (_CUBEMAP || _LIGHTING)
	float3 WorldPos : TEXCOORD1;
	float3 WorldNormal : TEXCOORD2;
#endif
};

Texture2D DiffuseTexture0;
SamplerState DiffuseTextureSampler0;

Texture2D DiffuseTexture1;
SamplerState DiffuseTextureSampler1;

TextureCube CubemapTexture;
SamplerState CubemapTextureSampler;

void CalculateLighting(float3 vWorldPos, float3 vWorldNormal, out float4 vDiffuseLight, out float4 vSpecularLight)
{
	float4 vCameraPos = inv_view_matrix[3];
	
	vDiffuseLight = g_vAmbientLight + vEmissive;
	vSpecularLight = 0;
	
	for ( int i=0; i<LIGHT_MAX_LIGHTS; i++ )
	{
		bool enabled = Lights[i].m_LightProperty.y;
		if ( enabled )
		{
			int lighttype = Lights[i].m_LightProperty.x;
			float4 vAmbient = Lights[i].m_vAmbient;
			float4 vDiffuse, vSpecular;
			float3 vLightDir;
			float  fLightAttenuation = 1.0f;
			float  fSpotLightEffect = 1.0f;
						
			if ( lighttype==LIGHT_DIRECTIONAL )
			// 方向光
			{
				vLightDir = -Lights[i].m_vDirection;
			}
			else
			// 點光源跟聚光燈
			{
				// 計算光的方向跟距離
				float3 vDiff = Lights[i].m_vPosition.xyz - vWorldPos;
				float fLightDistance = length(vDiff);
				vLightDir = vDiff / fLightDistance;

				fLightAttenuation = dot(float3(1.0f, fLightDistance, fLightDistance * fLightDistance), Lights[i].m_vAttenuation);
				
				if( lighttype==LIGHT_SPOT )
				// 聚光燈
				{
					// 計算聚光燈的效果
					float fSpotLightCosine = dot(-vLightDir, Lights[i].m_vDirection);
					fSpotLightCosine = fSpotLightCosine > Lights[i].m_fSpotlightCutoffCosine 
							? fSpotLightCosine : 0.0f;
					fSpotLightEffect = pow(fSpotLightCosine, Lights[i].m_fSpotlightExponent);
				}
			}

			// 光源的Diffuse部份
			vDiffuse = Lights[i].m_vDiffuse * saturate(dot(vLightDir, vWorldNormal));
			// 方源的Spcular部份
			float3 vCameraDir = normalize(vCameraPos - vWorldPos);
			float3 vHalfDir = normalize(vLightDir + vCameraDir);
			float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
			float  fSpecular = pow(fSpecularCosine, vShininess.x);
			vSpecular = Lights[i].m_vSpecular * fSpecular;
			// 			
			vDiffuse = vDiffuse/fLightAttenuation * fSpotLightEffect;
			vSpecular = vSpecular/fLightAttenuation * fSpotLightEffect;
			// 
			vDiffuseLight += vAmbient + vDiffuse;
			vSpecularLight += vSpecular;
		}
	}	
}

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	// 位置座標轉換
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	
	#if (_VERTEXCOLOR)
	{
		Out.Color = In.Color;
	}
	#else
	{
		Out.Color = vDiffuse;
	}
	#endif
	
	#if ( _CUBEMAP || _LIGHTING )
	{
		Out.WorldPos = mul(float4(In.Position, 1.0f), world_matrix);
		Out.WorldNormal = mul(In.Normal, (float3x3) world_matrix);
	}
	#endif

	#if ( _DIFFUSETEX0 || _DIFFUSETEX1 )
	{	
		float4 UV = 0;
		
		#if ( _DIFFUSETEX0 )
		{
			// 貼圖座標轉換
			float4 UV0 = mul(float4(In.UV0, 0.0f, 1.0f), texcoord_matrix[0]);
			UV0 /= UV0.w;
			UV.xy = UV0.xy;
		}
		#endif
		
		#if ( _DIFFUSETEX1 )
		{
			// 貼圖座標轉換
			float4 UV1 = mul(float4(In.UV1, 0.0f, 1.0f), texcoord_matrix[1]);
			UV1 /= UV1.w;
			UV.zw = UV1.xy;
		}
		#endif
		
		Out.UV = UV;
	}
	#endif
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 vColor = 0;
	float4 vSpecular = 0;
	float4 vDiffuse = 0;
#if (_CUBEMAP || _LIGHTING)
	float3 vWorldNormal = normalize(In.WorldNormal.xyz);
#endif
	
	#if (_LIGHTING)
	{
		CalculateLighting(In.WorldPos, vWorldNormal, vDiffuse, vSpecular);
		vColor = vDiffuse;
	}
	#else
	{
		vColor = In.Color;
	}
	#endif
	
	#if (_DIFFUSETEX0)
	{
		vColor *= DiffuseTexture0.Sample(DiffuseTextureSampler0, In.UV.xy);
	}
	#endif
	
	#if (_DIFFUSETEX1)
	{
		vColor *= DiffuseTexture1.Sample(DiffuseTextureSampler1, In.UV.zw);
	}
	#endif
	
	#if (_CUBEMAP)
	{
		float3 WorldPos = In.WorldPos;
		float3 CameraPos = inv_view_matrix[3].xyz;
		float3 CameraRay = normalize(WorldPos - CameraPos);
		float3 ReflectedRay = reflect(CameraRay, vWorldNormal);
		vColor += CubemapTexture.Sample(CubemapTextureSampler, ReflectedRay);
		//vColor.rgb = ReflectedRay * 0.5f + 0.5f;
	}
	#endif
	
	#if (_LIGHTING)
	{
		vColor += vSpecular;
	}
	#endif
	
	return vColor;
}
