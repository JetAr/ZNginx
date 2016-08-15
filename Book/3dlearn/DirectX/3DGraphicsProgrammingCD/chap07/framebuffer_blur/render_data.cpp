#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

//
Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
// 
bool g_bBlur = true;
// 模型
CGutModel g_Models[4];
// controller object
CGutUserControl g_Control;
//
Vertex_V3T2 g_Quad[4] =
{
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{-1.0f,-1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 1.0f,-1.0f, 0.0f}, {1.0f, 1.0f}}
};