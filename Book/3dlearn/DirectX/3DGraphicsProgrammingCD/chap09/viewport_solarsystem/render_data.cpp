#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;

Vertex_VC g_Border[4] = 
{
	{{-1.0f, 0.0f, 0.0f}, {255,255,255,255}},
	{{ 1.0f, 0.0f, 0.0f}, {255,255,255,255}},
	{{ 0.0f,-1.0f, 0.0f}, {255,255,255,255}},
	{{ 0.0f, 1.0f, 0.0f}, {255,255,255,255}}
};

Light g_Light;
Vector4 g_vAmbientLight(0.3f, 0.3f, 0.3f, 1.0f);

// controller object
CGutUserControl g_Control;
// 模型
CGutModel g_Models[3];
