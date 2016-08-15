// 設定頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : TEXCOORD0;
};

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
uniform row_major float4x4 world_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	float3 position = In.Position.xyz * 2.0f - 1.0f;
	float3 normal = In.Normal.xyz * 2.0f - 1.0f;
	Out.Position = mul( float4(position, 1), wvp_matrix);
	Out.Normal = mul( normal, (float3x3) world_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float3 vLightDir = float3(0.0f, 1.0f, 0.0f);
	float4 vDot = dot(vLightDir, In.Normal);
	
	return vDot;
}
