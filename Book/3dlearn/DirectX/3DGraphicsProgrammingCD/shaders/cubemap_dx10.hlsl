// 設定頂點的資料格式
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float3 WorldPos : TEXCOORD0;
	float3 WorldNormal : TEXCOORD1;
};

TextureCube CubemapTexture;
SamplerState CubemapSampler;

// 轉換矩陣
uniform row_major float4x4 wvp_matrix;
uniform row_major float4x4 world_matrix;
uniform row_major float4x4 inv_view_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position.xyz, 1), wvp_matrix);
	Out.WorldPos = mul(float4(In.Position, 1.0f), world_matrix);
	Out.WorldNormal = mul(In.Normal, (float3x3) world_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float3 WorldPos = In.WorldPos;
	float3 WorldNormal = normalize(In.WorldNormal);
	float3 CameraPos = inv_view_matrix[3].xyz;
	float3 CameraRay = normalize(WorldPos - CameraPos);
	float3 ReflectedRay = reflect(CameraRay, WorldNormal);
	
	float4 color = CubemapTexture.Sample(CubemapSampler, ReflectedRay);
	
	return color;
}
