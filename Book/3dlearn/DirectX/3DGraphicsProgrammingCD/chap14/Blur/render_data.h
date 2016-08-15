#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutUserControl.h"
#include "GutModel.h"

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

#define KERNELSIZE 9

extern float g_uv_offset_table[KERNELSIZE];
extern float g_weight_table[KERNELSIZE];

//
extern bool g_bPosteffect;
// 矩形
extern Vertex_VT g_FullScreenQuad[4];
