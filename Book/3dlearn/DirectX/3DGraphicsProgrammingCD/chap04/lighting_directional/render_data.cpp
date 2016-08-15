#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// 鏡頭位置
Vector4 g_eye(0.0f, 0.0f, 2.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// 鏡頭轉換矩陣
Matrix4x4 g_view_matrix;
// 物件旋轉矩陣
Matrix4x4 g_world_matrix;
// `光源資料`
Vector4 g_vLightAmbient(0.1f, 0.1f, 0.1f, 0.1f);
Vector4 g_vLightDirection(0.0f, 0.0f, 1.0f, 0.0f);
Vector4 g_vLightColor(1.0f, 1.0f, 1.0f, 1.0f);
// 矩形的4個頂點
// Position, Color, Normal
Vertex_VCN g_Quad[4] =
{
	{ Vector4(-0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4( 0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4(-0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4( 0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) }
};

Vertex_DX9 g_Quad_dx9[4];

// `把顏色初值設定成環境光`
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		pVertices[i].m_Color = g_vLightAmbient;
	}
}

// `計算方向光, 它只跟頂點面向和光源方向有關.`
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		// `求出轉換後在世界座標系的頂點面向, RotateVector函式只做旋轉, 忽略位移.`
		Vector4 vNormal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `頂點面向跟光線方向的交角, 決定反射光的強度.`
		Vector4 vIntensity = Vector3Dot(vNormal, g_vLightDirection);
		// `把intensity局限在0-1的范圍`
		vIntensity.Clamp_to_0();
		// `累加上計算出來方向光的強度`
		pVertices[i].m_Color += vIntensity * g_vLightColor;
		pVertices[i].m_Color = pVertices[i].m_Color.Clamp_to_1();
	}
}

void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		g_quad_in[i].m_Position.StoreXYZ(g_quad_out[i].m_Position);
		GutConvertColor(g_quad_in[i].m_Color, g_quad_out[i].m_Color);
	}
}
