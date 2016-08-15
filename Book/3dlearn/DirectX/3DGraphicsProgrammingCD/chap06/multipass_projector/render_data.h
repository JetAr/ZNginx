#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_LookAt;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};

extern Light g_Light;
extern Vector4 g_vAmbientLight;

extern bool g_bPass0;
extern bool g_bPass1;

// 常數定義
const float g_fFovW = 45.0f;
// 矩形
extern Vertex_VT g_Quad[4];
extern Vertex_VT g_FullScreenQuad[4];
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
extern int g_iMode;
//
extern CGutModel g_Model;