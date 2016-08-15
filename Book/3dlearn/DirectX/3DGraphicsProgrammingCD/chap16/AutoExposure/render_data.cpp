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
//
bool g_bPosteffect = true;
//
float g_fFovW = 60.0f;
float g_fNearZ = 0.1f;
float g_fFarZ = 100.0f;
//
CGutModel g_Model;
//
CGutUserControl g_Control;
//
int g_iMode = 1;