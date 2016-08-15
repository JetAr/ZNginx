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

bool g_bPosteffect = false;

float g_weight_table[KERNELSIZE] = {1.0f, 1.5f, 3.0f, 4.0f, 5.0f, 4.0f, 3.0f, 1.5f, 1.0f};
float g_uv_offset_table[KERNELSIZE] = {-4.0f,-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f};

Vector4 g_vBrightnessScale(1.5f);
Vector4 g_vBrightnessOffset(0.0f);