#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutDefs.h"
#include "GutUserControl.h"
#include "GutModel.h"

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};
//
extern bool g_bPosteffect;
//
extern int g_iMode;
// 矩形
extern Vertex_VT g_FullScreenQuad[4];
extern Vertex_VT g_FullScreenQuadInv[4];
//
extern float g_fFovW;
extern float g_fNearZ;
extern float g_fFarZ;
//
extern CGutModel g_Model;
//
extern CGutUserControl g_Control;