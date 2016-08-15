#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// Position, Texcoord
Vertex_VT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 0.0f}}
};

// Position, Texcoord
Vertex_VT g_FullScreenQuadInv[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 0.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 1.0f}}
};

bool g_bPosteffect = true;

float g_weight_table[KERNELSIZE] = {1.0f, 1.5f, 3.0f, 4.0f, 5.0f, 4.0f, 3.0f, 1.5f, 1.0f};
float g_uv_offset_table[KERNELSIZE] = {-4.0f,-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
//float g_uv_offset_table[KERNELSIZE] = {-7.5f, -5.5f, -3.5f, -1.5f, 0.0f, 1.5f, 3.5f, 5.5f, 7.5f};

Vector4 g_vBrightnessScale(1.0f);
Vector4 g_vBrightnessOffset(0.0f);
//
float g_fFovW = 60.0f;
float g_fNearZ = 0.1f;
float g_fFarZ = 100.0f;
//
Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
//
sGutLight g_Light0;
//
Vector4 g_vAmbientLight(0.1f);
float g_fMaterialShininess = 20.0f;
//
CGutModel g_ModelSun;
CGutModel g_ModelEarth;
//
CGutUserControl g_Control;
//
Vector4 g_vDepthOfField(3.0f, 1.0f, 7.0f, 1.0f);