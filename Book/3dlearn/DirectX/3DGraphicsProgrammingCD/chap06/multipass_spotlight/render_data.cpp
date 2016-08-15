#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

int g_iWindowWidth = 512;
int g_iWindowHeight = 512;

float g_fOrthoSize = 5.0f;
float g_fOrthoWidth = g_fOrthoSize;
float g_fOrthoHeight = g_fOrthoSize;

// 鏡頭位置
Vector4 g_eye(0.0f, 0.0f, 5.0f); 
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
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}}
};

Vertex_VT g_FullScreenQuad[4] =
{
	{{0.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}}
};

int g_iMode = 1;