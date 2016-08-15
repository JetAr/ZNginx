#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutDefs.h"
#include "GutModel.h"
#include "GutUserControl.h"

#define KERNELSIZE 9

extern float g_uv_offset_table[KERNELSIZE];
extern float g_weight_table[KERNELSIZE];
extern bool g_bBlur;
extern int g_iMode;

//
const int shadowmap_size = 1024;
const int PCF_samples_row = 5;
const int PCF_samples = (PCF_samples_row * PCF_samples_row);

const int Interpolated_PCF_samples = 16;
// 
extern CGutUserControl g_Control;
extern CGutModel g_Model;
// camera fov
extern float g_fFOV;
// spotlight fov
extern float g_fLightFOV;
extern float g_fLightNearZ, g_fLightFarZ;
extern Vector4 g_vLightPos;
extern Vector4 g_vLightLookAt;
extern Vector4 g_vLightUp;
// 
extern float g_fZBias;
//
extern Vector4 g_vTexOffset[PCF_samples];
extern Vector4 g_vTexOffset2[PCF_samples];

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

extern Vertex_VT g_FullScreenQuad[4];
extern Vertex_V3T2 g_Quad[4];

#define NUM_LIGHTS 2

extern sGutLight g_Lights[NUM_LIGHTS];

extern Vector4 g_vPosition;

extern int g_iWidth, g_iHeight;
extern float g_fRippleSize;

extern CGutModel g_Model;