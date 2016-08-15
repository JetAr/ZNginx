// `頂點的資料格式`
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

// `設定Vertex Shader輸出的資料格式`
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
};

// `轉換矩陣`
uniform row_major float4x4 g_worldviewproj_matrix : register(c0);
uniform row_major float4x4 g_world_matrix : register(c4);
// `鏡頭位置`
uniform float4 g_camera_position  : register(c8);
// `環境光`
uniform float4 g_Ambient : register(c12);
// `方向光`
uniform float3 g_Light_Direction : register(c13);
// `材質和光源相乘的結果`
uniform float4 g_Light_Ambient : register(c14);
uniform float4 g_Light_Diffuse : register(c15);
uniform float4 g_Light_Specular : register(c16);
// `Specular的亮點集中程度`
uniform float  g_Light_Shininess : register(c17);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// `轉換到螢幕座標`
	Out.Position = mul( In.Position, g_worldviewproj_matrix);
	
	// `計算Normal, Position在世界座標系上的方向跟位置`
	float3 vWorldNormal = mul( In.Normal, (float3x3) g_world_matrix);
	float3 vWorldPosition = mul( In.Position, g_world_matrix).xyz;
	// `初值`
	float4 vAmbient = g_Ambient + g_Light_Ambient;
	// `方向光的Diffuse部份`
	float4 vDiffuse = g_Light_Diffuse * saturate(dot(g_Light_Direction, vWorldNormal));
	
	// `方向光的Spcular部份`
	float3 vCameraPosition = g_camera_position.xyz;
	float3 vCameraDir = normalize(vCameraPosition - vWorldPosition);
	float3 vHalfDir = normalize(g_Light_Direction + vCameraDir);
	float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
	float  fSpecular = pow(fSpecularCosine, g_Light_Shininess);
	float4 vSpecular = g_Light_Specular * fSpecular;
	
	// `全部加起來`
	Out.Color = vAmbient + vDiffuse + vSpecular;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// `使用頂點間內插出來的顏色`
	return In.Color;
}
