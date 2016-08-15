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
	float m_Position[3];	// 頂點位置
	unsigned char m_Color[4]; // 頂點顏色
};

// 常數定義
const float g_fFovW = 45.0f;
const int	g_iMaxNumGrids = 100;
const int	g_iMaxNumGridVertices = (g_iMaxNumGrids+1) * (g_iMaxNumGrids+1);
const int	g_iMaxNumGridIndices = g_iMaxNumGrids * (g_iMaxNumGrids * 2 + 2);

// 格子模型資料
extern int	g_iNumGridVertices;
extern int	g_iNumGridIndices;
extern int	g_iNumGridTriangles;
extern Vertex_VCN *g_pGridVertices;
extern Vertex_DX9 *g_pGridVerticesDX9;
extern unsigned short *g_pGridIndices;

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
extern Vector4 g_vLightAmbient;
extern Vector4 g_vLightDirection;
extern Vector4 g_vLightPosition;
extern Vector4 g_vLightColor;
extern Vector4 g_vLightAttenuation;
extern float   g_fSpotLightCutoff;
extern float   g_fSpotLightExponent;

//
void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices);
// 產生格子的函式
bool GenerateGrids(int x_grids, int y_grids, 
				   Vertex_VCN **ppVertices, int &num_vertices, 
				   unsigned short **ppIndices, int &num_indices,
				   int &num_triangles);
// 釋放格子模型資源
bool ReleaseGridsResource(Vertex_VCN **ppVertices, unsigned short **ppIndices);
// 計算打光的函式
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices);
void CalculatePointLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateSpotLight(Vertex_VCN *pVertices, int num_vertices);
