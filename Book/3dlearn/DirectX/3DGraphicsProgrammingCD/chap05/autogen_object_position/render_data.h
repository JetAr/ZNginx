#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_V
{
	float m_Position[3]; // 頂點位置
};

// 常數定義
const float g_fFovW = 45.0f;
// 矩形
extern Vertex_V g_Quad[4];
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
//
extern int g_iFilterMode;
extern bool g_bAnisotropic;