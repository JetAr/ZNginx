// 設定頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

// 轉換矩陣
uniform row_major float4x4 viewproj_matrix;
// 顏色
uniform float4 color;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( In.Position, viewproj_matrix);
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	return color;
}
