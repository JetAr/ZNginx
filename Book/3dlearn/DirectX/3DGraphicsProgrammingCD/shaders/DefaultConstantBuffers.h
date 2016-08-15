
cbuffer MatrixConstants : register(b0)
{
	// 轉換矩陣
	row_major float4x4 wvp_matrix;
	row_major float4x4 view_matrix;
	row_major float4x4 world_matrix;
	row_major float4x4 inv_view_matrix;
	row_major float4x4 normal_matrix;
	row_major float4x4 texcoord_matrix[4];
	row_major float4x4 light_wvp_matrix;
	row_major float4x4 light_wv_matrix;
}

cbuffer MtlConstants : register(b1)
{
	float4 vEmissive;
	float4 vAmbient;
	float4 vDiffuse;
	float4 vSpecular;
	float4 vShininess;
}

#define LIGHT_POINT		0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_SPOT		2
#define LIGHT_MAX_LIGHTS 8

struct Light
{
	// position & orientation
	float4 m_vPosition;
	float4 m_vDirection;
	// color & intensity
	float4 m_vAmbient;
	float4 m_vDiffuse;
	float4 m_vSpecular;
	// attenuation
	float4 m_vAttenuation;
	// spotlight effect
	float  m_fSpotlightInnerCone;
	float  m_fSpotlightCutoffCosine;
	float  m_fSpotlightExponent;	
	float  m_fPadding;
	// type & enable/disable
	int4   m_LightProperty;
	//int	   m_iLightType; // m_LightProperty.x
	//bool   m_bEnabled;	// m_LightProperty.y
};

// 光源
cbuffer Lights : register(b2)
{
	uniform float4 g_vAmbientLight;
	Light Lights[LIGHT_MAX_LIGHTS];
};
