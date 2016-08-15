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

extern sGutLight g_Light0;
extern Vector4 g_vAmbientLight;
extern float g_fMaterialShininess;

#define KERNELSIZE 9

extern float g_uv_offset_table[KERNELSIZE];
extern float g_weight_table[KERNELSIZE];

extern Vector4 g_vBrightnessOffset;
extern Vector4 g_vBrightnessScale;

//
extern bool g_bPosteffect;
// 矩形
extern Vertex_VT g_FullScreenQuad[4];
extern Vertex_VT g_FullScreenQuadInv[4];
//
extern float g_fFovW;
extern float g_fNearZ;
extern float g_fFarZ;
//
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
//
extern CGutModel g_ModelSun;
extern CGutModel g_ModelEarth;
//
extern CGutUserControl g_Control;