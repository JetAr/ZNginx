#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

bool g_bBlur = false;

int g_iMode = 0x01|0x02|0x04|0x08;
// user control object
CGutUserControl g_Control;
// teapot model
CGutModel g_Model;
// camera settings
float g_fFOV = 45.0f;
float g_fCameraNearZ = 0.1f;
float g_fCameraFarZ = 100.0f;

// Position, Texcoord
Vertex_VT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 0.0f}}
};

Vertex_V3T2 g_Quad[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 0.0f}}
};

sGutLight g_Lights[NUM_LIGHTS];
//
Vector4 g_vPosition(0.0f, 0.0f, 0.0f, 1.0f);
//
int g_iWidth = 512;
int g_iHeight = 512;

float g_fRippleSize = 10.0f;

