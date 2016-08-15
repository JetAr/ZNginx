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
// 矩形的4個頂點
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
	{{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}}
};
