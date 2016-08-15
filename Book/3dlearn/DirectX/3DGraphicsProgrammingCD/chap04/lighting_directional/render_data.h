#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VCN
{
	Vector4 m_Position; // 頂點位置
	Vector4 m_Color;	// 頂點顏色
	Vector4 m_Normal;	// 頂點面向
};

struct Vertex_DX9
{
	float m_Position[3];	// 頂點位置
	unsigned char m_Color[4]; // 頂點顏色
};

// 常數定義
const float g_fFovW = 45.0f;
extern Vertex_VCN g_Quad[4];
extern Vertex_DX9 g_Quad_dx9[4];

// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 
// 鏡頭轉換矩陣
extern Matrix4x4 g_view_matrix;
// 物件旋轉矩陣
extern Matrix4x4 g_world_matrix;
// 光源資料
extern Vector4 g_vLightAmbient;
extern Vector4 g_vLightDirection;
extern Vector4 g_vLightColor;

// 計算打光的函式
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices);

void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices);
