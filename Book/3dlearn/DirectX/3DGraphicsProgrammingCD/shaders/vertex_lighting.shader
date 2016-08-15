// 頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color	: COLOR;
};

uniform row_major float4x4 g_worldviewproj_matrix : register(c0);
uniform row_major float4x4 g_world_matrix : register(c4);

// 環境光
uniform float4 g_Ambient : register(c8);
// 方向光
uniform float4 g_Light0_Direction : register(c9);
uniform float4 g_Light0_Ambient : register(c10);
uniform float4 g_Light0_Diffuse : register(c11);
uniform float4 g_Light0_Specular : register(c12);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( In.Position, g_worldviewproj_matrix);
	float3 vWorld_Position = mul( In.Position, g_world_matrix);
	float3 vWorld_Normal = mul( In.Normal, (float3x3) g_world_matrix);
	
	// 初值	
	float4 vLighting = g_Ambient + g_Light0_Ambient;
	// 方向光
	vLighting += g_Light0_Diffuse * saturate(dot(g_Light0_Direction, vWorld_Normal));
		
	Out.Color = vLighting;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	// 使用頂點間內插出來的顏色
	return In.Color;
}
