#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

struct Vertex_VC
{
	float m_Position[3];
	// R,G,B,A in OpenGL
	// B,G,R,A in Direct3D 9
	// Direct3D 10可是是兩種其中之1, 看設定而定.
	unsigned char m_RGBA[4];
};

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

// 矩形
extern Vertex_VT g_Quad[4];
// controller object
extern CGutUserControl g_Control;

// 球的資料
extern Vertex_VC *g_pSunVertices;
extern Vertex_VC *g_pEarthVertices;
extern Vertex_VC *g_pMoonVertices;
extern unsigned short *g_pSphereIndices;
// 邊線
extern Vector4 g_Border[4];

extern int g_iNumSphereVertices;
extern int g_iNumSphereTriangles;
extern int g_iNumSphereIndices;

// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 
// 鏡頭轉換矩陣
extern Matrix4x4 g_view_matrix;

extern Matrix4x4 g_world_matrix;
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;

// 產生球模型的程式
bool CreateSphere(float radius, Vertex_VC **ppVertices, unsigned short **ppIndices, float *color=NULL, int stacks = 20, int pieces = 20);
