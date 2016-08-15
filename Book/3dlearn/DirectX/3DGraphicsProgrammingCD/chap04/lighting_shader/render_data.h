#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutDefs.h"

const int g_iNumLights = 3;

struct Light_Info
{
	Vector4 m_vPosition;
	Vector4 m_vDirection;

	Vector4 m_vAmbientColor;
	Vector4 m_vDiffuseColor;
	Vector4 m_vSpecularColor;
	Vector4 m_vAttenuation;

	float	m_fSpotlightCutoff;
	float	m_fSpotlightExponent;

	int		m_eType;
	bool	m_bEnabled;

	Light_Info(void)
	{
		m_vPosition = Vector4::GetZero();
		m_vDirection.Set(0.0f, 0.0f, -1.0f, 0.0f);

		m_vAmbientColor.Set (0.0f, 0.0f, 0.0f, 1.0f);
		m_vDiffuseColor.Set (1.0f, 1.0f, 1.0f, 1.0f);
		m_vSpecularColor.Set(0.0f, 0.0f, 0.0f, 1.0f);

		m_vAttenuation.Set(1.0f, 0.0f, 0.0f);

		m_fSpotlightCutoff = 180.0f;
		m_fSpotlightExponent = 1.0f;

		m_eType = LIGHT_DIRECTIONAL;
		m_bEnabled = false;
	}
};

// 光源
extern Light_Info g_Lights[];
extern Vector4 g_vAmbientLight;
extern int  g_iSelectedLight;
// 常數定義
const float g_fFovW = 45.0f;
const int	g_iMaxNumGrids = 100;
const int	g_iMaxNumGridVertices = (g_iMaxNumGrids+1) * (g_iMaxNumGrids+1);
const int	g_iMaxNumGridIndices = g_iMaxNumGrids * (g_iMaxNumGrids * 2 + 2);

// 格子模型資料
extern int	g_iNumGridVertices;
extern int	g_iNumGridIndices;
extern int	g_iNumGridTriangles;
extern Vertex_V3N3 *g_pGridVertices;
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
// 物件材質
extern Vector4 g_vMaterialAmbient;
extern Vector4 g_vMaterialDiffuse;
extern Vector4 g_vMaterialSpecular;
extern Vector4 g_vMaterialEmissive;
extern float   g_fMaterialShininess;
// 打光設定
extern bool g_bLocalViewer;
extern bool g_bAutoNormalize;
extern bool g_bDiffuseReflection;
extern bool g_bSpecularReflection;

/*
// 產生格子的函式
bool GenerateGrids(int x_grids, int y_grids, 
Vertex_VN **ppVertices, int &num_vertices, 
unsigned short **ppIndices, int &num_indices,
int &num_triangles);
// 釋放格子模型資源
bool ReleaseGridsResource(Vertex_VN **ppVertices, unsigned short **ppIndices);
*/
