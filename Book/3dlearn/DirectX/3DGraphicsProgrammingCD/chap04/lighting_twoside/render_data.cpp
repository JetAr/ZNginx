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
// 光源資料
Vector4 g_vGlobal_AmbientLight(0.1f, 0.1f, 0.1f, 1.0f);
Vector4 g_vLightDirection(0.0f, 0.0f, -1.0f, 0.0f);
Vector4 g_vLightAmbient(0.0f, 0.0f, 0.0f, 0.0f);
Vector4 g_vLightDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
Vector4 g_vLightSpecular(0.0f, 0.0f, 0.0f, 0.0f);
// 正面材質
Vector4 g_vMaterialAmbient(1.0f, 0.0f, 0.0f, 1.0f);
Vector4 g_vMaterialDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
Vector4 g_vMaterialSpecular(0.0f, 0.0f, 0.0f, 1.0f);
Vector4 g_vMaterialEmissive(0.0f, 0.0f, 0.0f, 0.0f);
float	g_fMaterialShininess = 0.0f;
// 背面材質
Vector4 g_vMaterialAmbient_Back(0.0f, 1.0f, 0.0f, 1.0f);
Vector4 g_vMaterialDiffuse_Back(0.0f, 1.0f, 0.0f, 1.0f);
Vector4 g_vMaterialSpecular_Back(0.0f, 0.0f, 0.0f, 1.0f);
Vector4 g_vMaterialEmissive_Back(0.0f, 0.0f, 0.0f, 0.0f);
float	g_fMaterialShininess_Back = 0.0f;

// 矩形的4個頂點
// Position, Normal
Vertex_VN g_Quad[4] =
{
	{{ -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f}},
	{{  0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f}},
	{{ -0.5f,  0.5f, 0.0f }, {0.0f, 0.0f, 1.0f}},
	{{  0.5f,  0.5f, 0.0f }, {0.0f, 0.0f, 1.0f}}
};
