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
	float m_Position[3];
	float m_Normal[3];
	unsigned char m_Color[4];
};

extern int g_iWindowWidth, g_iWindowHeight;
extern float g_fOrthoWidth, g_fOrthoHeight;
extern float g_fOrthoSize;

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
#define MAX_ACTIVATED_LIGHTS 2

struct Light
{
	Vector4 m_Position;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};

extern float g_fMaterialShininess;
extern Vector4 g_vAmbientLight;

extern int g_iNumActivatedLights;
extern Light g_ActivatedLights[MAX_ACTIVATED_LIGHTS];
//
#define NUM_LIGHTS 16
extern Vertex_VCN g_LightPosition[NUM_LIGHTS];
extern Vertex_DX9 g_LightPosition_dx9[NUM_LIGHTS];

void LightManager(void);
void ConvertToDX9(Vertex_VCN g_vertices_in[], Vertex_DX9 g_vertices_out[], int num_vertices);
