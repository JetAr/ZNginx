#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

int g_iMode = 0x01|0x02|0x04;

// user control object
CGutUserControl g_Control;

// teapot model
CGutModel g_Model;
CGutModel g_Sphere;

// camera settings
float g_fFOV = 45.0f;
float g_fCameraNearZ = 0.1f;
float g_fCameraFarZ = 100.0f;

// light settings
float g_fLightFOV = 45.0f;
float g_fLightNearZ = 0.5f;
float g_fLightFarZ = 20.0f;

// Position, Texcoord
Vertex_VT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 0.0f}}
};

sGutLight g_Lights[NUM_LIGHTS];
Vertex_VC g_LightsVC[NUM_LIGHTS];
