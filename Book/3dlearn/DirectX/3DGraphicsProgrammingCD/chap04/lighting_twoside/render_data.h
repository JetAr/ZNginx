#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VN
{
	float m_Position[3];	// 頂點位置
	float m_Normal[3];		// 頂點面向
};

// 常數定義
const float g_fFovW = 45.0f;
extern Vertex_VN g_Quad[4];

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
extern Vector4 g_vGlobal_AmbientLight;
extern Vector4 g_vLightAmbient;
extern Vector4 g_vLightDirection;
extern Vector4 g_vLightDiffuse;
extern Vector4 g_vLightSpecular;
// 正面材質
extern Vector4 g_vMaterialAmbient;
extern Vector4 g_vMaterialDiffuse;
extern Vector4 g_vMaterialSpecular;
extern Vector4 g_vMaterialEmissive;
extern float   g_fMaterialShininess;
// 背面材質
extern Vector4 g_vMaterialAmbient_Back;
extern Vector4 g_vMaterialDiffuse_Back;
extern Vector4 g_vMaterialSpecular_Back;
extern Vector4 g_vMaterialEmissive_Back;
extern float   g_fMaterialShininess_Back;
