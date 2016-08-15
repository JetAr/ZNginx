// 設定頂點的資料格式
struct VS_INPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
};

// 設定Vertex Shader輸出的資料格式
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Color	: COLOR;
};

// 轉換矩陣
uniform row_major float4x4 viewproj_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// 座標轉換
	Out.Position = mul( In.Position, viewproj_matrix);
	// 讀取頂點的顏色
	Out.Color = In.Color;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	// 使用頂點間內插出來的顏色
	return In.Color;
}
