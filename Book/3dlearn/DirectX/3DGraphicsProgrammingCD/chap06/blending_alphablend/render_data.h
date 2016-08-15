#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

struct Vertex_VCT
{
	float m_Position[3]; // 頂點位置
	unsigned char m_Color[4];
	float m_Texcoord[2]; // 貼圖座標
};

// 常數定義
const float g_fFovW = 45.0f;
// 矩形
extern Vertex_VCT g_Quad[4];
extern Vertex_VCT g_FullScreenQuad[4];
// controller object
extern CGutUserControl g_Control;
//
extern int g_iMode;
